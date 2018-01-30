#include "Grid.h"
#include "../../game/objects/Block.h"
#include "DirectXHelpers.h"

Grid::Grid() {
	m_gridWidth = 15;
	m_gridHeight = 5;
}

Grid::Grid(const int worldWidth, const int worldHeight, const int cellSize){
	m_gridWidth = worldWidth;
	m_gridHeight = worldHeight;
	m_cellSize = cellSize;
	
	time = 0.0f;
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


Block* Grid::raytraceBlock(const DirectX::SimpleMath::Vector3 &playerPos, const DirectX::SimpleMath::Vector3 &aimVec, float dt) {
	DirectX::SimpleMath::Vector2 currentPos = DirectX::SimpleMath::Vector2(playerPos.x, playerPos.y);
	DirectX::SimpleMath::Vector2 direction = DirectX::SimpleMath::Vector2(aimVec.x, aimVec.y);
	int deltaX, deltaY;
	Grid::Index currentIndex;
	float nextIntersectY, nextIntersectX;
	float tY, tX, t;

	if (aimVec.x > 0.f)
		deltaX = 1;
	else if (aimVec.x < 0.f)
		deltaX = -1;
	else {
		deltaX = 0;
		direction.x = 0.000001f;
	}

	if (aimVec.y > 0.f)
		deltaY = 1;
	else if (aimVec.y < 0.f)
		deltaY = -1;
	else {
		deltaY = 0;
		direction.y = 0.000001f;
	}

	currentIndex.x = static_cast<int>(floor(currentPos.x / m_cellSize));
	currentIndex.y = static_cast<int>(floor(currentPos.y / m_cellSize));

	bool intersection = false;

	while (!intersection) {
		if (deltaX > 0)
			nextIntersectX = float(deltaX + currentIndex.x) * m_cellSize;
		else
			nextIntersectX = float(currentIndex.x) * m_cellSize;
		 tX = DirectX::XMMax(fabs((nextIntersectX - currentPos.x)), 0.0001f) / direction.x;
		 
		if (deltaY > 0)
			nextIntersectY = float(deltaY + currentIndex.y) * m_cellSize;
		else
			nextIntersectY = float(currentIndex.y) * m_cellSize;

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

		 intersection = atGrid(currentIndex.x, currentIndex.y);

		 currentPos = currentPos + direction * t;
	}	

	time += dt;
	
	if (time > 3.0f) {
		std::cout << "x: " << currentIndex.x << " y: " << currentIndex.y << std::endl;
		std::cout << "xPos: " << playerPos.x << " yPos: " << playerPos.y << std::endl;
		std::cout << "xCurrent: " << currentPos.x << " yCurrent: " << currentPos.y << std::endl;
		time = 0.0f;
	}

	return m_cells[1][1];
}