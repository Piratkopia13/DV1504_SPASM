#include "Grid.h"

Grid::Grid() {
	m_gridWidth = 15;
	m_gridHeight = 5;
}

Grid::Grid(const int worldWidth, const int worldHeight){
	m_gridWidth = worldWidth;
	m_gridHeight = worldHeight;

	for (int i = 0; i < m_gridWidth; i++) {
		for (int j = 0; j < m_gridHeight; j++) {
			m_cells[i][j].push_back(nullptr);
		}
	}
}

Grid::~Grid() {

}

void Grid::addObject(Object& object, const int x, const int y){
	m_cells[x][y].push_back(&object);
}
