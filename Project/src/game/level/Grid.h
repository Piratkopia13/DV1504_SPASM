#pragma once

#include "../../sail/Sail.h"

class Block;
class Grid {
public:
	Grid();
	Grid(const int worldWidth, const int worldHeight);
	~Grid();

	
	void addBlock(Block* block, const int x, const int y);
	bool atGrid(const int x, const int y);

private:
	std::vector<std::vector<Block*>> m_cells;
	
	int m_gridWidth;
	int m_gridHeight;
	

};