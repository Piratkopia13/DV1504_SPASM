#pragma once

#include "../../sail/Sail.h"
#include "../objects/common/Object.h"
#include "../objects/Block.h"
#include "../../sail/graphics/models/FbxModel.h"

namespace {
	static const std::string DEFAULT_LEVEL_LOCATION = "res/levels/";
	// The size in x- and y-axis of a block
	static const int DEFAULT_BLOCKSIZE = 1;
}

/**
	Handles all objects of a level
*/
class Level {

public:
	Level();
	Level(const std::string& filename);
	~Level();

	void update(const float delta);
	void render(const float delta);

private:
	// Number of blocks in the x-axis
	int m_width;
	// Number of blocks in the y-axis
	int m_height;
	
	std::vector<FbxModel> m_models;

};