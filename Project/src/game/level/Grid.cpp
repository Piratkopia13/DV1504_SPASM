#include "Grid.h"
#include "../../game/objects/Block.h"

Grid::Grid() {
	m_gridWidth = 15;
	m_gridHeight = 5;
}

Grid::Grid(const int worldWidth, const int worldHeight, const int cellSize){
	m_gridWidth = worldWidth;
	m_gridHeight = worldHeight;
	m_cellSize = cellSize;

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


Block* Grid::raytraceBlock(const DirectX::SimpleMath::Vector3 &playerPos, const DirectX::SimpleMath::Vector3 &aimVec) {
	DirectX::SimpleMath::Vector2 currentPos = DirectX::SimpleMath::Vector2(playerPos.x, playerPos.y);
	int deltaX, deltaY;
	Grid::Index currentIndex;
	float nextIntersectY, nextIntersectX;
	float tY, tX, t;

	if (aimVec.x > 0.f)
		deltaX = 1;
	else if (aimVec.x < 0.f)
		deltaX = -1;
	else
		deltaX = 0;

	if (aimVec.y > 0.f)
		deltaY = 1;
	else if (aimVec.y < 0.f)
		deltaY = -1;
	else
		deltaY = 0;

	currentIndex.x = static_cast<int>(floor(currentPos.x / m_cellSize));
	currentIndex.y = static_cast<int>(floor(currentPos.y / m_cellSize));

	bool intersection = false;

	while (!intersection) {
		 nextIntersectX = float(deltaX + currentIndex.x) * m_cellSize;
		 
		 tX = (nextIntersectX - currentPos.x) / aimVec.x;
		 
		 nextIntersectY = float(deltaY + currentIndex.y) * m_cellSize;

		 tY = (nextIntersectY - currentPos.y) / aimVec.y;

		 if (tX > tY) {
			 t = tY;
			 currentIndex.y += deltaY;
		 }
		 else {
			 t = tX;
			 currentIndex.x += deltaX;
		 }

		 intersection = atGrid(currentIndex.x, currentIndex.y);

		 if (!intersection) {
			 currentPos = currentPos + aimVec * t;
		 }
	}
	
	return m_cells[currentIndex.x][currentIndex.y];
}