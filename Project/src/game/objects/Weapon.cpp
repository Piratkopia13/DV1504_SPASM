#include "Weapon.h"

Weapon::Weapon() {
	m_drawModel = nullptr;
	m_held = false;
	int m_team = -1;
}

Weapon::Weapon(Model *drawModel, int team) {
	m_drawModel = drawModel;
	m_team = team;
	m_held = true;
}

Weapon::~Weapon() {

}

const bool Weapon::getHeld() const {
	return m_held;
}

void Weapon::setHeld(bool held) {
	m_held = held;
}

void Weapon::setModel(Model *newModel) {
	m_drawModel = newModel;
}

void Weapon::fire(DirectX::SimpleMath::Vector3 direction) {
	//Create projectile with inputs; startPos, direction, speed/force etc.
	Projectile temp(getTransform().getTranslation(), direction * 5.0f, 10.0f, 1, 200);
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