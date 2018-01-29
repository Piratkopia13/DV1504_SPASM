#include "Character.h"

#pragma once

Character::Character()
{

	this->usingController = 0;
	this->controllerPort = 0;
	this->inputVec = DirectX::SimpleMath::Vector3(0, 0, 0);
	this->speed = 100;
	this->jumping = 0;
	for (int i = 0; i < 2; i++) {
		this->padVibration[i] = 1;
		this->vibrationReduction[i] = 1;
	}
	this->getTransform().setScale(0.1);
	this->getTransform().setRotations(Vector3(0, 1.55, 0));
}

Character::Character(Model * model) : Character() {
	this->setModel(model);

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
			}
			if (padTracker.b == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(0, 1);
			}
			if (padTracker.x == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(0, 1);
			}
			if (padTracker.y == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(0, 1);
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
			if (padTracker.rightTrigger == GamePad::ButtonStateTracker::HELD) {
				this->fire();
			}
			if (padTracker.leftTrigger == GamePad::ButtonStateTracker::HELD) {
				this->hook();
			}



			//update inputVector
			this->inputVec = Vector3(
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

			if (padTracker.menu == 3) {

				//change to pause
				PostQuitMessage(this->controllerPort);
			}
			if (padTracker.back == 3) {

				// show scoreboard ? 
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
	
	

	this->setVelocity(this->inputVec * this->speed);
	this->currentWeapon->setVelocity(this->inputVec * this->speed);

	this->currentWeapon->getTransform().setRotations(Vector3(1.6, -1.6, this->sinDegFromVec(this->aimVec) - 1.6));
	this->currentWeapon->move(dt);

	Moveable::move(dt);



}

void Character::draw() {
	this->m_Model->setTransform(&this->getTransform());
	this->m_Model->draw();
	this->currentWeapon->draw();

}

void Character::setController(const bool usingController) {
	this->usingController = usingController;
}

void Character::setControllerPort(const unsigned int port) {
	if (port < 4)
		this->controllerPort = port;
	
#ifdef _DEBUG
	this->getTransform().setTranslation(DirectX::SimpleMath::Vector3(int(port * 10)+50, 50, 0));
#endif
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
	this->currentWeapon->setPosition(this->getTransform().getTranslation());
	this->currentWeapon->getTransform().setScale(0.7);
	
}

void Character::jump()
{
	this->jumping = true;
	this->getTransform().translate(Vector3(0,10,0));
}

void Character::stopJump()
{
	this->jumping = false;
	this->getTransform().translate(Vector3(0, -10, 0));
}

void Character::fire()
{
	this->getTransform().scaleUniformly(1.001);
	
}

void Character::hook()
{
	this->getTransform().scaleUniformly(0.999);
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

