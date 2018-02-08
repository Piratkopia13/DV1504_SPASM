#pragma once

#include "../../sail/Sail.h"


class Block;
class Grid {
public:

	struct Index {
		int x;
		int y;
	};

	Grid();
	Grid(const int worldWidth, const int worldHeight);
	~Grid();

	void addBlock(Block* block, const int x, const int y);
	bool atGrid(const int x, const int y);
	// getCollisionIndices returns a list of all indices that a bounding box could collide with in the future
	std::vector<Index> getCollisionIndices(const AABB& boundingBox);
	// getCollisionIndices returns a list of all indices that a bounding box is currently colliding with
	std::vector<Index> getCurrentCollisionIndices(const AABB& boundingBox);

	std::vector<std::vector<Block*>>& getAllBlocks();

private:
	std::vector<std::vector<Block*>> m_cells;
	
	int m_gridWidth;
	int m_gridHeight;

};