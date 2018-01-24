#include "Character.h"
#include "../../sail/Application.h"
#pragma once

Character::Character()
{
	
	this->usingController = 0;
	this->controllerPort = 0;
	this->inputVec = DirectX::SimpleMath::Vector3(0, 0, 0);
	this->speed = 100;
}

Character::Character(Model * model) : Character() {
	this->model = model;
}
Character::Character(Model * model, unsigned int usingController = 0, unsigned int port = 0) 
	: Character() {
	if (usingController) {



	}
}

Character::~Character() {
	//delete this->model;

}

void Character::input() {



}

void Character::update(float dt) {


	Application* app = Application::getInstance();
	if (!usingController) {

		auto& keyboard = app->getInput().keyboard;
		auto state = app->getInput().keyboardState;

		this->inputVec = DirectX::SimpleMath::Vector3(state.D - state.A, 0, 0);
		auto& pad = app->getInput().gamepad;
		auto statex = pad->GetState(controllerPort);
		for(int i = 0;i<4;i++)
			pad->SetVibration(i, state.Q, state.E);

		

	}
	else {
		
		auto& pad = app->getInput().gamepad;
		auto state = pad->GetState(controllerPort);
		if(state.IsConnected()) {
			
			pad->SetVibration(this->controllerPort, state.IsXPressed() + state.triggers.left, state.IsAPressed() + state.triggers.right);
			

			this->inputVec = DirectX::SimpleMath::Vector3(state.thumbSticks.leftX, state.thumbSticks.leftY, 0);
			if (state.IsViewPressed()) {
				PostQuitMessage(this->controllerPort);
			}

		}

		//pad->SetVibration(this->controllerPort, inputVec.x, inputVec.y);


	}
	
	this->move(this->inputVec * dt * this->speed);


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
		this->padVibration[index] += addition;
}

void Character::updateVibration(float dt) {
	
	static float deltaAcc = 0;
	deltaAcc += dt;

	for (int i = 0; i < 4; i++) {
		this->padVibration[i];

		
	}
}

