#pragma once
#include "Character.h"
#include "../collision/CollisionHandler.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Character::Character()
	: Moveable()
	, m_currentTeam(0)
	, m_respawnTime(1)
{
	m_inputDevice = { 1, 0 };
	m_input = {Vector3(0.f,0.f,0.f), Vector3(1.f,0.f,0.f)};
	m_movement = { 0.f, 10.f, 0.f, 1.0f};
	m_playerHealth.setMax(100.f);
	m_playerHealth.setHealth(100.f);
	m_playerHealth.regen = 5.f;
	m_vibration[0] = { 0, 0};
	m_vibFreq = 1.f / 30.f;
	m_vibDeltaAcc = 0;
	m_resetAttacker = 0.0f;
	m_lastAttackerIndex = -1;

	getTransform().setRotations(Vector3(0.0f, 1.57f, 0.0f));

	m_thrusterEmitter = std::shared_ptr<ParticleEmitter>(new ParticleEmitter(ParticleEmitter::EXPLOSION, Vector3(-1.f, 0.f, 0.f), 
		Vector3(-0.5f, 0.f, -0.5f), Vector3(5.f, -5.f, 0.5f), 500.f, 200, 0.15f, 0.3f, lightColor, 1.f, 0U, true, false));
	setLightColor(Vector4(1, 1, 1, 1));

	m_nextRespawnPoint = Vector3::Zero;

	//addVibration(0, 1.f, 2.f);
}

Character::Character(Model * bodyModel, Model * lArmModel, Model* headModel, int index) : Character() {
	this->setModel(bodyModel);
	this->updateBoundingBox();
	m_leftArm = lArmModel;
	m_head = headModel;
	m_playerIndex = index;
}
Character::Character(Model * bodyModel, Model * lArmModel, Model* headModel, int index, unsigned int usingController, unsigned int port)
	: Character(bodyModel, lArmModel, headModel, index) {
	this->setController(usingController);
	this->setControllerPort(port);
}

Character::~Character() {
	Memory::safeDelete(m_weapon);
	Memory::safeDelete(m_hook);
}

void Character::processInput() {
	if (!m_playerHealth.alive)
		return;
	

	if (!m_inputDevice.controller) {

		const Keyboard::State& keyState = Application::getInstance()->getInput().getKeyboardState();
		const Keyboard::KeyboardStateTracker& keyTracker = Application::getInstance()->getInput().getKbStateTracker();
		m_input.movement = Vector3(
			(float)keyState.IsKeyDown(Keyboard::D) - (float)keyState.IsKeyDown(Keyboard::A),
			(float)keyState.IsKeyDown(Keyboard::W) - (float)keyState.IsKeyDown(Keyboard::S),
			0);

	
		if (keyTracker.pressed.V) {

		}
		//update aim Direction
		Vector3 tempVec = Vector3(
			(float)keyState.IsKeyDown(Keyboard::Right) - (float)keyState.IsKeyDown(Keyboard::Left),
			(float)keyState.IsKeyDown(Keyboard::Up) - (float)keyState.IsKeyDown(Keyboard::Down),
			0);

		if (tempVec.LengthSquared() > 0.3) {
			tempVec.Normalize();
			m_input.aim = tempVec;
		}


		if (keyTracker.pressed.Space) {
			jump();
		}
	
		if (keyTracker.pressed.Z) {
			m_weapon->triggerPull();
		}
		if (keyTracker.released.Z) {
			m_weapon->triggerRelease();
		}
		if (keyTracker.pressed.X) {
			hook();
		}
		if (keyTracker.released.X) {
			stopHook();
		}



	}
	else {

		const GamePad::State& padState = Application::getInstance()->getInput().getGamePadState(m_inputDevice.controllerPort);
		const GamePad::ButtonStateTracker& padTracker = Application::getInstance()->getInput().getGpStateTracker(m_inputDevice.controllerPort);

		if (padState.IsConnected()) {

			// ON BUTTON CLICK
			if (padTracker.a == GamePad::ButtonStateTracker::PRESSED) {
				this->jump();
			}
			if (padTracker.b == GamePad::ButtonStateTracker::PRESSED) {
				if (!m_movement.inCover && !m_movement.hooked) {
					DirectX::SimpleMath::Vector3 pos = getTransform().getTranslation();
					m_movement.inCover = CollisionHandler::getInstance()->resolveCoverCollision(pos);
				}
				else
					m_movement.inCover = false;
			}
			if (padTracker.x == GamePad::ButtonStateTracker::PRESSED) {
				m_playerHealth.addHealth(-10);
				//this->addVibration(0, 1);
			}
			if (padTracker.y == GamePad::ButtonStateTracker::PRESSED) {
				m_playerHealth.addHealth(10);
			}

			// ON BUTTON RELEASE
			if (padTracker.a == GamePad::ButtonStateTracker::RELEASED) {
				this->stopJump();
			}
			if (padTracker.b == GamePad::ButtonStateTracker::RELEASED) {
				
			}
			if (padTracker.x == GamePad::ButtonStateTracker::RELEASED) {
				
			}
			if (padTracker.y == GamePad::ButtonStateTracker::RELEASED) {
				//this->addVibration(0, 1);
			}		

			// ON BUTTON HOLD
			if (padState.buttons.a == GamePad::ButtonStateTracker::HELD) {
				
			}
			if (padState.buttons.b == GamePad::ButtonStateTracker::HELD) {
				
			}
			if (padState.buttons.x == GamePad::ButtonStateTracker::HELD) {
			
			}
			if (padState.buttons.y == GamePad::ButtonStateTracker::HELD) {
			
			}

			// ON TRIGGER HOLD

			if (padTracker.rightTrigger == GamePad::ButtonStateTracker::PRESSED) {
				m_weapon->triggerPull();
				
			}
			if (padTracker.rightTrigger == GamePad::ButtonStateTracker::RELEASED) {
				m_weapon->triggerRelease();

			}
			if (padTracker.leftTrigger == GamePad::ButtonStateTracker::PRESSED) {
				hook();
			}
			if (padTracker.leftTrigger == GamePad::ButtonStateTracker::RELEASED) {
				stopHook();
			}


			
			//update inputVector
			m_input.movement = Vector3(
				padState.thumbSticks.leftX,
				padState.thumbSticks.leftY, 
				0);
			//update aim Direction
			Vector3 tempVec = Vector3(
				padState.thumbSticks.rightX,
				padState.thumbSticks.rightY,
				0);

			if (tempVec.LengthSquared() > 0.1) {
				tempVec.Normalize();
				m_input.aim = tempVec;
			}
		}
	}
}

