#pragma once

#include "../sail/Sail.h"
#include "../game/objects/common/Object.h"

class Grid {
public:
	Grid();
	Grid(const int worldWidth, const int worldHeight);
	~Grid();

	
	void addObject(Object& object, const int x, const int y);
	std::vector<Object*> getObjects(const float x, const float y, const float width, const float height);
	
	//bool inCell(const float x, const float y, const float width, const float height);

private:
	std::vector<std::vector<std::vector<Object*>>> m_cells;
	
	int m_gridWidth;
	int m_gridHeight;
	

};