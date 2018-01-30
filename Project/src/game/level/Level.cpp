#include "Level.h"
#include "../objects/Block.h"
#include "Grid.h"

#include <string>
#include <fstream>

Level::Level(const std::string& filename, DeferredRenderer& deferredRenderer) {
	std::ifstream infile(DEFAULT_LEVEL_LOCATION + filename);
	
	if (infile) {
		std::string line;
		unsigned int currTask = 0;
		unsigned int x = 0;
		unsigned int y = 0;

		std::cout << "models" << std::endl;
		while (infile >> line) {
			if (!line.compare("attributes")) {
				currTask = 1;
				infile >> line;
			}
			if (!line.compare("map")) {
				currTask = 2;
				infile >> line;
				m_grid = std::make_unique<Grid>(m_width, m_height, DEFAULT_BLOCKSIZE);
			}


			switch (currTask) {
			
			case 0: // Load models
				std::cout << line << std::endl;
				m_models.push_back(std::make_unique<FbxModel>(line));
				m_models.back()->getModel()->buildBufferForShader(&deferredRenderer.getGeometryShader());
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
					std::cout << "x: " << x << " y: " << y << " id: " << c << std::endl;
					switch (c) {
					case '1':
						m_blocks.push_back(std::make_unique<Block>(m_models.at(0)->getModel()));
						m_blocks.back()->getTransform().setTranslation(DirectX::SimpleMath::Vector3(x * DEFAULT_BLOCKSIZE, y * DEFAULT_BLOCKSIZE, 0.f));
						m_blocks.back()->getTransform().setScale(DEFAULT_SCALING);
						m_grid->addBlock(m_blocks.back().get(), x, y - 1);
						break;
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
	}
	else {
		std::cout << "Failed to load level file." << std::endl;
	}
}

Level::~Level() {

}

void Level::update(const float delta) {

}

void Level::draw() {
	for (const auto& block : m_blocks) 
		block->draw();
}

std::vector<std::unique_ptr<Block>>& Level::getBlockList() {
	return m_blocks;
}
