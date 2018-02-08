#pragma once

#include "../../sail/Sail.h"

class Character;
class Projectile;
class CharacterHandler;

class CollisionHandler {
public:
	CollisionHandler(Level* level, CharacterHandler* charHandler, ProjectileHandler* projHandler);
	~CollisionHandler();

	void resolveLevelCollisionWith(Character* chara, float dt);
	void resolveLevelCollisionWith(Projectile* proj);
	bool resolveProjectileCollisionWith(Character* chara);
	DirectX::SimpleMath::Vector3 rayTraceLevel(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir);

	static CollisionHandler* getInstance();

private:
	// Singleton instance
	static CollisionHandler* m_instance;

	// Handles
	Level* m_level;
	CharacterHandler* m_characterHandler;
	ProjectileHandler* m_projectileHandler;

};