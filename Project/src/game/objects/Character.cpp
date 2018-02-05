#pragma once
#include "Character.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Character::Character()
	: Moveable(),
	m_currentTeam(0)
{
	m_inputDevice = { 1, 0 };
	m_input = {Vector3(0,0,0), Vector3(1,0,0)};
	m_movement = { 0, 10 };
	m_playerHealth = { 100, 120, 1 };
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
	if (m_weapon)
		delete m_weapon;
	if (m_hook)
		delete m_hook;
}

void Character::input(
	DirectX::GamePad::State& padState,
	DirectX::GamePad::ButtonStateTracker& padTracker,
	DirectX::Keyboard::State& keyState,
	DirectX::Keyboard::KeyboardStateTracker& keyTracker) {
	if (!m_playerHealth.alive)
		return;


	if (!m_inputDevice.controller) {


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
				
			}
			if (padTracker.y == GamePad::ButtonStateTracker::PRESSED) {
				this->addVibration(0, 1);
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
	auto& pad = app->getInput().gamepad;

	if (updateVibration(dt))
		pad->SetVibration(m_inputDevice.controllerPort,
			m_vibration[0].currentStrength,
			m_vibration[1].currentStrength);

	if (!m_playerHealth.alive)
		return;

	if(m_playerHealth.current < m_playerHealth.max)
		m_playerHealth.current += m_playerHealth.current;


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

	Moveable::move(dt);

	if (m_weapon) {
		m_weapon->getTransform().setTranslation(this->getTransform().getTranslation() + Vector3(0.f,0.5f,-0.8f));
		m_weapon->getTransform().setRotations(Vector3(1.6f, -1.6f, this->sinDegFromVec(m_input.aim) - 1.6f));
		m_weapon->update(dt, m_input.aim);
	}
	if (m_hook) {
		m_hook->update(dt, m_weapon->getTransform().getTranslation());
	}
}

void Character::draw() {
	this->m_Model->setTransform(&getTransform());
	this->m_Model->getMaterial()->setColor(lightColor);
	this->m_Model->draw();
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



unsigned int Character::getPort()
{
	return m_inputDevice.controllerPort;
}

unsigned int Character::getTeam()
{
	return m_currentTeam;
}

float Character::getHealth()
{
	return m_playerHealth.current;
}

float Character::getMaxHealth()
{
	return m_playerHealth.max;
}

bool Character::isAlive()
{
	return m_playerHealth.alive;
}



void Character::setVibration(unsigned int index, float strength, float time)
{
	m_vibration[index] = { strength, time };
}

void Character::addVibration(unsigned int index, float strength, float time)
{
	m_vibration[index].currentStrength += strength;
	m_vibration[index].timeLeft += time;
}

void Character::setTeam(unsigned int team)
{
	m_currentTeam = team;
}

void Character::setWeapon(Weapon * weapon)
{
	m_weapon = weapon;
	m_weapon->setHeld(true);
	m_weapon->getTransform().setScale(1.0f);
}

bool Character::isUsingController()
{
	return m_inputDevice.controller;
}

void Character::setHook(Hook* hook) {
	this->m_hook = hook;
}

void Character::living() {
	m_playerHealth.current = m_playerHealth.max;
	m_playerHealth.alive = true;
	m_weapon->setHeld(true);
}
void Character::dead() {
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
	m_weapon->fire(m_input.aim);
}

void Character::hook()
{
	m_hook->triggerPull(m_weapon->getTransform().getTranslation(), m_input.aim);
	m_movement.hooked = true;
}

void Character::stopHook()
{
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

