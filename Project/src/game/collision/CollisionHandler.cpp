#include "CollisionHandler.h"
#include "../objects/Character.h"
#include "../objects/common/Moveable.h"
#include "../level/Grid.h"
#include "../UpgradeHandler.h"
#include "../CharacterHandler.h"

using namespace DirectX::SimpleMath;

CollisionHandler* CollisionHandler::m_instance = nullptr;

CollisionHandler::CollisionHandler(Level* level, CharacterHandler* charHandler, ProjectileHandler* projHandler, UpgradeHandler* upHandler)
	: m_level(level)
	, m_characterHandler(charHandler)
	, m_projectileHandler(projHandler)
	, m_upgradeHandler(upHandler)
{
	// Set up instance if not set
	if (m_instance) {
		Logger::Error("Only one CollisionHandler can exist!");
		return;
	}
	m_instance = this;
}

CollisionHandler::~CollisionHandler() {
	m_instance = nullptr;
}

void CollisionHandler::resolveLevelCollisionWith(Character* chara, float dt) {

	Moveable& moveable = *dynamic_cast<Moveable*>(chara);
	DirectX::SimpleMath::Vector3 toMove(0.f, 0.f, 0.f);

	moveable.setGrounded(false);

	AABB tempBB(*moveable.getBoundingBox());

	float EPS = 0.01f;
	DirectX::SimpleMath::Vector3 mMin = tempBB.getMinPos();
	DirectX::SimpleMath::Vector3 mMax = tempBB.getMaxPos();
	DirectX::SimpleMath::Vector3 mVel = moveable.getVelocity() * dt;

	std::vector<Grid::Index> indices = m_level->getGrid()->getCollisionIndices(tempBB);

	if (indices.size() > 0) {
		bool colX = false;
		bool colY = false;
		for (Grid::Index index : indices) {
			float bMinX = index.x * Level::DEFAULT_BLOCKSIZE;
			float bMaxX = (index.x + 1) * Level::DEFAULT_BLOCKSIZE;
			float bMinY = index.y * Level::DEFAULT_BLOCKSIZE;
			float bMaxY = (index.y + 1) * Level::DEFAULT_BLOCKSIZE;

			if (mMax.x + mVel.x > bMinX && mMin.x + mVel.x < bMaxX &&
				mMax.y > bMinY && mMin.y < bMaxY) {
				if (mVel.x < 0.f)
					toMove.x = bMaxX - mMin.x + EPS;
				else if (mVel.x > 0.f)
					toMove.x = bMinX - mMax.x - EPS;
			}

			if (mMax.y + mVel.y + EPS > bMinY && mMin.y + mVel.y - EPS < bMaxY &&
				mMax.x > bMinX && mMin.x < bMaxX) {
				if (mVel.y < 0.f) {
					toMove.y = bMaxY - mMin.y + EPS;
				}
				else if (mVel.y > 0.f)
					toMove.y = bMinY - mMax.y - EPS;
			}

			if (toMove.x && !(colX)) {
				colX = true;
				if (abs(toMove.x) <= EPS) toMove.x = 0.f;
				DirectX::SimpleMath::Vector3 tempVel = moveable.getVelocity();
				tempVel.x = 0.f;
				moveable.setVelocity(tempVel);
				toMove.y = 0.f;
				moveable.move(toMove);
			}

			if (toMove.y && !(colY)) {
				colY = true;
				if (abs(toMove.y) <= EPS) toMove.y = 0.f;
				DirectX::SimpleMath::Vector3 tempVel = moveable.getVelocity();
				tempVel.y = 0.f;
				moveable.setVelocity(tempVel);
				toMove.x = 0.f;
				moveable.move(toMove);
			}
		}

		if (!(colX || colY)) {
			for (Grid::Index index : indices) {
				float bMinX = index.x * Level::DEFAULT_BLOCKSIZE;
				float bMaxX = (index.x + 1) * Level::DEFAULT_BLOCKSIZE;
				float bMinY = index.y * Level::DEFAULT_BLOCKSIZE;
				float bMaxY = (index.y + 1) * Level::DEFAULT_BLOCKSIZE;

				if (mMax.x + mVel.x > bMinX && mMin.x + mVel.x < bMaxX &&
					mMax.y + mVel.y > bMinY && mMin.y + mVel.y < bMaxY) {
					if (mVel.x < 0.f)
						toMove.x = bMaxX - mMin.x + EPS;
					else if (mVel.x > 0.f)
						toMove.x = bMinX - mMax.x - EPS;

					if (mVel.y < 0.f)
						toMove.y = bMaxY - mMin.y + EPS;
					else if (mVel.y > 0.f)
						toMove.y = bMinY - mMax.y - EPS;
				}

				if (abs(toMove.x) < abs(toMove.y)) {
					if (abs(toMove.x) <= EPS) toMove.x = 0.f;
					DirectX::SimpleMath::Vector3 tempVel = moveable.getVelocity();
					tempVel.x = 0.f;
					moveable.setVelocity(tempVel);
					toMove.y = 0.f;
					moveable.move(toMove);
				}

				if (abs(toMove.y) < abs(toMove.x)) {
					if (abs(toMove.y) <= EPS) toMove.y = 0.f;
					DirectX::SimpleMath::Vector3 tempVel = moveable.getVelocity();
					tempVel.y = 0.f;
					moveable.setVelocity(tempVel);
					toMove.x = 0.f;
					moveable.move(toMove);
				}
			}
		}

		for (Grid::Index index : indices) {
			float bMinX = index.x * Level::DEFAULT_BLOCKSIZE;
			float bMaxX = (index.x + 1) * Level::DEFAULT_BLOCKSIZE;
			float bMinY = index.y * Level::DEFAULT_BLOCKSIZE;
			float bMaxY = (index.y + 1) * Level::DEFAULT_BLOCKSIZE;

			if (mMax.y - EPS * 2.0 > bMinY && mMin.y - EPS * 2.0 < bMaxY &&
				mMax.x > bMinX && mMin.x < bMaxX)
				moveable.setGrounded(true);
		}
	}

}