void Character::update(float dt) {
	Application* app = Application::getInstance();
	CollisionHandler* collHandler = CollisionHandler::getInstance();
	auto& gamePad = Application::getInstance()->getInput().getGamePad();

	if (updateVibration(dt)) {

		gamePad.SetVibration(m_inputDevice.controllerPort,
			m_vibration[0].currentStrength,
			m_vibration[1].currentStrength);
		/*Logger::log("0: " + std::to_string(m_vibration[0].currentStrength));
		Logger::log("1: " + std::to_string(m_vibration[1].currentStrength));*/

	}


	if (!m_playerHealth.alive) {
		// PLAYER IS DEAD

		m_thrusterEmitter->updateSpawnsPerSecond(0.f);

		// Smoothstep ghost to new spawn position
		setPosition(Vector3::SmoothStep(m_deathPoint, m_nextRespawnPoint, m_deathInterp));
		m_deathInterp += dt / m_respawnTime;

	} else {
		// PLAYER IS ALIVE

		m_thrusterEmitter->updateSpawnsPerSecond(500.f);
		m_resetAttacker += dt;
		if (m_playerHealth.current < m_playerHealth.max) {
			m_playerHealth.current += m_playerHealth.regen * dt;
		}

	if (collHandler->outOfBounds(this)) {
		damage(getMaxHealth());
	}
	
	if (m_resetAttacker >= 1.f)
		m_lastAttackerIndex = -1;

	m_playerHealth.updatePercent();

		if (!m_movement.inCover && getTransform().getTranslation().z == 0.f) {
			if (grounded()) {
				if (!m_movement.hooked) {//Movement while on the ground
					if(m_input.movement.x > 0)
						this->setVelocity(DirectX::SimpleMath::Vector3(min(this->getVelocity().x + 1.0f, m_movement.speed), this->getVelocity().y, 0.f));
					else if(m_input.movement.x < 0)
						this->setVelocity(DirectX::SimpleMath::Vector3(max(this->getVelocity().x - 1.0f, -m_movement.speed), this->getVelocity().y, 0.f));
					else
						this->setVelocity(DirectX::SimpleMath::Vector3(this->getVelocity().x, this->getVelocity().y, 0.f));

					if (fabs(this->getVelocity().x) > 5)
						this->setAcceleration(DirectX::SimpleMath::Vector3(this->getVelocity().x * -6.f, 0.f, 0.f));
					else if (this->getVelocity().x < -1)
						this->setAcceleration(DirectX::SimpleMath::Vector3(20.f, 0.f, 0.f));
					else if (this->getVelocity().x > 1)
						this->setAcceleration(DirectX::SimpleMath::Vector3(-20.f, 0.f, 0.f));
					else if (fabs(this->getVelocity().x) < 1.f) {
						this->setAcceleration(DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));
						this->setVelocity(DirectX::SimpleMath::Vector3(0.f, this->getVelocity().y, 0.f));
					}
				}
				else//Movement on the ground while using grappling hook
				{	
					if (fabs(this->getVelocity().x) <(m_movement.speed / 2.f)) {
						if (m_hook->getDirection().x >= 0) {
							this->setVelocity(DirectX::SimpleMath::Vector3(min(this->getVelocity().x + m_hook->getDirection().x * m_movement.speed, (m_movement.speed / 2.f)), this->getVelocity().y, 0.f));
						}
						else
						{
							this->setVelocity(DirectX::SimpleMath::Vector3(max(this->getVelocity().x + m_hook->getDirection().x * m_movement.speed, (-m_movement.speed / 2.f)), this->getVelocity().y, 0.f));
						}
					}
					this->setAcceleration(DirectX::SimpleMath::Vector3(0.f, fabs(m_hook->getDirection().y) * 20.0f, 0.f));
				}

			}
			else {
				if (!m_movement.hooked) {//Movement in the air
					float velX = getVelocity().x;
					if ((m_input.movement.x > 0.f && velX <= m_movement.speed * 0.8f) || 
						(m_input.movement.x < 0.f && velX >= -m_movement.speed * 0.8f)) {
						velX += m_input.movement.x * m_movement.speed * 0.1f;
					}
					//velX = max(min(velX, m_movement.speed * 0.8f), -m_movement.speed * 0.8f);
					this->setVelocity(DirectX::SimpleMath::Vector3(velX, this->getVelocity().y, 0.f));
					this->setAcceleration(DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));
				}
				else {//Movement in the air while using grappling hook
					DirectX::SimpleMath::Vector3 currVelocity = this->getVelocity();
					currVelocity.Normalize();
					if (m_hook->getDirection().Dot(currVelocity) < 0.f) {
						DirectX::SimpleMath::Vector3 tempVec = m_hook->getDirection().Cross(DirectX::SimpleMath::Vector3(0.f, 0.f, 1.f));

						tempVec = (this->getVelocity().Dot(tempVec) / tempVec.LengthSquared()) * tempVec;
						this->setVelocity(tempVec);
					}

					this->setAcceleration(m_hook->getDirection() * 20.0f);
				}
			}

		
			if (m_hook) {
				//m_hook->update(dt, m_weapon->getTransform().getTranslation() + m_hook->getDirection() * 0.60f + Vector3(0.0f, 0.0f, 0.28f - std::signbit(m_input.aim.x) * 0.56f)); //Hook starts from hand
				m_movement.hooked = m_hook->update(dt, getTransform().getTranslation() + Vector3(0.0f, 0.0f, 0.28f - std::signbit(m_input.aim.x) * 0.56f)); //Hook starts from shoulder
			}


			// Check for and handle if a projectile collides with this character
			Vector3 knockbackDir;
			float dmgTaken;
			float knockbackAmount;
			bool hit = collHandler->resolveProjectileCollisionWith(this, knockbackDir, dmgTaken, knockbackAmount, m_lastAttackerIndex);
			if (hit) {
				damage(dmgTaken);
				addVelocity(knockbackDir * knockbackAmount);
				setGrounded(false);
				m_resetAttacker = 0.0f;

				// Hit particle effet
				m_particleHandler->addEmitter(std::shared_ptr<ParticleEmitter>(new ParticleEmitter(
					ParticleEmitter::EXPLOSION, getTransform().getTranslation() - knockbackDir * 0.35f, Vector3(-0.5f), Vector3(15.f, 15.f, 4.f),
					0.f, 10, 0.4f, 0.3f, Vector4(0.5f, 0.5f, 0.5f, 1.0f), 0.2f, 10U, true, true)));

				VibrateController(0, 1.f, 1.f);
			}
		}

		//Going in and out of cover
		if (getTransform().getTranslation().z == 0.f && m_movement.inCover) {
			this->setVelocity(DirectX::SimpleMath::Vector3((((floor(getTransform().getTranslation().x)) - getTransform().getTranslation().x) + 0.5f) * m_movement.speed, 0.f, m_movement.speed));
		}
		else if (getTransform().getTranslation().z > 1.f && m_movement.inCover) {
			this->setVelocity(DirectX::SimpleMath::Vector3((((floor(getTransform().getTranslation().x)) - getTransform().getTranslation().x) + 0.5f) * m_movement.speed, 0.f, 0.f));
		}
		else if (getTransform().getTranslation().z > 0.f && !m_movement.inCover)
			this->setVelocity(DirectX::SimpleMath::Vector3(0.f, 0.f, -m_movement.speed));
		else if (getTransform().getTranslation().z < 0.f && !m_movement.inCover){
			this->setVelocity(DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));
		}
		if (getTransform().getTranslation().z < 0.f) {
			getTransform().setTranslation(DirectX::SimpleMath::Vector3(getTransform().getTranslation().x, getTransform().getTranslation().y, 0.f));
			m_movement.inCover = false;
		}


		//----Character turn animation----
		float tempRotation = getTransform().getRotations().y;
		if (std::signbit(m_input.aim.x) && tempRotation < 4.71f) {
			m_movement.xDirection = -1.0f;
			getTransform().setRotations(Vector3(0.0f, min(tempRotation + 15.7f * dt, 4.71f), 0.0f));
		}
		else if (!std::signbit(m_input.aim.x) && tempRotation > 1.57f) {
			m_movement.xDirection = 1.0f;
			getTransform().setRotations(Vector3(0.0f, max(tempRotation - 15.7f * dt, 1.57f), 0.0f));
		}
		//--------------------------------

		Moveable::updateVelocity(dt);
		collHandler->resolveLevelCollisionWith(this, dt);
		Moveable::move(dt, false);
		collHandler->resolveUpgradeCollisionWith(this);

		//----Weapon aim animation----
		if (m_weapon) {
			m_weapon->getTransform().setTranslation(getTransform().getTranslation());
			Matrix tempMatrix;
			tempMatrix *= Matrix::CreateRotationX(-m_movement.xDirection * (sinDegFromVec(m_input.aim) + 0.785f) + std::signbit(m_movement.xDirection) * 1.57f);
			m_weapon->getTransform().setMatrix(tempMatrix * getTransform().getMatrix());
			if (!m_movement.inCover) {
				m_weapon->update(dt, m_input.aim);
			}
		}
		//----------------------------
	}
}


