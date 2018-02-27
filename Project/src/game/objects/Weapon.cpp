#include "Weapon.h"
#include "../collision/CollisionHandler.h"
#include "../ParticleHandler.h"
#include "../../sail/resources/audio/SoundManager.h"
#include "Character.h"

using namespace DirectX::SimpleMath;

Weapon::Weapon() {
	m_held = false;
	int m_team = -1;
	m_projectileHandler = nullptr;
	m_owner = nullptr;

	m_triggerHeld = false;
	m_timeSinceFire = 0;
}

Weapon::Weapon(Model *armModel, Model* laserModel, Model* dotModel, ProjectileHandler* projHandler, ParticleHandler* particleHandler, Character* owner)
	: Weapon()
{
	m_particleHandler = particleHandler;
	model = armModel;
	m_owner = owner;

	m_laser.laserModel = laserModel;
	m_laser.dotModel = dotModel;
	m_laser.dotTransform.setScale(0.1f);

	m_projectileHandler = projHandler;
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
	//m_timeSinceFire = 0;
}

DirectX::SimpleMath::Vector3 Weapon::getNozzlePos() const {
	return m_nozzlePos;
}

void Weapon::fire(const DirectX::SimpleMath::Vector3& direction) {

	static float baseSpeed = 20.0f;
	static float baseDamage = 10.0f;
	static float baseKnockback = 5.0f;
	static Vector3 zVec(0, 0, 1);
	static float diff = 0.2f;

	static Vector4 projColor(2.f);
	static float projScale = 4.f;

	if (m_projectileHandler) {
		float extraSpeed = 1;
		float extraDamage = 1;
		float extraKnockback = 1;
		if (m_upgrade->knockbackActive()) {
			extraKnockback = m_upgrade->knockbackRate();
		}
		if (m_upgrade->damageActive()) {
			extraDamage = m_upgrade->damageMultiplier();
		}

		float pitch = Utils::rnd() * 0.3f + 0.8f;
		Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Laser, 0.4f, pitch);

		// Muzzle flash particles
		m_particleHandler->addEmitter(std::shared_ptr<ParticleEmitter>(new ParticleEmitter(
			ParticleEmitter::EXPLOSION, m_nozzlePos, direction - Vector3(0.5f), Vector3(15.f, 15.f, 4.f),
			0.f, 25, 0.3f, 0.1f, Vector4(0.8f, 0.5f, 0.2f, 1.f), 0.2f, 25U, true, true)));

		m_owner->VibrateController(1, 0.7f * extraDamage * m_upgrade->multiCount(), min(1.75f / m_upgrade->autoRate(), 1.5f));

		//Create projectile with inputs; startPos, direction, speed/force etc.
		Projectile* temp = new Projectile(
			m_nozzlePos,
			direction * baseSpeed * extraSpeed, 
			baseDamage * extraDamage, 
			baseKnockback * extraKnockback,
			m_owner->getTeam());
		if (m_upgrade->gravActive()) {
			temp->setGravScale(0);
		}
		temp->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(direction.y, direction.x)));
		temp->getTransform().scaleUniformly(projScale);
		temp->setLightColor(projColor);
		m_projectileHandler->addProjectile(temp);


		if (m_upgrade->multiActive()) {
			
			Vector3 changeVec = direction.Cross(zVec);
			changeVec.Normalize();

			
			for (unsigned int i = 0; i < m_upgrade->multiCount(); i++) {
				Vector3 tempVec1 = direction + changeVec * diff * (i + 1.f);
				Vector3 tempVec2 = direction - changeVec * diff * (i + 1.f);

				tempVec1.Normalize();
				tempVec2.Normalize();
				pitch = Utils::rnd() * 0.2f + 0.9f;
				Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Laser, 0.2f, pitch);
				Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Laser, 0.2f, pitch);

				Projectile* temp1 = new Projectile(
					m_nozzlePos,
					tempVec1 * baseSpeed * extraSpeed,
					baseDamage * extraDamage,
					baseKnockback * extraKnockback,
					m_owner->getTeam());
				Projectile* temp2 = new Projectile(
					m_nozzlePos,
					tempVec2 * baseSpeed * extraSpeed,
					baseDamage * extraDamage,
					baseKnockback * extraKnockback,
					m_owner->getTeam());
				if (m_upgrade->gravActive()) {
					temp1->setGravScale(0);
					temp2->setGravScale(0);
				}

				temp1->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(tempVec1.y, tempVec1.x)));
				temp2->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(tempVec2.y, tempVec2.x)));
				temp1->setLightColor(projColor);
				temp2->setLightColor(projColor);
				temp1->getTransform().scaleUniformly(projScale);
				temp2->getTransform().scaleUniformly(projScale);
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

	static float baseAuto = 0.3f;

	if (m_triggerHeld) {

		if (m_timeSinceFire >= baseAuto * m_upgrade->autoRate() || m_timeSinceFire == 0) {
			fire(direction);

			m_timeSinceFire = 0.0;
		}
	}
	m_timeSinceFire += dt;

	//Updating position where projectiles spawn
	//Create projectile with inputs; startPos, direction, speed/force etc.
	Vector3 tempPos = getTransform().getTranslation();
	Matrix tempMatrix;

	//translation away from origo of the weapon (Z should be -0.3 and 0.3 but that does not hit the boundingbox atm
	if (direction.x >= 0.0f) {
		tempMatrix *= Matrix::CreateTranslation(Vector3(0.4f, -0.36f, -0.15f));
	}
	else {
		tempMatrix *= Matrix::CreateTranslation(Vector3(-0.4f, -0.36f, 0.15f));
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

	m_nozzlePos = Vector3(XMVector4Transform(Vector4(0.0f, 0.0f, 0.0f, 1.0f), tempMatrix));
	float length = (CollisionHandler::getInstance()->rayTraceLevel(m_nozzlePos, direction) - m_nozzlePos).Length();
	m_laser.laserTransform.setTranslation(m_nozzlePos + direction * (min(length, 5.f) / 2.f));
	m_laser.laserTransform.setNonUniScale(min(50.f, length * 10), 1.f, 1.f);
	m_laser.laserTransform.setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(direction.y, direction.x)));

	m_laser.dotTransform.setTranslation(CollisionHandler::getInstance()->rayTraceLevel(m_nozzlePos, direction) + DirectX::SimpleMath::Vector3(0.f, 0.f, m_laser.laserTransform.getTranslation().z) + (direction * 0.05f)); //Last addition for looks with the current model

	//move(dt);
}

void Weapon::draw() {
	model->setTransform(&getTransform());
	if (this->getTransform().getTranslation().z < 0.5f) {
		m_laser.laserModel->setTransform(&m_laser.laserTransform);
		m_laser.laserModel->getMaterial()->setColor(DirectX::SimpleMath::Vector4(1.0f, 0.f, 0.f, 1.f));

		m_laser.dotModel->getMaterial()->setColor(DirectX::SimpleMath::Vector4(4.0f, 0.f, 0.f, 1.f));

		m_laser.dotModel->setTransform(&m_laser.dotTransform);

		m_laser.laserModel->draw();
		m_laser.dotModel->draw();
	}
	model->getMaterial()->setColor(lightColor);
	model->draw();
}