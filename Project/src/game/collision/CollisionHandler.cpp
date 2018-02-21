#include "CollisionHandler.h"
#include "../objects/Character.h"
#include "../objects/common/Moveable.h"
#include "../level/Grid.h"
#include "../UpgradeHandler.h"


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

bool CollisionHandler::checkLevelCollisionWith(Projectile* proj, DirectX::SimpleMath::Vector3& hit, float dt) {

	auto& collisionIndices = m_level->getGrid()->getCurrentCollisionIndices(*proj->getBoundingBox());

	if (collisionIndices.size() > 0) {
		// TODO: calculate the exact hit point using the velocity and backtracking, and set hit to this point
		
		auto& currentPos = proj->getTransform().getTranslation();
		auto dir = proj->getVelocity();
		dir.Normalize();

		hit = rayTraceLevel(currentPos - proj->getVelocity() * dt, dir);

		// return hit
		return true;
	}

	// return no hit
	return false;
}

bool CollisionHandler::resolveCoverCollision(const DirectX::SimpleMath::Vector3& playerPos) {
	return m_level->getGrid()->checkHoles(Grid::convertToIndexed(playerPos));
}

bool CollisionHandler::resolveProjectileCollisionWith(Character* chara) {

	auto& projectiles = m_projectileHandler->getProjectiles();

	bool hit = false;
	for (unsigned int i = 0; i < projectiles.size(); i++) {
		if (projectiles.at(i)->getTeam() != chara->getTeam()) {
			auto* proj = projectiles.at(i);
			if (chara->getBoundingBox()->containsOrIntersects(*proj->getBoundingBox())) {
				chara->damage(proj->getDamage());
				m_projectileHandler->removeAt(i);
				hit = true;
			}
		}
	}

	return hit;

}

bool CollisionHandler::resolveUpgradeCollisionWith(Character * character) {

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



DirectX::SimpleMath::Vector3 CollisionHandler::rayTraceLevel(const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& dir) {
	
	DirectX::SimpleMath::Vector2 currentPos = origin;
	DirectX::SimpleMath::Vector2 direction = dir;
	DirectX::SimpleMath::Vector3 hitPos;
	int deltaX, deltaY;
	Grid::Index currentIndex;
	float nextIntersectY, nextIntersectX;
	float tY, tX, t;

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

	while (!intersection && currentIndex.x > -1 && currentIndex.x < (m_level->getGridWidth() - 1)) {
		if (deltaX > 0)
			nextIntersectX = float(deltaX + currentIndex.x) * Level::DEFAULT_BLOCKSIZE;
		else
			nextIntersectX = float(currentIndex.x) * Level::DEFAULT_BLOCKSIZE;
		tX = DirectX::XMMax(fabs((nextIntersectX - currentPos.x)), 0.0001f) / direction.x;

		if (deltaY > 0)
			nextIntersectY = float(deltaY + currentIndex.y) * Level::DEFAULT_BLOCKSIZE;
		else
			nextIntersectY = float(currentIndex.y) * Level::DEFAULT_BLOCKSIZE;

		tY = DirectX::XMMax(fabs((nextIntersectY - currentPos.y)), 0.0001f) / direction.y;

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

		intersection = m_level->getGrid()->atGrid(currentIndex.x, currentIndex.y);

		currentPos = currentPos + direction * t;
	}

	hitPos = DirectX::SimpleMath::Vector3(currentPos.x, currentPos.y, 0.0f);

	return hitPos;
}

CollisionHandler* CollisionHandler::getInstance() {
	return m_instance;
}
