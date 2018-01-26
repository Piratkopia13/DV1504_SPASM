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

	float test;
	
	void addBlock(Block* block, const int x, const int y);
	bool atGrid(const int x, const int y);
	std::vector<Index> getCollisionIndices(const AABB& boundingBox, const float cellSize);

private:
	std::vector<std::vector<Block*>> m_cells;
	
	int m_gridWidth;
	int m_gridHeight;
	

};