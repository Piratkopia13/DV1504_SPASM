#include "ProjectileHandler.h"

ProjectileHandler::ProjectileHandler() {
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
	m_projectileLifeSpan.push_back(2.0f);
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
		m_projectiles.at(i)->move(dt);
		m_projectileLifeSpan.at(i) -= dt;
		if (m_projectileLifeSpan.at(i) < 0) {
			removeAt(i);
		}
	}
}

void ProjectileHandler::draw() {
	for (unsigned int i = 0; i < m_projectiles.size(); i++) {
		m_projectiles.at(i)->draw();
	}
}