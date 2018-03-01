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

Level::Level(const std::string& filename) 
	: m_grid(nullptr)
{
	m_instancedBlocks = std::make_unique<InstancedBlocks>();

	std::ifstream infile(DEFAULT_LEVEL_LOCATION + filename);

	if (infile) {		

		std::string line;
		unsigned int currTask = 0;
		unsigned int x = 0;
		unsigned int y = 0;

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
						if (c == '1') {
							// Normal blocks
							DeferredInstancedGeometryShader::InstanceData instanceData;
							instanceData.color = Vector4::One;
							instanceData.position = Vector3(float(x + 0.5f) * DEFAULT_BLOCKSIZE, float(y - 0.5f) * DEFAULT_BLOCKSIZE, 0.f);
							m_grid->addBlock(&m_instancedBlocks->addInstance(instanceData), x, y - 1);
						} else if (c == 'c') {
							// Controlnodes
							m_grid->addControlpoint(static_cast<int>(x), static_cast<int>(y - 1));
						} else if (c == 'h') {
							// Holes to hide in
							m_grid->addHole(static_cast<int>(x), static_cast<int>(y - 1));
						} else if (c == 'p') {
							// Spawnpoints for players
							m_grid->addPlayerSpawnPoint(static_cast<int>(x), static_cast<int>(y - 1));
						} else if (c == 'u') {
							// Spawnpoints for upgrades
							m_grid->addUpgradeSpawnPoint(static_cast<int>(x), static_cast<int>(y - 1));
						}
					} else if (c == '1') {
						DeferredInstancedGeometryShader::InstanceData instanceData;
						instanceData.color = Vector4::One;
						if (currTask == 3) {
							instanceData.position = Vector3(float(x + 0.5f) * DEFAULT_BLOCKSIZE, float(y - 0.5f) * DEFAULT_BLOCKSIZE, 1.f * DEFAULT_BLOCKSIZE);
							m_instancedBlocks->addInstance(instanceData);
						} else if (currTask == 4) {
							instanceData.position = Vector3(float(x + 0.5f) * DEFAULT_BLOCKSIZE, float(y - 0.5f) * DEFAULT_BLOCKSIZE, 2.f * DEFAULT_BLOCKSIZE);
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