bool CollisionHandler::rayTraceAABB(const Ray& ray, const AABB& bb, Vector3& hitPoint, float& hitT) {

	auto& min = bb.getMinPos();
	auto& max = bb.getMaxPos();

	static float EPS = 0.0000000000001f;

	float tmin = (min.x - ray.o.x) / ( ray.d.x + EPS );
	float tmax = (max.x - ray.o.x) / ( ray.d.x + EPS );
	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (min.y - ray.o.y) / ( ray.d.y + EPS );
	float tymax = (max.y - ray.o.y) / ( ray.d.y + EPS );
	if (tymin > tymax) std::swap(tymin, tymax);

	if (tmin > tymax || tymin > tmax) return false;

	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;

	float tzmin = (min.z - ray.o.z) / ( ray.d.z + EPS );
	float tzmax = (max.z - ray.o.z) / ( ray.d.z + EPS );
	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if (tmin > tzmax || tzmin > tmax) return false;

	if (tzmin > tmin) tmin = tzmin;
	if (tzmax > tmax) tmax = tzmax;
	
	if (tmin < 0.f) return false;

	hitPoint = ray.o + ray.d * tmin;
	hitT = tmin;
	return true;
}

bool CollisionHandler::checkLevelCollisionWith(Projectile* proj, float dt, float& t, Vector3& hitPos) {

	Vector3 normalizedVel = proj->getVelocity();
	normalizedVel.Normalize();
	Vector3 projPos = proj->getTransform().getTranslation();
	projPos.z = 0.f;

	// Ray trace the distance the projectile will travel this frame
	bool rayHit = rayTraceLevel({ projPos, normalizedVel }, hitPos, t);
	if (rayHit && t <= proj->getVelocity().Length() * dt) {
		// return hit
		return true;
	}

	// return no hit
	return false;
}

