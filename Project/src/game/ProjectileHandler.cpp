#include "ProjectileHandler.h"
#include "collision/CollisionHandler.h"
#include "ParticleHandler.h"

using namespace DirectX::SimpleMath;

ProjectileHandler::ProjectileHandler(ParticleHandler* particleHandler)
: m_particleHandler(particleHandler) {
	m_projectileModel = Application::getInstance()->getResourceManager().getFBXModel("projectile").getModel();

	m_traceEmitter = std::shared_ptr<ParticleEmitter>(
		new ParticleEmitter(ParticleEmitter::EXPLOSION, Vector3::Zero,
							Vector3::Zero, Vector3(3.f, 3.f, 0.5f), 0.f, 10000U, 0.2f, 0.f, Vector4(0.8f, 0.8f, 0.8f, 1.f), 0.f, 0U, true, false));
	m_particleHandler->addEmitter(m_traceEmitter);
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

		spawnTracer(proj->getTransform().getTranslation(), proj->getTransform().getTranslation() + proj->getVelocity() * dt, dt);

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

void ProjectileHandler::projectileHitSomething(Projectile* proj, const DirectX::SimpleMath::Vector3& hitPos, float dt) {
	// last beam particle effects
	spawnTracer(proj->getTransform().getTranslation(), hitPos, dt);
}

void ProjectileHandler::spawnTracer(const DirectX::SimpleMath::Vector3& start, const DirectX::SimpleMath::Vector3& end, float dt) {
	m_traceEmitter->spawnBeamParticles(start, end, 0.3f, 0.1f, 0.1f + dt);
}
