#include "CollisionHandler.h"

CollisionHandler* CollisionHandler::m_instance = nullptr;

CollisionHandler::CollisionHandler(Level* level, CharacterHandler* charHandler, ProjectileHandler* projHandler)
	: m_level(level)
	, m_characterHandler(charHandler)
	, m_projectileHandler(projHandler)
{
	// Set up instance if not set
	if (m_instance) {
		Logger::Error("Only one CollisionHandler can exist!");
		return;
	}
	m_instance = this;
}

CollisionHandler::~CollisionHandler() {
}

void CollisionHandler::resolveLevelCollisionWith(Character* chara) {
}

void CollisionHandler::resolveLevelCollisionWith(Projectile* proj) {
}

void CollisionHandler::resolveProjectileCollisionWith(Character* chara) {
}


DirectX::SimpleMath::Vector3 CollisionHandler::rayTraceLevel(const DirectX::SimpleMath::Vector3 & origin, const DirectX::SimpleMath::Vector3 & dir) {
	return DirectX::SimpleMath::Vector3();
}

CollisionHandler* CollisionHandler::getInstance() {
	return m_instance;
}