void Character::draw() {
	//----hooking animations----
	Transform armTransform;
	Transform bodyTransform;
	if (m_movement.hooked && m_hook && !m_movement.inCover) {
		armTransform.rotateAroundX(-m_movement.xDirection * (sinDegFromVec(m_hook->getDirection()) + 1.57f));
		bodyTransform.rotateAroundX(-m_movement.xDirection * (sinDegFromVec(m_movement.xDirection * m_hook->getDirection())) * 0.3f);
		armTransform.setMatrix(armTransform.getMatrix() * getTransform().getMatrix());
		bodyTransform.setMatrix(bodyTransform.getMatrix() * getTransform().getMatrix());
	}
	else {
		armTransform = getTransform();
		bodyTransform = getTransform();
	}
	//--------------------------

	model->setTransform(&bodyTransform);
	m_leftArm->setTransform(&armTransform);
	m_head->setTransform(&bodyTransform);

	float colorGrad = m_playerHealth.healthPercent * 2 + 0.5f;
	DirectX::SimpleMath::Vector4 color = lightColor * colorGrad;
	/*color.w = 3.f;
	color.w *= colorGrad;*/
	model->getMaterial()->setColor(color);
	m_leftArm->getMaterial()->setColor(color);
	m_head->getMaterial()->setColor(color);

	model->draw();
	m_leftArm->draw();
	m_head->draw();
	if (m_weapon->getHeld()) {
		m_weapon->setLightColor(color);
		m_weapon->draw();
	}
	if (m_hook) { // && !m_movement.inCover) 
		m_hook->setLightColor(lightColor);
		m_hook->draw();
	}

	// Update thruster particle settings



	Vector4 tempPos(0.0f, -0.75f, 0.0f, 1.0f);
	tempPos = XMVector4Transform(tempPos, bodyTransform.getMatrix());

	m_thrusterEmitter->updateEmitPosition(Vector3(tempPos));
	if (grounded()) {
		m_thrusterEmitter->updateVelocityVariety(Vector3(10.f, 0.f, 0.5f));
		m_thrusterEmitter->updateGravityScale(0.5f);
	}
	else {
		m_thrusterEmitter->updateVelocityVariety(Vector3(5.f, -2.f, 0.5f));
		m_thrusterEmitter->updateGravityScale(1.f);
	}
}

