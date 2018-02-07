#pragma once

#include "../../sail/Sail.h"


class Block;
class Grid {
public:

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

	void addBlock(Block* block, const int x, const int y);
	bool atGrid(const int x, const int y);
	std::vector<Index> getCollisionIndices(const AABB& boundingBox);
	std::vector<std::vector<Block*>>& getAllBlocks();

private:
	std::vector<std::vector<Block*>> m_cells;
	
	int m_gridWidth;
	int m_gridHeight;

};