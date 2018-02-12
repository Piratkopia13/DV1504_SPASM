#include "Weapon.h"

using namespace DirectX::SimpleMath;

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

void Weapon::addUpgrade(const Upgrade& upgrade) {
	m_upgrade->combine(upgrade);
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

	static float baseSpeed = 5.0f;
	static float baseDamage = 10.0f;
	static Vector3 zVec(0, 0, 1);
	static float diff = 0.2;

	if (m_projectileHandler) {
		float extraSpeed = 1;
		float extraDamage = 1;
		if (m_upgrade->speedActive()) {
			extraSpeed = m_upgrade->speedRate();
		}
		if (m_upgrade->damageActive()) {
			extraDamage = m_upgrade->damageMultiplier();
		}

		//Create projectile with inputs; startPos, direction, speed/force etc.
		Vector3 tempPos = getTransform().getTranslation();
		Matrix tempMatrix;
		
		//translation away from origo of the weapon
		if (direction.x >= 0.0f) {
			tempMatrix *= Matrix::CreateTranslation(Vector3(0.4f, -0.36f, -0.3f));
		}
		else {
			tempMatrix *= Matrix::CreateTranslation(Vector3(-0.4f, -0.36f, 0.3f));
		}

		//rotation around the origo of the weapon
		if (direction.x >= 0.0f) {
			tempMatrix *= Matrix::CreateRotationZ(atan2(direction.y, direction.x));
		}
		else {
			tempMatrix *= Matrix::CreateRotationZ(atan2(direction.y, direction.x) + 3.14f);
		}

		//translation into world space
		tempMatrix *= Matrix::CreateTranslation(tempPos);

		tempPos = Vector3(XMVector4Transform(Vector4(0.0f, 0.0f, 0.0f, 1.0f), tempMatrix));

		Projectile* temp = new Projectile(
			tempPos,
			direction * baseSpeed * extraSpeed, 
			baseDamage * extraDamage, 
			m_team);
		if (m_upgrade->gravActive()) {
			temp->setGravScale(0);
		}
		temp->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f,atan2(direction.y, direction.x)));
		temp->setLightColor(DirectX::SimpleMath::Vector4(5.f));
		m_projectileHandler->addProjectile(temp);


		if (m_upgrade->multiActive()) {
			
			Vector3 changeVec = direction.Cross(zVec);
			changeVec.Normalize();

			
			for (size_t i = 0; i < (size_t)m_upgrade->multiCount()*0.5; i++) {
				Vector3 tempVec1 = direction + changeVec * diff*(i+1);
				Vector3 tempVec2 = direction - changeVec * diff*(i+1);

				tempVec1.Normalize();
				tempVec2.Normalize();

				Projectile* temp1 = new Projectile(
					tempPos,
					tempVec1 * baseSpeed * extraSpeed,
					baseDamage * extraDamage,
					m_team);
				Projectile* temp2 = new Projectile(
					tempPos,
					tempVec2 * baseSpeed * extraSpeed,
					baseDamage * extraDamage,
					m_team);
				if (m_upgrade->gravActive()) {
					temp1->setGravScale(0);
					temp2->setGravScale(0);
				}

				temp1->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(tempVec1.y, tempVec1.x)));
				temp2->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(tempVec2.y, tempVec2.x)));
				m_projectileHandler->addProjectile(temp1);
				m_projectileHandler->addProjectile(temp2);
			}




		}


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
		else if (m_upgrade->autoActive() && (m_timeSinceFire >= baseAuto * m_upgrade->autoRate() || m_timeSinceFire == 0)) {
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