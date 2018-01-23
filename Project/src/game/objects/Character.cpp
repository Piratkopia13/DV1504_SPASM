#include "Character.h"
#include "../../sail/Application.h"
#pragma once

Character::Character()
{
	this->usingController = 0;
	this->controllerPort = 0;
	this->inputVec = DirectX::SimpleMath::Vector3(0, 0, 0);
	this->speed = 1;

}

Character::Character(Model * model) : Character()
{
	this->model = model;
}

Character::~Character()
{
	delete this->model;

}

void Character::input() {

	Application* app = Application::getInstance();
	if (!usingController) {

		auto& keyboard = app->getInput().keyboard;
		auto state = app->getInput().keyboardState;

		this->inputVec = DirectX::SimpleMath::Vector3(state.D - state.A, 0, 0);

	}
	else {
		auto& pad = app->getInput().gamepad;
		auto state = pad->GetState(controllerPort);
		//pad->SetVibration(this->controllerPort, state.triggers.left, state.triggers.right);

		this->inputVec = DirectX::SimpleMath::Vector3(state.thumbSticks.leftX, state.thumbSticks.leftY, 0);
		pad->SetVibration(this->controllerPort, inputVec.x, inputVec.y);


	}


}

void Character::update(float dt)
{
	
	this->move(this->inputVec * dt * this->speed);


}

void Character::draw()
{
	this->model->setTransform(&this->getTransformation());
	this->model->draw();
}

void Character::setController(const bool usingController)
{
	this->usingController = usingController;
}

void Character::setControllerPort(const unsigned int port)
{
	if (port < 4)
		this->controllerPort = port;
}
