#include "Weapon.h"

Weapon::Weapon() {
	m_drawModel = nullptr;
	m_held = false;
	int m_team = -1;
}

Weapon::Weapon(Model *drawModel) {
	m_drawModel = drawModel;
}

Weapon::~Weapon() {

}

const bool Weapon::getHeld() const {
	return m_held;
}

void Weapon::setHeld(bool held, int team = -1) {
	m_held = held;
	m_team = team;
}

void Weapon::setModel(Model *newModel) {
	m_drawModel = newModel;
}

void Weapon::fire() {
	//Create projectile with inputs; startPos, direction, speed/force etc.
}

void Weapon::update(float dt) {
	if (!m_held) {
		move(dt);
	}
}

void Weapon::draw() {
	m_drawModel->setTransform(&getTransform());
	m_drawModel->draw();
}