#pragma once

#include "../../sail/Sail.h"
#include "../../sail/graphics/shader/deferred/DynBlockDeferredInstancedGeometryShader.h"
//#include "Grid.h"

namespace {
	static const std::string DEFAULT_LEVEL_LOCATION = "res/levels/";
	// The size in x- and y-axis of a block
	static const float DEFAULT_SCALING = 1.0f;
}

/**
	Handles all objects of a level
*/
class CharacterHandler;
template <class T, typename D>
class InstancedBlocks;
//class DynBlockDeferredInstancedGeometryShader;
//public:
//	class InstanceData;
//};
class Grid;
class Moveable;
class Level {

public:
	static const float DEFAULT_BLOCKSIZE;

	struct LevelBlock {
		int health = 5;
		DynBlockDeferredInstancedGeometryShader::InstanceData* data = nullptr;
		float respawnTime = 3.f;
		float timeDead = 0.0f;
		bool destroyed = false;
		void update(float delta) {
			if (destroyed) {
				timeDead += delta;
				if (timeDead > respawnTime) {
					timeDead = 0.0f;
					destroyed = false;
					health = 5;
					data->modelMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(data->modelMatrix.Translation().x, data->modelMatrix.Translation().y, 0.f);
					data->color = DirectX::SimpleMath::Vector3::One;
				}
			}
		}
	};

	Level(const std::string& filename);
	~Level();

	void update(const float delta, CharacterHandler* charHandler);
	void draw();

	Grid* getGrid();
	DirectX::SimpleMath::Vector2 getGridWorldSize();
	const int& getGridWidth() const;
	const int& getGridHeight() const;

	void blockHit(const DirectX::SimpleMath::Vector3& projVelocity,const DirectX::SimpleMath::Vector3& hitPos);
	void setBlockVariation(const int x, const int y);
	void updateAdjacent(const int x, const int y);

private:
	static const unsigned int BLOCK_VARIATIONS;

	// Number of blocks in the x-axis
	int m_width;
	// Number of blocks in the y-axis
	int m_height;
	
	// Models used in the level
	std::vector<std::unique_ptr<FbxModel>> m_models;
	// Blocks placed in the level
	//std::vector<std::unique_ptr<Block>> m_blocks;
	std::unique_ptr<InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>> m_instancedBlocks;

	// Grid of the level
	std::unique_ptr<Grid> m_grid;

	std::vector<std::vector<LevelBlock>> m_blocks;
};