bool CollisionHandler::resolveCoverCollision(const DirectX::SimpleMath::Vector3& playerPos) {
	return m_level->getGrid()->checkHoles(Grid::convertToIndexed(playerPos));
}

bool CollisionHandler::outOfBounds(Character* character) {
	float x = character->getTransform().getTranslation().x;
	float y = character->getTransform().getTranslation().y;
	bool oob = false;
	if (x < 0 || y < 0 || x > m_level->getGridWidth() || y > m_level->getGridHeight())
		oob = true;

	return oob;
}

bool CollisionHandler::outOfBounds(Projectile* projectile) {
	float x = projectile->getTransform().getTranslation().x;
	float y = projectile->getTransform().getTranslation().y;
	bool oob = false;
	if (x < -5.f || y < -5.f || x > m_level->getGridWidth() + 5.f || y > m_level->getGridHeight() + 5.f)
		oob = true;

	return oob;
}

bool CollisionHandler::checkCharacterCollisionWith(Projectile* proj, float dt, float& t, Character** hitCharacter, CharacterHitResult& hitResult) {

	bool hit = false;
	
	for (unsigned int i = 0; i < m_characterHandler->getNrOfPlayers(); i++) {
		Character* chara = m_characterHandler->getCharacter(i);
		if (proj->getTeam() != chara->getTeam()) {

			Vector3 normalizedVel = proj->getVelocity();
			normalizedVel.Normalize();
			Vector3 projPos = proj->getTransform().getTranslation();
			projPos.z = 0.f;
			// Ray trace the distance the projectile will travel this frame
			Vector3 hitPoint;
			bool rayHit = rayTraceAABB({ projPos, normalizedVel }, *chara->getBoundingBox(), hitPoint, t);

			if (rayHit && t <= proj->getVelocity().Length() * dt) {
				hitResult.hitDmg = proj->getDamage();
				hitResult.attacker = proj->getOwner();
				hitResult.knockbackDir = proj->getVelocity();
				hitResult.knockbackDir.Normalize();
				hitResult.hitPos = hitPoint;
				hitResult.knockbackAmount = proj->getKnockbackAmount();
				*hitCharacter = chara;
				
				//chara->hitByProjectile(hitResult);

				hit = true;
				break;
			}
		}
	}

	return hit;

}

bool CollisionHandler::resolveUpgradeCollisionWith(Character* character) {

	size_t t = m_upgradeHandler->getNrOfSpawners();

	for (size_t i = 0; i < t; i++) {
		UpgradeHandler::UpgradeSpawn* spawn = m_upgradeHandler->getSpawn(i);
		if (spawn->getOnline()) {
			DirectX::SimpleMath::Vector2 pMinPos = character->getBoundingBox()->getMinPos();
			DirectX::SimpleMath::Vector2 pMaxPos = character->getBoundingBox()->getMaxPos();
			DirectX::SimpleMath::Vector2 uMinPos = spawn->getBoundingBox()->getMinPos();
			DirectX::SimpleMath::Vector2 uMaxPos = spawn->getBoundingBox()->getMaxPos();
			uMaxPos.y += 0.7f;
			
			if (pMinPos.x <= uMaxPos.x && pMinPos.y <= uMaxPos.y && pMaxPos.x >= uMinPos.x && pMaxPos.y >= uMinPos.y)
				character->addUpgrade(spawn->take());
		}
	}



	return false;
}



