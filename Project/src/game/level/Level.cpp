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
				m_grid = std::make_unique<Grid>(m_width, m_height);
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
					y = m_height - 1;
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
						m_blocks.back()->getTransform().setTranslation(DirectX::SimpleMath::Vector3((x) * DEFAULT_BLOCKSIZE, (y) * DEFAULT_BLOCKSIZE, 0.f));
						std::cout << "Block position X: " << x * DEFAULT_BLOCKSIZE << " Y: " << y * DEFAULT_BLOCKSIZE << std::endl;
						m_blocks.back()->getTransform().setScale(DEFAULT_SCALING);
						m_grid->addBlock(m_blocks.back().get(), x, y);
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

DirectX::SimpleMath::Vector3 Level::tempCollisionTest(DirectX::SimpleMath::Vector3 pos) {
	int indexX = 0;
	int indexY = 0;

	float mMinX = pos.x - 0.0005f;
	float mMaxX = pos.x + 0.0005f;
	float mMinY = pos.y - 0.0005f;
	float mMaxY = pos.y + 0.0005f;

	float EPS = 0.5f;
	float bMinX = indexX * DEFAULT_BLOCKSIZE;
	float bMaxX = (indexX + 1) * DEFAULT_BLOCKSIZE;
	float bMinY = indexY * DEFAULT_BLOCKSIZE;
	float bMaxY = (indexY + 1) * DEFAULT_BLOCKSIZE;

	float velX = 1.f;
	float velY = 1.f;

	DirectX::SimpleMath::Vector3 toMove;
	toMove.x = 0.f;
	toMove.y = 0.f;
	toMove.z = 0.f;

	if (mMaxX + velX > bMinX &&
		mMinX + velX < bMinX &&
		mMaxY > bMinY &&
		mMinY < bMaxY) {
		toMove.x = bMinX - (mMaxX + velX);
	}
	else if (mMinX - velX < bMaxX &&
			mMaxX - velX > bMaxX &&
			mMaxY > bMinY &&
			mMinY < bMaxY) {
		toMove.x = bMaxX - (mMinX - velX);
	}

	if (mMaxY + velY > bMinY &&
		mMinY + velY < bMinY &&
		mMaxX > bMinX &&
		mMinX < bMaxX) {
		toMove.y = bMinY - (mMaxY + velY);
	}
	else if (mMinY - velY < bMaxY &&
		mMaxY - velY > bMaxY &&
		mMaxX > bMinX &&
		mMinX < bMaxX) {
		toMove.y = bMaxY - (mMinY - velY);
	}

	return toMove;
}