void Character::setController(const bool usingController) {
	m_inputDevice.controller = usingController;
}

void Character::setControllerPort(const unsigned int port) {
	if (port < 4)
		m_inputDevice.controllerPort = port;
}

unsigned int Character::getPort() {
	return m_inputDevice.controllerPort;
}

unsigned int Character::getTeam() {
	return m_currentTeam;
}

float Character::getHealth() {
	return m_playerHealth.current;
}

float Character::getMaxHealth() {
	return m_playerHealth.max;
}

bool Character::isAlive() {
	return m_playerHealth.alive;
}

void Character::damage(float dmg) {
	m_playerHealth.addHealth(-dmg);
}

const DirectX::SimpleMath::Vector3& Character::getAimDirection() const {
	return m_input.aim;
}

void Character::VibrateController(unsigned int index, float strength, float timeDecreaseMul) {
	m_vibration[index].currentStrength = strength;
	if (m_vibration[index].currentStrength > strength) m_vibration[index].currentStrength = strength;
	if (m_vibration[index].currentStrength > 1.f) m_vibration[index].currentStrength = 1.f;

	m_vibration[index].decreaseMul = timeDecreaseMul;
}

void Character::setTeam(unsigned int team) {
	m_currentTeam = team;
}

void Character::setParticleHandler(ParticleHandler* particleHandler) {
	m_particleHandler = particleHandler;
}

