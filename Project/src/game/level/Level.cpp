#include "Level.h"

#include <string>
#include <fstream>

Level::Level() {
	m_width = 10;
	m_height = 10;
}

Level::Level(const std::string& filename) {
	std::ifstream infile(DEFAULT_LEVEL_LOCATION + filename);
	
	if (infile) {
		std::string line;
		unsigned int currTask = 0;
		Object* object = nullptr;
		unsigned int x = 0;
		unsigned int y = 0;

		std::cout << "models" << std::endl;
		while (infile >> line) {
			if (!line.compare("map")) {
				std::cout << "blocks" << std::endl;
				currTask = 1;
				infile >> line;
			}

			switch (currTask) {
			
			case 0: // Load models
				std::cout << line << std::endl;
				m_models.push_back(FbxModel(line));
				break;
			
			case 1: // Load map
				x = 0;

				for (auto c : line) {
					std::cout << "x: " << x << " y: " << y << " id: " << c << std::endl;
					switch (c) {
					case '1':
						//object = new Block(m_models.at(0).getModel());
						//grid.add(object, pos)
						m_blocks.push_back(Block(m_models.at(0).getModel()));
						m_blocks.end()->getTransform().setTranslation(DirectX::SimpleMath::Vector3(x * DEFAULT_BLOCKSIZE, y * DEFAULT_BLOCKSIZE, 0.f));
						break;
					default:
						break;
					}
					x++;
				}

				y++;
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

void Level::render() {
	for (Block block : m_blocks) {
		block.draw();
	}
}