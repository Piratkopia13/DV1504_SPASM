#include "Level.h"
#include "../objects/Block.h"
#include "Grid.h"
#include "../objects/common/Moveable.h"
#include "../objects/InstancedBlocks.h"

#include <string>
#include <fstream>
#include <cctype>

using namespace DirectX::SimpleMath;

const float Level::DEFAULT_BLOCKSIZE = 1.0f;
const unsigned int Level::BLOCK_VARIATIONS = 16;

Level::Level(const std::string& filename) 
	: m_grid(nullptr)
{
	m_instancedBlocks = std::make_unique<InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>>();

	std::ifstream infile(DEFAULT_LEVEL_LOCATION + filename);

	float variationOffsetRowMul = 1.f / BLOCK_VARIATIONS;

	if (infile) {		

		std::string line;
		unsigned int currTask = 0;
		unsigned int x = 0;
		unsigned int y = 0;

		char** mapData = nullptr;

		UINT blockCount = 0;

		while (infile >> line) {
			if (!line.compare("attributes")) {
				currTask = 1;
				infile >> line;
			}
			if (!line.compare("map")) {
				currTask = 2;
				infile >> line;
				m_width = line.length();

				// Count the map height and num blocks
				m_height = 0;
				std::string mapLine = line;
				auto seekPos = infile.tellg();
				// Valid map line until first character in line is not a digit
				bool countHeight = true;
				while (true) {
					// Only count on lines that are part of the map (beginning with a digit)
					if (std::isdigit(mapLine[0])) {
						if (countHeight) {
							m_height++;
						}
						for (char& c : mapLine) {
							if (c == '1')
								blockCount++;
						}
					} else {
						countHeight = false;
					}
					if (!(infile >> mapLine)) break;
				}
				// Seek back to beginning of map
				infile.clear();
				infile.seekg(seekPos);

				// Init instance data
				m_instancedBlocks->init(blockCount);

				y = m_height;

				m_grid = std::make_unique<Grid>(m_width, m_height);

				// Allocate memory to store map data
				mapData = new char*[m_width];
				for (int i = 0; i < m_width; i++) {
					mapData[i] = new char[m_height];
				}

			}
			if (!line.compare("depth1")) {
				currTask = 3;
				infile >> line;
				y = m_height;
			}
			if (!line.compare("depth2")) {
				currTask = 4;
				infile >> line;
				y = m_height;
			}

			switch (currTask) {

			case 0: // Load models
				m_models.push_back(std::make_unique<FbxModel>(line));
				m_models.back()->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());
				break;

			case 1:
				// TODO: parse attributes here
				break;

			case 2: // Load map
			case 3:
			case 4:
				x = 0;
				for (char& c : line) {

					if (currTask == 2) {
						mapData[x][y - 1] = c;
					} else if (c == '1') {
						DynBlockDeferredInstancedGeometryShader::InstanceData instanceData;
						instanceData.blockVariationOffset = 1 * variationOffsetRowMul;
						instanceData.color = Vector3::One;
						if (currTask == 3) {
							instanceData.modelMatrix = Matrix::CreateTranslation(Vector3(float(x + 0.5f) * DEFAULT_BLOCKSIZE, float(y - 0.5f) * DEFAULT_BLOCKSIZE, 1.f * DEFAULT_BLOCKSIZE));
							m_instancedBlocks->addInstance(instanceData);
						} else if (currTask == 4) {
							instanceData.modelMatrix = Matrix::CreateTranslation(Vector3(float(x + 0.5f) * DEFAULT_BLOCKSIZE, float(y - 0.5f) * DEFAULT_BLOCKSIZE, 2.f * DEFAULT_BLOCKSIZE));
							m_instancedBlocks->addInstance(instanceData);
						}
					}
					x++;
				}

				y--;
				break;

			default:
				break;

			}
		}


		for (int x = 0; x < m_width; x++) {
			for (int y = 0; y < m_height; y++) {
				char& c = mapData[x][y];

				if (c == '1') {
					// Normal blocks
					DynBlockDeferredInstancedGeometryShader::InstanceData instanceData;
					instanceData.color = Vector3::One;

					// Logic to set the texture coordinate offset depending on what blocks are next to the current block
					// This removes the lines between blocks
					float row = 1;

					bool right = (x - 1 != -1) ? (mapData[x - 1][y] == '1') : false;
					bool left = (x + 1 != m_width) ? (mapData[x + 1][y] == '1') : false;
					bool up = (y + 1 != m_height) ? (mapData[x][y + 1] == '1') : false;
					bool down = (y - 1 != -1) ? (mapData[x][y - 1] == '1') : false;

					if (!left && !right && !up && !down) row = 1;
					if (!left && !right && up && !down) row = 2;
					if (!left && right && !up && !down) row = 3;
					if (!left && !right && !up && down) row = 4;
					if (left && !right && !up && !down) row = 5;
					if (!left && right && up && !down) row = 6;
					if (!left && right && !up && down) row = 7;
					if (left && !right && !up && down) row = 8;
					if (left && !right && up && !down) row = 9;
					if (!left && !right && up && down) row = 10;
					if (left && right && !up && !down) row = 11;
					if (!left && right && up && down) row = 12;
					if (left && right && !up && down) row = 13;
					if (left && !right && up && down) row = 14;
					if (left && right && up && !down) row = 15;
					if (left && right && up && down) row = 16;

					instanceData.blockVariationOffset = variationOffsetRowMul * row;
					instanceData.modelMatrix = Matrix::CreateTranslation(Vector3(float(x + 0.5f) * DEFAULT_BLOCKSIZE, float(y + 0.5f) * DEFAULT_BLOCKSIZE, 0.f));
					m_grid->addBlock(&m_instancedBlocks->addInstance(instanceData), x, y);
				} else if (c == 'c') {
					// Controlnodes
					m_grid->addControlpoint(static_cast<int>(x), static_cast<int>(y));
				} else if (c == 'h') {
					// Holes to hide in
					m_grid->addHole(static_cast<int>(x), static_cast<int>(y));
				} else if (c == 'p') {
					// Spawnpoints for players
					m_grid->addPlayerSpawnPoint(static_cast<int>(x), static_cast<int>(y));
				} else if (c == 'u') {
					// Spawnpoints for upgrades
					m_grid->addUpgradeSpawnPoint(static_cast<int>(x), static_cast<int>(y));
				}
			}
		}

		// Deallocate memory
		for (int i = 0; i < m_width; i++) {
			Memory::safeDeleteArr(mapData[i]);
		}
		Memory::safeDeleteArr(mapData);

	}
	else {
		Logger::Error("Could not load the file '" + filename + "'.");
	}
}

Level::~Level() {

}

void Level::update(const float delta, CharacterHandler* charHandler) {
}

void Level::draw() {
	// Draw instanced
	m_instancedBlocks->draw();
}

Grid* Level::getGrid() {
	return m_grid.get();
}

DirectX::SimpleMath::Vector2 Level::getGridWorldSize() {
	return DirectX::SimpleMath::Vector2((float)m_width, (float)m_height) * Level::DEFAULT_BLOCKSIZE;
}

const int & Level::getGridWidth() const {
	return m_width;
}

const int & Level::getGridHeight() const {
	return m_height;
}

