#include "Level.h"
#include "../objects/Block.h"
#include "Grid.h"
#include "../objects/common/Moveable.h"

#include <string>
#include <fstream>

const float Level::DEFAULT_BLOCKSIZE = 1.0f;

Level::Level(const std::string& filename, DeferredRenderer& deferredRenderer) 
	: m_grid(nullptr)
{
	std::ifstream infile(DEFAULT_LEVEL_LOCATION + filename);
	
	if (infile) {
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
				m_grid = std::make_unique<Grid>(m_width, m_height, (int) DEFAULT_BLOCKSIZE);
			}


			switch (currTask) {
			
			case 0: // Load models
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
					switch (c) {
					case '1':
						m_blocks.push_back(std::make_unique<Block>(m_models.at(0)->getModel()));
						m_blocks.back()->getTransform().setTranslation(DirectX::SimpleMath::Vector3(float(x + 0.5f) * DEFAULT_BLOCKSIZE, float(y - 0.5f) * DEFAULT_BLOCKSIZE, 0.f));
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

	moveable.setGrounded(false);

	AABB tempBB(*moveable.getBoundingBox());

	float EPS = 0.01f;
	DirectX::SimpleMath::Vector3 mMin = tempBB.getMinPos();
	DirectX::SimpleMath::Vector3 mMax = tempBB.getMaxPos();
	DirectX::SimpleMath::Vector3 mVel = moveable.getVelocity() * dt;

	std::vector<Grid::Index> indices = m_grid->getCollisionIndices(tempBB, DEFAULT_BLOCKSIZE);

	if (indices.size() > 0) {
		bool colX = false;
		bool colY = false;
		for (Grid::Index index : indices) {
			float bMinX = index.x * DEFAULT_BLOCKSIZE;
			float bMaxX = (index.x + 1) * DEFAULT_BLOCKSIZE;
			float bMinY = index.y * DEFAULT_BLOCKSIZE;
			float bMaxY = (index.y + 1) * DEFAULT_BLOCKSIZE;

			if (mMax.x + mVel.x > bMinX && mMin.x + mVel.x < bMaxX &&
				mMax.y > bMinY && mMin.y < bMaxY) {
				if (mVel.x < 0.f)
					toMove.x = bMaxX - mMin.x + EPS;
				else if (mVel.x > 0.f)
					toMove.x = bMinX - mMax.x - EPS;
			}

			if (mMax.y + mVel.y + EPS > bMinY && mMin.y + mVel.y - EPS < bMaxY &&
				mMax.x > bMinX && mMin.x < bMaxX) {
				if (mVel.y < 0.f) {
					toMove.y = bMaxY - mMin.y + EPS;
				}
				else if (mVel.y > 0.f)
					toMove.y = bMinY - mMax.y - EPS;
			}

			if (toMove.x && !(colX)) {
				colX = true;
				if (abs(toMove.x) <= EPS) toMove.x = 0.f;
				DirectX::SimpleMath::Vector3 tempVel = moveable.getVelocity();
				tempVel.x = 0.f;
				moveable.setVelocity(tempVel);
				toMove.y = 0.f;
				moveable.move(toMove);
			}

			if (toMove.y && !(colY)) {
				colY = true;
				if (abs(toMove.y) <= EPS) toMove.y = 0.f;
				DirectX::SimpleMath::Vector3 tempVel = moveable.getVelocity();
				tempVel.y = 0.f;
				moveable.setVelocity(tempVel);
				toMove.x = 0.f;
				moveable.move(toMove);
			}
		}

		if (!(colX || colY)) {
			for (Grid::Index index : indices) {
				float bMinX = index.x * DEFAULT_BLOCKSIZE;
				float bMaxX = (index.x + 1) * DEFAULT_BLOCKSIZE;
				float bMinY = index.y * DEFAULT_BLOCKSIZE;
				float bMaxY = (index.y + 1) * DEFAULT_BLOCKSIZE;

				if (mMax.x + mVel.x > bMinX && mMin.x + mVel.x < bMaxX &&
					mMax.y + mVel.y > bMinY && mMin.y + mVel.y < bMaxY) {
					if (mVel.x < 0.f)
						toMove.x = bMaxX - mMin.x + EPS;
					else if (mVel.x > 0.f)
						toMove.x = bMinX - mMax.x - EPS;

					if (mVel.y < 0.f)
						toMove.y = bMaxY - mMin.y + EPS;
					else if (mVel.y > 0.f)
						toMove.y = bMinY - mMax.y - EPS;
				}

				if (abs(toMove.x) < abs(toMove.y)) {
					if (abs(toMove.x) <= EPS) toMove.x = 0.f;
					DirectX::SimpleMath::Vector3 tempVel = moveable.getVelocity();
					tempVel.x = 0.f;
					moveable.setVelocity(tempVel);
					toMove.y = 0.f;
					moveable.move(toMove);
				}

				if (abs(toMove.y) < abs(toMove.x)) {
					if (abs(toMove.y) <= EPS) toMove.y = 0.f;
					DirectX::SimpleMath::Vector3 tempVel = moveable.getVelocity();
					tempVel.y = 0.f;
					moveable.setVelocity(tempVel);
					toMove.x = 0.f;
					moveable.move(toMove);
				}
			}
		}

		for (Grid::Index index : indices) {
			float bMinX = index.x * DEFAULT_BLOCKSIZE;
			float bMaxX = (index.x + 1) * DEFAULT_BLOCKSIZE;
			float bMinY = index.y * DEFAULT_BLOCKSIZE;
			float bMaxY = (index.y + 1) * DEFAULT_BLOCKSIZE;

			if (mMax.y - EPS * 2.0 > bMinY && mMin.y - EPS * 2.0 < bMaxY &&
				mMax.x > bMinX && mMin.x < bMaxX)
				moveable.setGrounded(true);
		}
	}

	return toMove;
}

Grid* Level::getGrid() {
	return m_grid.get();
}

DirectX::SimpleMath::Vector2 Level::getGridWorldSize() {
	auto& blocks = getGrid()->getAllBlocks();
	return DirectX::SimpleMath::Vector2((float)blocks.size(), (float)blocks.at(0).size()) * Level::DEFAULT_BLOCKSIZE;
}
