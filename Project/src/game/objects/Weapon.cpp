#include "Weapon.h"

Weapon::Weapon() {
	m_held = false;
	int m_team = -1;
	m_projectileHandler = nullptr;

	this->automatic = true;
	this->triggerHeld = false;
	this->timeSinceFire = 0;
	this->cooldownTime = 0.1;
}

Weapon::Weapon(Model *drawModel, ProjectileHandler* projHandler, int team) : Weapon() {
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

void Weapon::triggerPull()
{
	this->triggerHeld = true;
}

void Weapon::triggerRelease()
{
	this->triggerHeld = false;
	this->timeSinceFire = 0;
}

void Weapon::fire(DirectX::SimpleMath::Vector3 direction) {
	if (m_projectileHandler != nullptr) {
		//Create projectile with inputs; startPos, direction, speed/force etc.
		Projectile* temp = new Projectile(getTransform().getTranslation(), direction * 200.0f, 10.0f, 1);
		temp->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f,atan2(direction.y, direction.x)));
		m_projectileHandler->addProjectile(temp);
	}
}

void Weapon::update(float dt, DirectX::SimpleMath::Vector3 direction) {

	if (this->triggerHeld) {
		if (timeSinceFire == 0.0 && !this->automatic) {
			this->fire(direction);
		}
		else if (this->automatic && this->timeSinceFire >= this->cooldownTime) {
			this->fire(direction);

			this->timeSinceFire = 0.0;
		}
		this->timeSinceFire += dt;
	}



	this->move(dt);
}

void Weapon::draw() {
	m_Model->setTransform(&getTransform());
	m_Model->draw();
}