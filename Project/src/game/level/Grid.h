#pragma once

#include "../../sail/Sail.h"
#include "../../sail/graphics/shader/deferred/DynBlockDeferredInstancedGeometryShader.h"

class Grid {
public:
	//typedef DynBlockDeferredInstancedGeometryShader::InstanceData BlockInstance<

	struct Index {
		int x;
		int y;
	};

	static Index convertToIndexed(DirectX::SimpleMath::Vector3 pos) {
		int x = static_cast<int>(floor(pos.x));
		int y = static_cast<int>(floor(pos.y));

		return Index{ x, y };
	}

	static std::vector<Index> convertToIndexed(AABB* aabb) {
		int minX = static_cast<int>(floor(aabb->getMinPos().x));
		int minY = static_cast<int>(floor(aabb->getMinPos().y));
		int maxX = static_cast<int>(floor(aabb->getMaxPos().x));
		int maxY = static_cast<int>(floor(aabb->getMaxPos().y));

		std::vector<Index> toReturn;

		for (int i = minX; i < maxX + 1; i++)
			for (int j = minY; j < maxY + 1; j++)
				toReturn.push_back(Index{ i, j });

		return toReturn;
	}

	Grid();
	Grid(const int worldWidth, const int worldHeight);
	~Grid();

	void addBlock(DynBlockDeferredInstancedGeometryShader::InstanceData* block, const int x, const int y);

	bool checkHoles(const Index& index);

	void addHole(const int x, const int y);
	void addControlpoint(const int x, const int y);
	void addPlayerSpawnPoint(const int x, const int y);
	void addUpgradeSpawnPoint(const int x, const int y);
	bool atGrid(const int x, const int y);
	// getCollisionIndices returns a list of all indices that a bounding box could collide with in the future
	std::vector<Index> getCollisionIndices(const AABB& boundingBox);
	// getCollisionIndices returns a list of all indices that a bounding box is currently colliding with
	std::vector<Index> getCurrentCollisionIndices(const AABB& boundingBox);

	std::vector<std::vector<DynBlockDeferredInstancedGeometryShader::InstanceData*>>& getAllBlocks();
	std::vector<Index> & getControlpointIndices();
	std::vector<Index> & getPlayerSpawnPointIndices();
	std::vector<Index> & getUpgradeSpawnPointIndices();
	

private:
	std::vector<std::vector<DynBlockDeferredInstancedGeometryShader::InstanceData*>> m_cells;
	// Controlpoint indices
	std::vector<Index> m_cpIndices;
	// Player spawnpoint indices
	std::vector<Index> m_pspIndices;
	// Upgrade spawnpoint indices
	std::vector<Index> m_uspIndices;

	std::vector<Index> m_holes;
	
	int m_gridWidth;
	int m_gridHeight;

};