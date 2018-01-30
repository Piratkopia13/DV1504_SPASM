#include "Level.h"
#include "../objects/Block.h"
#include "Grid.h"
#include "../objects/common/Moveable.h"

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

DirectX::SimpleMath::Vector3 Level::collisionTest(Moveable& moveable, const float dt) {
	DirectX::SimpleMath::Vector3 toMove(0.f, 0.f, 0.f);

	if (moveable.getVelocity().Length()) {
		int indexX = 0;
		int indexY = 0;

		AABB tempBB(*moveable.getBoundingBox());

		float EPS = 0.01f;
		DirectX::SimpleMath::Vector3 mMin = tempBB.getMinPos();
		DirectX::SimpleMath::Vector3 mMax = tempBB.getMaxPos();
		// TESTING
		float tempFix = mMax.x - mMin.x;
		mMax.x += tempFix;
		mMin.x += tempFix;
		tempBB.setMinPos(mMin);
		tempBB.setMaxPos(mMax);
		// TESTING
		DirectX::SimpleMath::Vector3 mVel = moveable.getVelocity() * dt;

		std::cout << "P2 Moveable MinX: " << mMin.x <<
			" Moveable MinY: " << mMin.y <<
			" Moveable MaxX: " << mMax.x <<
			" Moveable MaxY: " << mMax.y << std::endl;

		std::vector<Grid::Index> indices = m_grid->getCollisionIndices(tempBB, DEFAULT_BLOCKSIZE);

		if (indices.size() > 0) {
			for (Grid::Index index : indices) {
				float bMinX = index.x * DEFAULT_BLOCKSIZE;
				float bMaxX = (index.x + 1) * DEFAULT_BLOCKSIZE;
				float bMinY = index.y * DEFAULT_BLOCKSIZE;
				float bMaxY = (index.y + 1) * DEFAULT_BLOCKSIZE;


				if (mMax.x + mVel.x > bMinX && mMin.x + mVel.x < bMaxX &&
					mMax.y > bMinY && mMin.y < bMaxY) {
					if (mVel.x < 0)
						toMove.x = bMaxX - (mMin.x + mVel.x) + EPS;
					else if (mVel.x > 0)
						toMove.x = bMinX - (mMax.x + mVel.x) - EPS;
				}

				if (mMax.y + mVel.y > bMinY && mMin.y + mVel.y < bMaxY &&
					mMax.x > bMinX && mMin.x < bMaxX) {
					if (mVel.y < 0)
						toMove.y = bMaxY - (mMin.y + mVel.y) + EPS;
					else if (mVel.y > 0)
						toMove.y = bMinY - (mMax.y + mVel.y) - EPS;
				}

				if (toMove.x) {
					if (abs(toMove.x) <= EPS) toMove.x = 0.f;
					DirectX::SimpleMath::Vector3 tempVelocity = moveable.getVelocity();
					moveable.setVelocity(DirectX::SimpleMath::Vector3(toMove.x, 0.f, 0.f));
					moveable.move(1.0f);
					tempVelocity.x = 0.f;
					moveable.setVelocity(tempVelocity);
				}

				if (toMove.y) {
					if (abs(toMove.y) <= EPS) toMove.y = 0.f;
					DirectX::SimpleMath::Vector3 tempVelocity = moveable.getVelocity();
					moveable.setVelocity(DirectX::SimpleMath::Vector3(0.f, toMove.y, 0.f));
					moveable.move(1.0f);
					tempVelocity.y = 0.f;
					moveable.setVelocity(tempVelocity);
				}
			}
		}
	}

	return toMove;
}

DirectX::SimpleMath::Vector3 Level::tempCollisionTest(DirectX::SimpleMath::Vector3& pos, DirectX::SimpleMath::Vector3& vel, DirectX::SimpleMath::Vector2& boundaries) {
	int indexX = 0;
	int indexY = 0;


	float mMinX = pos.x;
	float mMaxX = pos.x + boundaries.x;
	float mMinY = pos.y;
	float mMaxY = pos.y + boundaries.y;
	float velX = vel.x;
	float velY = vel.y;
	
	AABB tempBB(DirectX::SimpleMath::Vector3(mMinX, mMinY, 0.0f), DirectX::SimpleMath::Vector3(mMaxX, mMaxY, 0.0f));

	float EPS = 0.1f;
	std::vector<Grid::Index> indices = m_grid->getCollisionIndices(tempBB, DEFAULT_BLOCKSIZE);
	DirectX::SimpleMath::Vector3 toMove(0.f, 0.f, 0.f);

	if (indices.size() > 0) {
		for (Grid::Index index : indices) {
			float bMinX = index.x * DEFAULT_BLOCKSIZE;
			float bMaxX = (index.x + 1) * DEFAULT_BLOCKSIZE;
			float bMinY = index.y * DEFAULT_BLOCKSIZE;
			float bMaxY = (index.y + 1) * DEFAULT_BLOCKSIZE;


			if (mMaxX + velX > bMinX && mMinX + velX < bMaxX &&
				mMaxY > bMinY && mMinY < bMaxY) {

				if (vel.x < 0)
					toMove.x = bMaxX - (mMinX + velX) + EPS;
				else if (vel.x > 0)
					toMove.x = bMinX - (mMaxX + velX) - EPS;
			}

			if (mMaxY + velY > bMinY && mMinY + velY < bMaxY &&
				mMaxX > bMinX && mMinX < bMaxX) {

				if (vel.y < 0)
					toMove.y = bMaxY - (mMinY + velY) + EPS;
				else if (vel.y > 0)
					toMove.y = bMinY - (mMaxY + velY) - EPS;
			}

			if (toMove.x || toMove.y) {
				if (abs(toMove.x) <= EPS) toMove.x = 0.f;
				if (abs(toMove.y) <= EPS) toMove.y = 0.f;
			}

			//std::cout << index.x << "   " << index.y << std::endl;
		}
		//std::cout << "_______________________________" << std::endl;
	}

	return toMove;
}