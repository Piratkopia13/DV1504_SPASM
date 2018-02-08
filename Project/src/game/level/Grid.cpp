#include "Grid.h"
#include "../../game/objects/Block.h"
#include "DirectXHelpers.h"

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

std::vector<Grid::Index> Grid::getCollisionIndices(const AABB& boundingBox) {
	std::vector<Grid::Index> indices;
	DirectX::SimpleMath::Vector2 bottomLeft;
	DirectX::SimpleMath::Vector2 topRight;

	//Calculating minimum and maximum indices in the grid that the bounding box is in
	bottomLeft.x = boundingBox.getMinPos().x;
	bottomLeft.y = boundingBox.getMinPos().y;
	topRight.x = boundingBox.getMaxPos().x;
	topRight.y = boundingBox.getMaxPos().y;

	bottomLeft /= Level::DEFAULT_BLOCKSIZE;
	topRight /= Level::DEFAULT_BLOCKSIZE;

	int minX = static_cast<int>(floor(bottomLeft.x));
	int minY = static_cast<int>(floor(bottomLeft.y));
	int maxX = static_cast<int>(floor(topRight.x));
	int maxY = static_cast<int>(floor(topRight.y));

	/*minX = max(minX, 1);
	minY = max(minY, 1);
	maxX = min(m_gridWidth - 1, maxX);
	maxY = min(m_gridHeight - 1, maxY);

	for (int i = minX - 1; i < maxX + 1; i++) {
	for (int j = minY - 1; j < maxY + 1; j++) {
	if (m_cells[i][j] != nullptr) {
	Grid::Index index;
	index.x = i;
	index.y = j;
	indices.push_back(index);
	}
	}
	}*/
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

std::vector<Grid::Index> Grid::getCurrentCollisionIndices(const AABB& boundingBox) {
	std::vector<Grid::Index> indices;

	DirectX::SimpleMath::Vector2 minInGridCoords = boundingBox.getMinPos() / Level::DEFAULT_BLOCKSIZE;
	DirectX::SimpleMath::Vector2 maxInGridCoords = boundingBox.getMaxPos() / Level::DEFAULT_BLOCKSIZE;;

	if (minInGridCoords.x >= 0 && minInGridCoords.y >= 0 && maxInGridCoords.y < m_gridHeight && maxInGridCoords.x < m_gridWidth) {
		for (int x = (int)minInGridCoords.x; x <= maxInGridCoords.x; x++) {
			for (int y = (int)minInGridCoords.y; y <= maxInGridCoords.y; y++) {
				if (m_cells[x][y] != nullptr) {
					indices.push_back(Grid::Index{ x, y });
					//Logger::log("Hit!");
				}
			}
		}
	}

	return indices;
}

void Grid::setHoles(const std::vector<Grid::Index>& indices) {
	m_holes = indices;
}

bool Grid::checkHoles(const Grid::Index& playerPos) {
	bool cover = false;
	for (Grid::Index index : m_holes) {
		if (index.x == playerPos.x && index.y == playerPos.y)
			cover = true;
	}
	return cover;
}

std::vector<std::vector<Block*>>& Grid::getAllBlocks() {
	return m_cells;
}
