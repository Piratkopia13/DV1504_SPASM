#include "Grid.h"
#include "../../game/objects/Block.h"

Grid::Grid() {
	m_gridWidth = 15;
	m_gridHeight = 5;
}

Grid::Grid(const int worldWidth, const int worldHeight){
	m_gridWidth = worldWidth;
	m_gridHeight = worldHeight;

	m_cells = std::vector<std::vector<Block*>>(m_gridWidth, std::vector<Block*>(m_gridHeight, nullptr));
}

Grid::~Grid() {

}

void Grid::addBlock(Block* block, const int x, const int y){
	m_cells[x][y] = block;
}

bool Grid::atGrid(const int x, const int y) {
	if (m_cells[x][y])
		return true;
	return false;
}