bool CollisionHandler::rayTraceLevel(const Ray& ray, DirectX::SimpleMath::Vector3& hitPos, float& hitT) {
	
	DirectX::SimpleMath::Vector2 currentPos = ray.o;
	DirectX::SimpleMath::Vector2 direction = ray.d;
	int deltaX, deltaY;
	Grid::Index currentIndex;
	float nextIntersectY, nextIntersectX;
	float tY, tX, t;
	float tTotal = 0.f;

	DirectX::SimpleMath::Vector3 test;

	if (direction.x > 0.f)
		deltaX = 1;
	else if (direction.x < 0.f)
		deltaX = -1;
	else {
		deltaX = 0;
		direction.x = 0.000001f;
	}

	if (direction.y > 0.f)
		deltaY = 1;
	else if (direction.y < 0.f)
		deltaY = -1;
	else {
		deltaY = 0;
		direction.y = 0.000001f;
	}

	currentIndex.x = static_cast<int>(floor(currentPos.x / Level::DEFAULT_BLOCKSIZE));
	currentIndex.y = static_cast<int>(floor(currentPos.y / Level::DEFAULT_BLOCKSIZE));

	bool intersection = false;
	if (m_level->getGrid()->atGrid(currentIndex.x, currentIndex.y))
		intersection = true;

	while (!intersection) {
		if (deltaX > 0)
			nextIntersectX = float(deltaX + currentIndex.x) * Level::DEFAULT_BLOCKSIZE;
		else
			nextIntersectX = float(currentIndex.x) * Level::DEFAULT_BLOCKSIZE;
		tX = max(fabs((nextIntersectX - currentPos.x)), 0.0001f) / direction.x;

		if (deltaY > 0)
			nextIntersectY = float(deltaY + currentIndex.y) * Level::DEFAULT_BLOCKSIZE;
		else
			nextIntersectY = float(currentIndex.y) * Level::DEFAULT_BLOCKSIZE;

		tY = max(fabs((nextIntersectY - currentPos.y)), 0.0001f) / direction.y;

		tX = fabs(tX);
		tY = fabs(tY);

		if (tX > tY) {
			t = tY;
			currentIndex.y += deltaY;
		}
		else {
			t = tX;
			currentIndex.x += deltaX;
		}

		if (currentIndex.x < 0 || currentIndex.y < 0 || currentIndex.x > m_level->getGridWidth() || currentIndex.y > m_level->getGridHeight()) {
			return false;
		}
		else {
			intersection = m_level->getGrid()->atGrid(currentIndex.x, currentIndex.y);
		}

		currentPos = currentPos + direction * t;
		tTotal += t;
	}

	hitPos = DirectX::SimpleMath::Vector3(currentPos.x, currentPos.y, 0.0f);
	hitT = tTotal;

	return true;
}

bool CollisionHandler::resolveProjectileCollision(float dt) {

	auto& projectiles = m_projectileHandler->getProjectiles();

	for (unsigned int i = 0; i < projectiles.size(); i++) {
		auto& proj = projectiles.at(i);

		if (!outOfBounds(proj)) {

			bool hit = false;

			float tLevel = -1.f;
			float tCharacter = -1.f;
			Vector3 levelHitPos;
			Character* hitCharacter = nullptr;
			CharacterHitResult charaHitRes;
			bool levelHit = checkLevelCollisionWith(proj, dt, tLevel, levelHitPos);
			bool characterHit = checkCharacterCollisionWith(proj, dt, tCharacter, &hitCharacter, charaHitRes);

			if (levelHit && characterHit) {

				// Only the closest should be hit
				if (tLevel < tCharacter)
					characterHit = false;
				else
					levelHit = false;

			}

			if (levelHit) {
				m_projectileHandler->projectileHitLevel(levelHitPos);
				m_projectileHandler->projectileHitSomething(proj, levelHitPos, dt);
			}
			else if (characterHit) {
				hitCharacter->hitByProjectile(charaHitRes);
				m_projectileHandler->projectileHitSomething(proj, charaHitRes.hitPos, dt);
			}




			if (levelHit || characterHit) {
				m_projectileHandler->removeAt(i);
				i--;
			}
		}
		else {
			m_projectileHandler->removeAt(i);
			i--;
		}

	}


	return false;
}

CollisionHandler* CollisionHandler::getInstance() {
	return m_instance;
}
