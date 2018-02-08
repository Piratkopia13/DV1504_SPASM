#include "Weapon.h"

Weapon::Weapon() {
	m_held = false;
	int m_team = -1;
	m_projectileHandler = nullptr;

	m_triggerHeld = false;
	m_timeSinceFire = 0;
}

Weapon::Weapon(Model *drawModel, ProjectileHandler* projHandler, int team) 
	: Weapon()
{
	model = drawModel;
	m_projectileHandler = projHandler;
	m_team = team;
	m_held = true;
	m_upgrade = new Upgrade();
}

Weapon::~Weapon() {
	Memory::safeDelete(m_upgrade);
}

const bool Weapon::getHeld() const {
	return m_held;
}

void Weapon::setHeld(bool held) {
	m_held = held;
}

void Weapon::addUpgrade(Upgrade * upgrade) {
	m_upgrade->combine(*upgrade);
}

void Weapon::triggerPull()
{
	m_triggerHeld = true;
}

void Weapon::triggerRelease()
{
	m_triggerHeld = false;
	m_timeSinceFire = 0;
}

void Weapon::fire(const DirectX::SimpleMath::Vector3& direction) {
	if (m_projectileHandler) {
		float extraSpeed = 1;
		float extraDamage = 1;
		if (m_upgrade->speedActive()) {
			extraSpeed = m_upgrade->speedRate();
		}

		//Create projectile with inputs; startPos, direction, speed/force etc.
		Projectile* temp = new Projectile(getTransform().getTranslation(), direction * 25.0f * extraSpeed, 10.0f * extraDamage, m_team);
		temp->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f,atan2(direction.y, direction.x)));
		m_projectileHandler->addProjectile(temp);
	}
}

ProjectileHandler& Weapon::getProjectileHandler() {
	return *m_projectileHandler;
}

void Weapon::update(float dt, const DirectX::SimpleMath::Vector3& direction) {

	m_upgrade->update(dt);
	static float baseAuto = 1.0f;

	if (m_triggerHeld) {

		if (m_timeSinceFire == 0.0 && !m_upgrade->autoActive()) {
			fire(direction);
		}
		else if (m_upgrade->autoActive() && m_timeSinceFire >= baseAuto * m_upgrade->autoRate()) {
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