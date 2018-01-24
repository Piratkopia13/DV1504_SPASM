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

void Character::input() {
	if (!usingController) {
		Application * apc = Application::getInstance();
		//auto& keyboard = this->app->getInput().keyboard;
		auto state = apc->getInput().keyboardState;

		this->inputVec = DirectX::SimpleMath::Vector3(state.D - state.A, state.W - state.S, 0);
		

	}
	else {

		auto& pad = app->getInput().gamepad;
		auto state = pad->GetState(controllerPort);
		if (state.IsConnected()) {

			// on button click
			if (state.buttons.a == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(0, 1);
			}
			if (state.buttons.x == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(1, 1);
			}
			if (state.buttons.b == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(2, 1);
			}
			if (state.buttons.y == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(3, 1);
			}


			//update inputVector
			this->inputVec = Vector3(
				state.thumbSticks.leftX, 
				state.thumbSticks.leftY, 
				0);
			//update aim Direction
			this->aimVec = Vector3(
				state.thumbSticks.rightX,
				state.thumbSticks.rightY,
				0);
			
			if (state.IsStartPressed()) {

				//change to pause
				PostQuitMessage(this->controllerPort);
			}
			if (state.IsBackPressed()) {

				// show scoreboard ? 
			}
		}
	}
}

void Character::update(float dt) {
	auto& pad = this->app->getInput().gamepad;

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







bool Character::updateVibration(float dt) {
	static float freq = float(1) / float(30);
	static float deltaAcc = 0;
	int upd = 0;
	deltaAcc += dt;
	if(deltaAcc > freq)
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

