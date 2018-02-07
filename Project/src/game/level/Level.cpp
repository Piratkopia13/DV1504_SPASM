#include "Level.h"
#include "../objects/Block.h"
#include "Grid.h"
#include "../objects/common/Moveable.h"
#include "../gamemodes/payload/PayloadGamemode.h"

#include <string>
#include <fstream>

const float Level::DEFAULT_BLOCKSIZE = 1.0f;

Level::Level(const std::string& filename) 
	: m_grid(nullptr)
{
	std::ifstream infile(DEFAULT_LEVEL_LOCATION + filename);
	
	if (infile) {

		// ONLY FOR PAYLOADGAMEMODE - Controlnode Indices
		std::vector<Grid::Index> cnIndices;
		

		std::string line;
		unsigned int currTask = 0;
		unsigned int x = 0;
		unsigned int y = 0;

		while (infile >> line) {
			if (!line.compare("attributes")) {
				currTask = 1;
				infile >> line;
			}
			if (!line.compare("map")) {
				currTask = 2;
				infile >> line;
				m_grid = std::make_unique<Grid>(m_width, m_height);
			}


			switch (currTask) {
			
			case 0: // Load models
				m_models.push_back(std::make_unique<FbxModel>(line));
				m_models.back()->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());
				break;
			
			case 1:
				if (!line.compare(0, 6, "height")) {
					int find = line.find('=');
					m_height = std::stoi(line.substr(find + 1, line.length() - find));
					y = m_height;
				}
				if (!line.compare(0, 5, "width")) {
					int find = line.find('=');
					m_width = std::stoi(line.substr(find + 1, line.length() - find));
				}
				break;

			case 2: // Load map
				x = 0;
				for (auto c : line) {
					switch (c) {

					// Normal blocks
					case '1':
						m_blocks.push_back(std::make_unique<Block>(m_models.at(0)->getModel()));
						m_blocks.back()->getTransform().setTranslation(DirectX::SimpleMath::Vector3(float(x + 0.5f) * DEFAULT_BLOCKSIZE, float(y - 0.5f) * DEFAULT_BLOCKSIZE, 0.f));
						m_blocks.back()->getTransform().setScale(DEFAULT_SCALING);
						m_grid->addBlock(m_blocks.back().get(), x, y - 1);
						break;
					
					// Controlnodes
					case 'c':
						cnIndices.push_back(Grid::Index{ static_cast<int>(x), static_cast<int>(y - 1) });
					
					default:
						break;
					
					}
					x++;
				}

				y--;
				break;

			default:
				break;

			}
		}

		// SWITCH TO ENUM WHEN AVAILABLE
		if (m_currentGamemode == 0)
			m_gamemode = std::make_unique<PayloadGamemode>(cnIndices);

	}
	else {
		Logger::Error("Could not load the file '" + filename + "'.");
	}
}

Level::~Level() {

}

void Level::update(const float delta, CharacterHandler* charHandler) {
	if(m_gamemode)
		m_gamemode->update(charHandler, delta);
}

void Level::draw() {
	for (const auto& block : m_blocks) 
		block->draw();

	if (m_gamemode)
		m_gamemode->draw();
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

