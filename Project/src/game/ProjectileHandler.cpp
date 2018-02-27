#include "ProjectileHandler.h"
#include "collision/CollisionHandler.h"
#include "ParticleHandler.h"

using namespace DirectX::SimpleMath;

ProjectileHandler::ProjectileHandler(ParticleHandler* particleHandler)
: m_particleHandler(particleHandler) {
	m_projectileModel = Application::getInstance()->getResourceManager().getFBXModel("projectile").getModel();
}

ProjectileHandler::~ProjectileHandler() {
	for (unsigned int i = 0; i < m_projectiles.size(); i++) {
		delete m_projectiles.at(i);
	}
}

std::vector<Projectile*>& ProjectileHandler::getProjectiles() {
	return m_projectiles;
}

void ProjectileHandler::addProjectile(Projectile* newProjectile) {
	newProjectile->setModel(m_projectileModel);
	m_projectiles.push_back(newProjectile);
	m_projectileLifeSpan.push_back(10.0f);
}

void ProjectileHandler::removeAt(int index) {
	std::swap(m_projectiles.at(index), m_projectiles.back());
	std::swap(m_projectileLifeSpan.at(index), m_projectileLifeSpan.back());
	delete m_projectiles.back();
	m_projectileLifeSpan.pop_back();
	m_projectiles.pop_back();
}

void ProjectileHandler::update(float dt) {
	for (unsigned int i = 0; i < m_projectiles.size(); i++) {
		auto& proj = m_projectiles.at(i);

		proj->updateVelocity(dt);
		proj->move(dt);
		m_projectileLifeSpan.at(i) -= dt;

		if (m_projectileLifeSpan.at(i) < 0) {
			removeAt(i);
		}
	}
}

void ProjectileHandler::draw() {
	for (auto& proj : m_projectiles) {
		proj->draw();
	}
}

void ProjectileHandler::projectileHitLevel(const DirectX::SimpleMath::Vector3& hitPos) {

	// Hit particle effect
	m_particleHandler->addEmitter(std::shared_ptr<ParticleEmitter>(new ParticleEmitter(
		ParticleEmitter::FIREBALL, hitPos, Vector3(-0.5f), Vector3(5.f, 5.f, 2.f), 0.f, 25, 0.4f, 0.5f, Vector4::One, 1.f, 25U, true, true)));

}
