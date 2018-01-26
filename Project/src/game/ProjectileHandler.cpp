#include "ProjectileHandler.h"

ProjectileHandler::ProjectileHandler(DeferredRenderer& deferredRenderer) {
	m_projectileModel1 = std::make_unique<FbxModel>("block.fbx");
	m_projectileModel1->getModel()->buildBufferForShader(&deferredRenderer.getGeometryShader());
}

ProjectileHandler::~ProjectileHandler() {

}

std::vector<Projectile>* ProjectileHandler::getProjectiles() {
	return &m_projectiles;
}

void ProjectileHandler::addProjectile(Projectile newProjectile) {
	newProjectile.setModel(m_projectileModel1->getModel());
	m_projectiles.push_back(newProjectile);
}

void ProjectileHandler::removeAt(int index) {
	std::swap(m_projectiles.at(index), m_projectiles.back());
	m_projectiles.pop_back();
}

void ProjectileHandler::update(float dt) {
	for (unsigned int i = 0; i < m_projectiles.size(); i++) {
		m_projectiles.at(i).move(dt);
	}
}

void ProjectileHandler::draw() {
	for (unsigned int i = 0; i < m_projectiles.size(); i++) {
		m_projectiles.at(i).draw();
	}
}