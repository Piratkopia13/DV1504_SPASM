#pragma once

#include "../../sail/Sail.h"

class Character;
class Projectile;

class CollisionHandler {
public:
	CollisionHandler();
	~CollisionHandler();

	void resolveLevelCollisionWith(Character* chara);
	void resolveLevelCollisionWith(Projectile* proj);
	void resolveProjectileCollisionWIth(Character* chara);
	DirectX::SimpleMath::Vector3 rayTraceLevel(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir);

private:
};