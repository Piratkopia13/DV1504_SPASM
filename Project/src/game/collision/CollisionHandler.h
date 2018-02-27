#pragma once

#include "../../sail/Sail.h"

class Character;
class Projectile;
class CharacterHandler;
class UpgradeHandler;

class CollisionHandler {
public:
	CollisionHandler(Level* level, CharacterHandler* charHandler, ProjectileHandler* projHandler, UpgradeHandler* upHandler);
	~CollisionHandler();

	void resolveLevelCollisionWith(Character* chara, float dt);
	bool resolveProjectileCollisionWith(Character* chara, DirectX::SimpleMath::Vector3& knockbackDir, float& hitDmg, float& knockbackAmount, int& attacker);
	bool resolveCoverCollision(const DirectX::SimpleMath::Vector3& playerPos);
	bool checkLevelCollisionWith(Projectile* proj, DirectX::SimpleMath::Vector3& hit, float dt = 0.f);
	bool resolveUpgradeCollisionWith(Character* character);
	bool outOfBounds(Character* character);
	DirectX::SimpleMath::Vector3 rayTraceLevel(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir);

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