#include "Weapon.h"

Weapon::Weapon() {
	m_held = false;
	int m_team = -1;
	m_projectileHandler = nullptr;

	m_automatic = true;
	m_triggerHeld = false;
	m_timeSinceFire = 0;
	m_cooldownTime = 0.1f;
}

Weapon::Weapon(Model *drawModel, ProjectileHandler* projHandler, int team) 
	: Weapon()
{
	model = drawModel;
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

void Weapon::triggerPull() {
	m_triggerHeld = true;
}

void Weapon::triggerRelease() {
	m_triggerHeld = false;
	m_timeSinceFire = 0;
}

void Weapon::fire(const DirectX::SimpleMath::Vector3& direction) {
	if (m_projectileHandler != nullptr) {
		//Create projectile with inputs; startPos, direction, speed/force etc.
		Projectile* temp = new Projectile(getTransform().getTranslation(), direction * 25.0f, 10.0f, m_team);
		temp->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f,atan2(direction.y, direction.x)));
		temp->setLightColor(DirectX::SimpleMath::Vector4(5.f));
		m_projectileHandler->addProjectile(temp);
		//test
	}
}

ProjectileHandler& Weapon::getProjectileHandler() {
	return *m_projectileHandler;
}

void Weapon::update(float dt, const DirectX::SimpleMath::Vector3& direction) {

	if (m_triggerHeld) {
		if (m_timeSinceFire == 0.0 && !m_automatic) {
			fire(direction);
		}
		else if (m_automatic && m_timeSinceFire >= m_cooldownTime) {
			fire(direction);

			m_timeSinceFire = 0.0;
		}
		m_timeSinceFire += dt;
	}



	//move(dt);
}

void Weapon::draw() {
	model->setTransform(&getTransform());
	model->getMaterial()->setColor(lightColor);
	model->draw();
}