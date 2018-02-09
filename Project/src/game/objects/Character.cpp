#pragma once
#include "Character.h"
#include "../collision/CollisionHandler.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Character::Character()
	: Moveable(),
	m_currentTeam(0)
{
	m_inputDevice = { 1, 0 };
	m_input = {Vector3(0,0,0), Vector3(1,0,0)};
	m_movement = { 0, 10 };
	m_playerHealth.setMax(100);
	m_playerHealth.setHealth(100);
	m_playerHealth.regen = 5;
	m_vibration[0] = { 0, 0};

	getTransform().setRotations(Vector3(0.0f, 1.55f, 0.0f));
	setLightColor(Vector4(1, 1, 1, 1));
}

Character::Character(Model * model) : Character() {
	this->setModel(model);
	this->updateBoundingBox();
}
Character::Character(Model * model, unsigned int usingController, unsigned int port)
	: Character(model) {
	this->setController(usingController);
	this->setControllerPort(port);
}

Character::~Character() {
	Memory::safeDelete(m_weapon);
	Memory::safeDelete(m_hook);
}

void Character::input(
	DirectX::GamePad::State& padState,
	DirectX::GamePad::ButtonStateTracker& padTracker,
	DirectX::Keyboard::State& keyState,
	DirectX::Keyboard::KeyboardStateTracker& keyTracker) {
	if (!m_playerHealth.alive)
		return;
	

	if (!m_inputDevice.controller) {

		m_input.movement = Vector3(
			(float)keyState.IsKeyDown(Keyboard::D) - (float)keyState.IsKeyDown(Keyboard::A),
			(float)keyState.IsKeyDown(Keyboard::W) - (float)keyState.IsKeyDown(Keyboard::S),
			0);
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
		if (padState.IsConnected()) {

			// ON BUTTON CLICK
			if (padTracker.a == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(1, 1);
				this->jump();
			}
			if (padTracker.b == GamePad::ButtonStateTracker::PRESSED) {
				
			}
			if (padTracker.x == GamePad::ButtonStateTracker::PRESSED) {
				m_playerHealth.addHealth(-10);
				this->addVibration(0, 1);
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
				this->addVibration(0, 1);
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

			if (tempVec.LengthSquared() > 0.3) {			
				tempVec.Normalize();
				m_input.aim = tempVec;
			}
		}
	}
}

void Character::update(float dt) {
	Application* app = Application::getInstance();
	CollisionHandler* collHandler = CollisionHandler::getInstance();
	auto& pad = app->getInput().gamepad;

	if (updateVibration(dt))
		pad->SetVibration(m_inputDevice.controllerPort,
			m_vibration[0].currentStrength,
			m_vibration[1].currentStrength);

	if (!m_playerHealth.alive)
		return;

	if (m_playerHealth.current < m_playerHealth.max) {
		m_playerHealth.current += m_playerHealth.regen * dt;
	}

	m_playerHealth.updatePercent();
	

	if (grounded())
		this->setVelocity(DirectX::SimpleMath::Vector3(m_input.movement.x * m_movement.speed, this->getVelocity().y, 0.f));
	else {
		float velX = m_input.movement.x * m_movement.speed * 0.1f + getVelocity().x;
		velX = max(min(velX, m_movement.speed * 0.8f), -m_movement.speed * 0.8f);
		this->setVelocity(DirectX::SimpleMath::Vector3(velX, this->getVelocity().y, 0.f));
	}

	if (m_movement.hooked) {
		this->setGrounded(false);
		this->setAcceleration(m_hook->getDirection() * 40.0f);
	}

	Moveable::updateVelocity(dt);
	collHandler->resolveLevelCollisionWith(this, dt);
	Moveable::move(dt);

	if (m_weapon) {
		m_weapon->getTransform().setTranslation(this->getTransform().getTranslation() + Vector3(0.f,0.5f, -0.0f));
		m_weapon->getTransform().setRotations(Vector3(1.6f, -1.6f, this->sinDegFromVec(m_input.aim) - 1.6f));
		m_weapon->update(dt, m_input.aim);
	}
	if (m_hook) {
		m_hook->update(dt, m_weapon->getTransform().getTranslation());
	}

	collHandler->resolveProjectileCollisionWith(this);
	collHandler->resolveUpgradeCollisionWith(this);
}

void Character::draw() {
	model->setTransform(&getTransform());
	model->getMaterial()->setColor(lightColor*m_playerHealth.healthPercent);
	model->draw();
	if(m_weapon)
		m_weapon->draw();
	if(m_hook)
		m_hook->draw();
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

void Character::setVibration(unsigned int index, float strength, float time) {
	m_vibration[index] = { strength, time };
}

void Character::addVibration(unsigned int index, float strength, float time) {
	m_vibration[index].currentStrength += strength;
	m_vibration[index].timeLeft += time;
}

void Character::setTeam(unsigned int team) {
	m_currentTeam = team;
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

void Character::living() {
	m_playerHealth.current = m_playerHealth.max;
	m_playerHealth.alive = true;
	m_weapon->setHeld(true);
	
}
void Character::dead() {
	m_playerHealth.current = 0;
	m_playerHealth.updatePercent();
	m_playerHealth.alive = false;
	m_weapon->setHeld(false);
}

void Character::jump() {
	//this->jumping = true;
	if(grounded())
		this->setVelocity(getVelocity() + DirectX::SimpleMath::Vector3(0.f, 10.f, 0.f));
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
	m_hook->triggerPull(m_weapon->getTransform().getTranslation(), m_input.aim);
	m_movement.hooked = true;
}

void Character::stopHook() {
	m_hook->triggerRelease();
	setAcceleration(DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));
	m_movement.hooked = false;
}

bool Character::updateVibration(float dt) {
	static float freq = 1.f / 30.f;
	static float deltaAcc = 0;
	int upd = 0;
	deltaAcc += dt;
	if(deltaAcc >= freq)
		for (int i = 0; i < 2; i++) {
			
				
		}
	if (upd > 0)
		return true;
	else
		return false;
}

