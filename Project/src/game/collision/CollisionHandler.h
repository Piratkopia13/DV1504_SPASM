#pragma once

#include "../../sail/Sail.h"

class Character;
class Projectile;
class CharacterHandler;
class UpgradeHandler;

class CollisionHandler {
public:
	struct Ray {
		DirectX::SimpleMath::Vector3 o;
		DirectX::SimpleMath::Vector3 d;
	};
	struct CharacterHitResult {
		float hitDmg;
		int attacker;
		DirectX::SimpleMath::Vector3 knockbackDir;
		DirectX::SimpleMath::Vector3 hitPos;
		float knockbackAmount;
	};
public:
	CollisionHandler(Level* level, CharacterHandler* charHandler, ProjectileHandler* projHandler, UpgradeHandler* upHandler);
	~CollisionHandler();

	void resolveLevelCollisionWith(Character* chara, float dt);
	bool rayTraceAABB(const Ray& ray, const AABB& bb, DirectX::SimpleMath::Vector3& hitPos, float& hitT);
	bool checkCharacterCollisionWith(Projectile* proj, float dt, float& t, Character** hitCharacter, CharacterHitResult& hitResult);
	bool checkLevelCollisionWith(Projectile* proj, float dt, float& t, DirectX::SimpleMath::Vector3& hitPos);
	bool resolveCoverCollision(const DirectX::SimpleMath::Vector3& playerPos);
	bool resolveUpgradeCollisionWith(Character* character);
	bool outOfBounds(Character* character);
	bool rayTraceLevel(const Ray& ray, DirectX::SimpleMath::Vector3& hitPos, float& hitT);

	bool resolveProjectileCollision(float dt);

	static CollisionHandler* getInstance();

private:
	// Singleton instance
	static CollisionHandler* m_instance;

	// Handles
	Level* m_level;
	CharacterHandler* m_characterHandler;
	ProjectileHandler* m_projectileHandler;
	UpgradeHandler* m_upgradeHandler;

};