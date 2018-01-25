#include "Character.h"

#pragma once

Character::Character()
{

	this->usingController = 0;
	this->controllerPort = 0;
	this->inputVec = DirectX::SimpleMath::Vector3(0, 0, 0);
	this->speed = 100;
	for (int i = 0; i < 4; i++) {
		this->padVibration[i] = 1;
		this->vibrationReduction[i] = 1;
	}
}

Character::Character(Model * model) : Character() {
	this->model = model;
}
Character::Character(Model * model, unsigned int usingController = 0, unsigned int port = 0) 
	: Character() {
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
		
		this->inputVec = Vector3(
			
			keyState.D - keyState.A, 
			keyState.W - keyState.S, 
			0);
		

	}
	else {
		if (padState.IsConnected()) {

			// on button click
			if (padTracker.a == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(0, 1);
			}
			if (padTracker.x == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(1, 1);
			}
			if (padTracker.b == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(2, 1);
			}
			if (padTracker.y == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(3, 1);
			}
			
			if (padState.buttons.a == GamePad::ButtonStateTracker::HELD) {
				this->addVibration(1,1);
			}
			

			//update inputVector
			this->inputVec = Vector3(
				padState.thumbSticks.leftX,
				padState.thumbSticks.leftY, 
				0);
			//update aim Direction
			this->aimVec = Vector3(
				padState.thumbSticks.rightX,
				padState.thumbSticks.rightY,
				0);
			
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
	
	
	this->setVelocity(this->inputVec * dt * this->speed);

	Moveable::update(dt);
}

void Character::draw() {
	this->model->setTransform(&this->getTransform());
	this->model->draw();
}

void Character::setController(const bool usingController) {
	this->usingController = usingController;
}

void Character::setControllerPort(const unsigned int port) {
	if (port < 4)
		this->controllerPort = port;

	this->getTransform().setTranslation(DirectX::SimpleMath::Vector3(port, port, port));
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







bool Character::updateVibration(float dt) {
	static float freq = 1.f / 30.f;
	static float deltaAcc = 0;
	int upd = 0;
	deltaAcc += dt;
	if(deltaAcc >= freq)
		for (int i = 0; i < 4; i++) {
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

