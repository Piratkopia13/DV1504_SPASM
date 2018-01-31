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
	m_cellSize = cellSize;	m_cells = std::vector<std::vector<Block*>>(m_gridWidth, std::vector<Block*>(m_gridHeight, nullptr));
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

std::vector<Grid::Index> Grid::getCollisionIndices(const AABB& boundingBox, const float cellSize) {
	std::vector<Grid::Index> indices;
	DirectX::SimpleMath::Vector2 bottomLeft;
	DirectX::SimpleMath::Vector2 topRight;

	//Calculating minimum and maximum indices in the grid that the bounding box is in
	bottomLeft.x = boundingBox.getMinPos().x;
	bottomLeft.y = boundingBox.getMinPos().y;
	topRight.x = boundingBox.getMaxPos().x;
	topRight.y = boundingBox.getMaxPos().y;

	bottomLeft /= cellSize;
	topRight /= cellSize;

	int minX = static_cast<int>(floor(bottomLeft.x));
	int minY = static_cast<int>(floor(bottomLeft.y));
	int maxX = static_cast<int>(floor(topRight.x));
	int maxY = static_cast<int>(floor(topRight.y));

	//Getting all the boxes surrounding the bounding box
	int top = maxY + 1;
	int bottom = minY - 1;
	if (minX > 0 && minY > 0 && maxY < m_gridHeight - 1 && maxX < m_gridWidth - 1) {
		for (int i = minX - 1; i < maxX + 2; i++) {
			if (m_cells[i][bottom] != nullptr) {
				Grid::Index index;
				index.x = i;
				index.y = bottom;
				indices.push_back(index);
			}
			if (m_cells[i][top] != nullptr) {
				Grid::Index index;
				index.x = i;
				index.y = top;
				indices.push_back(index);
			}
		}
		top = maxX + 1;
		bottom = minX - 1;
		for (int i = minY; i < maxY + 1; i++) {
			if (m_cells[bottom][i] != nullptr) {
				Grid::Index index;
				index.x = bottom;
				index.y = i;
				indices.push_back(index);
			}
			if (m_cells[top][i] != nullptr) {
				Grid::Index index;
				index.x = top;
				index.y = i;
				indices.push_back(index);
			}
		}
	}

	return indices;
}


DirectX::SimpleMath::Vector3 Grid::raytraceBlock(const DirectX::SimpleMath::Vector3 &playerPos, const DirectX::SimpleMath::Vector3 &aimVec) {
	DirectX::SimpleMath::Vector2 currentPos = DirectX::SimpleMath::Vector2(playerPos.x, playerPos.y);
	DirectX::SimpleMath::Vector2 direction = DirectX::SimpleMath::Vector2(aimVec.x, aimVec.y);
	DirectX::SimpleMath::Vector3 hitPos;
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

	while (!intersection && currentIndex.x > -1 && currentIndex.x < (m_gridWidth - 1)) {
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

	hitPos = DirectX::SimpleMath::Vector3(currentPos.x, currentPos.y, 0.0f);

	return hitPos;
}
