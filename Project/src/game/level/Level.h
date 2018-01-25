#pragma once

#include "../../sail/Sail.h"
#include "../../sail/graphics/models/FbxModel.h"
#include "../../sail/graphics/renderer/DeferredRenderer.h"

namespace {
	static const std::string DEFAULT_LEVEL_LOCATION = "res/levels/";
	// The size in x- and y-axis of a block
	static const float DEFAULT_BLOCKSIZE = 8.f;
}

/**
	Handles all objects of a level
*/
class Block;
class Level {

public:
	Level(const std::string& filename, DeferredRenderer& deferredRenderer);
	~Level();

	void update(const float delta);
	void draw();

private:
	// Number of blocks in the x-axis
	int m_width;
	// Number of blocks in the y-axis
	int m_height;
	
	std::vector<std::unique_ptr<FbxModel>> m_models;
	std::vector<std::unique_ptr<Block>> m_blocks;

};