#pragma once

#include "../../sail/Sail.h"
#include "../../sail/graphics/shader/deferred/DynBlockDeferredInstancedGeometryShader.h"
//#include "../objects/Character.h"

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
		float maxHP = 60.f;
		float currentHP = maxHP;
		DynBlockDeferredInstancedGeometryShader::InstanceData* data = nullptr;
		float respawnTime = 5.f;
		float timeDead = 0.0f;
		float implodingTimer = 0.0f;
		bool attacked = false;
		float resetTimer = 0.f;
		bool imploding = false;
		bool destroyed = false;
		bool respawned = false;
		bool respawning = false;
		bool blocked = false;
		void update(float delta) {
			if (destroyed) {
				timeDead += delta;
				if (timeDead > respawnTime && !blocked) {
					timeDead = 0.0f;
					destroyed = false;
					respawning = true;
					currentHP = maxHP;
					implodingTimer = 0.0f;
					data->modelMatrix = DirectX::SimpleMath::Matrix::CreateScale(100.0f) * data->modelMatrix;
				}
			}
			if (attacked) {
				resetTimer = 0.f;
				attacked = false;
			}
			resetTimer += delta;
			if (resetTimer > 5.f) {
				currentHP = maxHP;
			}
			if (imploding) {
				implodingTimer += delta;
				data->modelMatrix = DirectX::SimpleMath::Matrix::CreateScale(0.99f + (Utils::rnd() * 0.02f)) * data->modelMatrix;
				if (implodingTimer > 1.0f) {
					data->color = DirectX::SimpleMath::Vector3(implodingTimer * 2.f);
					data->modelMatrix = DirectX::SimpleMath::Matrix::CreateScale(0.9f + (Utils::rnd() * 0.02f))
						* DirectX::SimpleMath::Matrix::CreateRotationX(Utils::rnd())
						* DirectX::SimpleMath::Matrix::CreateRotationY(Utils::rnd())
						* DirectX::SimpleMath::Matrix::CreateRotationZ(Utils::rnd())
						* data->modelMatrix;
					destroyed = true;
				}
				if (implodingTimer > 1.5f) {
					data->color = DirectX::SimpleMath::Vector3::Zero;
					data->modelMatrix = DirectX::SimpleMath::Matrix::CreateScale(0.01f) * DirectX::SimpleMath::Matrix::CreateTranslation(data->modelMatrix.Translation());
					imploding = false;
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

	void blockHit(const DirectX::SimpleMath::Vector3& projVelocity, const float damage, const DirectX::SimpleMath::Vector3& hitPos);
	void setBlockVariation(const int x, const int y);
	void updateAdjacent(const int x, const int y);

private:
	static const unsigned int BLOCK_VARIATIONS;

	// Number of blocks in the x-axis
	int m_width;
	// Number of blocks in the y-axis
	int m_height;
	bool m_destructible;
	
	// Models used in the level
	std::vector<std::unique_ptr<FbxModel>> m_models;
	// Blocks placed in the level
	//std::vector<std::unique_ptr<Block>> m_blocks;
	std::unique_ptr<InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>> m_instancedBlocks;

	// Grid of the level
	std::unique_ptr<Grid> m_grid;

	std::vector<std::vector<LevelBlock>> m_blocks;
};