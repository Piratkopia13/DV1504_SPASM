#include "Weapon.h"

Weapon::Weapon() {
	m_held = false;
	int m_team = -1;
	m_projectileHandler = nullptr;
}

Weapon::Weapon(Model *drawModel, ProjectileHandler* projHandler, int team) {
	m_Model = drawModel;
	m_projectileHandler = projHandler;
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

void Weapon::fire(DirectX::SimpleMath::Vector3 direction) {
	if (m_projectileHandler != nullptr) {
		//Create projectile with inputs; startPos, direction, speed/force etc.
		Projectile* temp = new Projectile(getTransform().getTranslation(), direction * 200.0f, 10.0f, 1);
		temp->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f,atan2(direction.y, direction.x)));
		m_projectileHandler->addProjectile(temp);
	}
}

void Weapon::update(float dt) {
	if (!m_held) {
		move(dt);
	}
}

void Weapon::draw() {
	m_Model->setTransform(&getTransform());
	m_Model->draw();
}