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
	bool resolveProjectileCollisionWith(Character* chara);
	bool resolveCoverCollision(const DirectX::SimpleMath::Vector3& playerPos);
	bool checkLevelCollisionWith(Projectile* proj, DirectX::SimpleMath::Vector3& hit);
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