void Character::setWeapon(Weapon * weapon) {
	m_weapon = weapon;
	m_weapon->setHeld(true);
	m_weapon->getTransform().setScale(1.0f);
}

bool Character::isUsingController() {
	return m_inputDevice.controller;
}

void Character::setHook(Hook* hook) {
	this->m_hook = hook;
}

void Character::addUpgrade(const Upgrade & upgrade){
	m_weapon->addUpgrade(upgrade);
}

void Character::setRespawnTime(float time) {
	m_respawnTime = time;
}

float Character::getRespawnTime() const {
	return m_respawnTime;
}

void Character::living() {
	m_playerHealth.current = m_playerHealth.max;
	m_playerHealth.alive = true;
	m_weapon->setHeld(true);
	Object::setLightColor(m_lightColor);
}
void Character::setRespawnPoint(const DirectX::SimpleMath::Vector3& respawnPoint) {
	m_nextRespawnPoint = respawnPoint;
}
void Character::dead() {
	m_playerHealth.current = 0;
	m_playerHealth.updatePercent();
	m_playerHealth.alive = false;
	m_weapon->triggerRelease();
	stopHook();
	setVelocity(Vector3::Zero);
	m_weapon->setHeld(false);
	VibrateController(0, 1.f, 0.3f);
	VibrateController(1, 1.f, 0.3f);

	// Ghost setup
	Vector4 color(0.8f, 0.8f, 0.8f, 1.f);
	Object::setLightColor(color);
	m_deathPoint = getTransform().getTranslation();
	m_deathInterp = 0.f;
}

void Character::setLightColor(const Vector4& color) {
	m_lightColor = color;
	Object::setLightColor(color);
	Vector4 colorCpy = color * 0.8f;
	colorCpy.Clamp(Vector4::Zero, Vector4::One * 2.f, colorCpy);
	//colorCpy.Clamp(color, Vector4::One);
	m_thrusterEmitter->updateColor(colorCpy);
}

void Character::jump() {
	//this->jumping = true;
	if (grounded()) {
		setVelocity(getVelocity() + Vector3(0.f, 10.f, 0.f));
		VibrateController(1, 0.5f, 1);
	}

	//this->getTransform().translate(Vector3(0,10,0));
}

void Character::stopJump() {
	//this->jumping = false;
	//this->getTransform().translate(Vector3(0, -10, 0));
}

void Character::fire()
{
	//m_weapon->fire(m_input.aim);
}

void Character::hook() {
	m_hook->triggerPull(m_weapon->getNozzlePos(), m_input.aim);
}

void Character::stopHook() {
	m_hook->triggerRelease();
	setAcceleration(DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));
	m_movement.hooked = false;
}

bool Character::updateVibration(float dt) {
	
	bool update = false;
	m_vibDeltaAcc += dt;
	if (m_vibDeltaAcc >= m_vibFreq) {

		for (int i = 0; i < 2; i++) {
			
			//if (m_vibration[i].timeLeft > 0) {
				m_vibration[i].currentStrength -= m_vibFreq * m_vibration[i].decreaseMul;
				//m_vibration[i].timeLeft -= dt;
			//}
			if (m_vibration[i].currentStrength < 0.f)
				m_vibration[i].currentStrength = 0.f;
				
		}
		update = true;
	}

	return update;
}

int Character::getLastAttacker() const {
	return m_lastAttackerIndex;
}

int Character::getIndex() const {
	return m_playerIndex;
}