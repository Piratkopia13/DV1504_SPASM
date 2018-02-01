#include "Character.h"

#pragma once

Character::Character()
	: Moveable()
{

	this->usingController = 0;
	this->controllerPort = 0;
	m_inputVec = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	this->aimVec = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	this->speed = 10;
	this->jumpTimer = 0;
	for (int i = 0; i < 2; i++) {
		//this->padVibration[i] = 1;
		this->vibrationReduction[i] = 1;
	}
	this->getTransform().setRotations(Vector3(0.0f, 1.55f, 0.0f));
	this->setLightColor(Vector4(1, 1, 1, 1));
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
	//delete this->model;

}

void Character::input(
	DirectX::GamePad::State& padState,
	GamePad::ButtonStateTracker& padTracker,
	DirectX::Keyboard::State& keyState,
	Keyboard::KeyboardStateTracker& keyTracker) {

	if (!usingController) {


	}
	else {
		if (padState.IsConnected()) {

			// ON BUTTON CLICK
			if (padTracker.a == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(1, 1);
				this->jump();
				//this->currentWeapon->cooldownTime += 0.02f;
			}
			if (padTracker.b == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(0, 1);
				/*if(this->currentWeapon->cooldownTime > 0.02f)
					this->currentWeapon->cooldownTime -= 0.02f;*/

			}
			if (padTracker.x == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(0, 1);
				this->currentWeapon->automatic = true;
			}
			if (padTracker.y == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(0, 1);
				this->currentWeapon->automatic = false;
			}

			// ON BUTTON RELEASE
			if (padTracker.a == GamePad::ButtonStateTracker::RELEASED) {
				this->addVibration(1, 1);
				this->stopJump();
			}
			if (padTracker.b == GamePad::ButtonStateTracker::RELEASED) {
				this->addVibration(0, 1);
			}
			if (padTracker.x == GamePad::ButtonStateTracker::RELEASED) {
				this->addVibration(0, 1);
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
				this->currentWeapon->triggerPull();
				
			}
			if (padTracker.rightTrigger == GamePad::ButtonStateTracker::RELEASED) {
				this->currentWeapon->triggerRelease();

			}
			if (padTracker.leftTrigger == GamePad::ButtonStateTracker::PRESSED) {
				hook();
			}
			if (padTracker.leftTrigger == GamePad::ButtonStateTracker::RELEASED) {
				m_hook->triggerRelease();
				setAcceleration(DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));
				m_hooked = false;
			}



			//update inputVector
			m_inputVec = Vector3(
				padState.thumbSticks.leftX,
				padState.thumbSticks.leftY, 
				0);
			//update aim Direction
			Vector3 tempVec = Vector3(
				padState.thumbSticks.rightX,
				padState.thumbSticks.rightY,
				0);
			if (tempVec.LengthSquared() > 0.3) {
				this->aimVec = tempVec;
				this->aimVec.Normalize();
			}
		}
	}
}

void Character::update(float dt) {
	Application* app = Application::getInstance();
	auto& pad = app->getInput().gamepad;

	if (updateVibration(dt))
		pad->SetVibration(this->controllerPort,
			this->padVibration[0],
			this->padVibration[1],
			this->padVibration[2],
			this->padVibration[3]);
	//if (this->jumping) {

	//	float jumpStr = 10;
	//	jumpStr -= this->jumpTimer * 10;
	//	this->addAcceleration(Vector3(0, jumpStr, 0));
	//	this->jumpTimer += dt;
	//}

	if (grounded())
		this->setVelocity(DirectX::SimpleMath::Vector3(m_inputVec.x * this->speed, this->getVelocity().y, 0.f));
	else {
		float velX = m_inputVec.x * this->speed * 0.1f + getVelocity().x;
		velX = max(min(velX, this->speed * 0.8f), -this->speed * 0.8f);
		this->setVelocity(DirectX::SimpleMath::Vector3(velX, this->getVelocity().y, 0.f));
	}

	if (m_hooked) {
		this->setGrounded(false);
		this->setAcceleration(m_hook->getDirection() * 40.0f);
	}

	Moveable::move(dt);

	this->currentWeapon->getTransform().setTranslation(this->getTransform().getTranslation() + Vector3(0.f,0.5f,-0.8f));
	this->currentWeapon->getTransform().setRotations(Vector3(1.6f, -1.6f, this->sinDegFromVec(this->aimVec) - 1.6f));

	this->currentWeapon->update(dt, this->aimVec);
	m_hook->update(dt, currentWeapon->getTransform().getTranslation());
}

void Character::draw() {
	this->m_Model->setTransform(&this->getTransform());
	this->m_Model->getMaterial()->setColor(this->lightColor);
	this->m_Model->draw();
	this->currentWeapon->draw();
	m_hook->draw();
}

void Character::setController(const bool usingController) {
	this->usingController = usingController;
}

void Character::setControllerPort(const unsigned int port) {
	if (port < 4)
		this->controllerPort = port;
	
//#ifdef _DEBUG
	this->getTransform().setTranslation(DirectX::SimpleMath::Vector3(port * 2.0f + 3.0f, 3.0f, 0.0f));
//#endif
}

void Character::addVibration(unsigned int index, float addition) {
	if (index < 4)
	{
		this->padVibration[index] += addition;
		if (this->padVibration[index] > 1)
		{
			this->padVibration[index] = 1;
		}
	}
}

unsigned int Character::getPort()
{
	return this->controllerPort;
}

void Character::setTeam(unsigned int team)
{
	this->currentTeam = team;
}

void Character::setWeapon(Weapon * weapon)
{
	this->currentWeapon = weapon;
	this->currentWeapon->setHeld(true);
	//this->currentWeapon->setPosition(this->getTransform().getTranslation()+Vector3(0,0.5,0.5f));
	this->currentWeapon->getTransform().setScale(1.0f);
	
}

void Character::setHook(Hook* hook) {
	this->m_hook = hook;
}

void Character::jump()
{
	//this->jumping = true;
	if(grounded())
		this->setVelocity(getVelocity() + DirectX::SimpleMath::Vector3(0.f, 10.f, 0.f));
	//this->getTransform().translate(Vector3(0,10,0));
}

void Character::stopJump()
{
	//this->jumping = false;
	//this->getTransform().translate(Vector3(0, -10, 0));
}

void Character::fire()
{
	currentWeapon->fire(aimVec);
}

void Character::hook()
{
	m_hook->triggerPull(currentWeapon->getTransform().getTranslation(), this->aimVec);
	m_hooked = true;
}

bool Character::updateVibration(float dt) {
	static float freq = 1.f / 30.f;
	static float deltaAcc = 0;
	int upd = 0;
	deltaAcc += dt;
	if(deltaAcc >= freq)
		for (int i = 0; i < 2; i++) {
			if (this->padVibration[i] > 0) {
				upd++;
				this->padVibration[i] -= this->vibrationReduction[i] * freq;
				if (this->padVibration[i] < 0)
					this->padVibration[i] = 0;
			}
				
		}
	if (upd > 0)
		return true;
	else
		return false;
}

