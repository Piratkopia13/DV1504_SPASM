#include "PayloadGamemode.h"

#include "../../CharacterHandler.h"

PayloadGamemode::PayloadGamemode(std::vector<Grid::Index> indices, DeferredRenderer& deferredRenderer)
: Gamemode(){
	m_controlNodeModel = std::make_unique<FbxModel>("block.fbx");
	m_controlNodeModel->getModel()->buildBufferForShader(&deferredRenderer.getGeometryShader());
	m_indices = indices;
	m_numOfNodes = indices.size();

	for (Grid::Index index : indices) {
		float x = Level::DEFAULT_BLOCKSIZE * (index.x + 0.5f);
		float y = Level::DEFAULT_BLOCKSIZE * (index.y - 0.5f);
		m_controlNodes.push_back(std::make_unique<ControlNode>(m_controlNodeModel->getModel()));
		m_controlNodes.back()->getTransform().setTranslation(DirectX::SimpleMath::Vector3(x, y, 0.f));
	}
}

PayloadGamemode::~PayloadGamemode() {}

void PayloadGamemode::update(CharacterHandler* charHandler, float dt) {
	int index = 0;
	
	std::vector<Grid::Index> numOfTeam;
	numOfTeam.resize(m_numOfNodes);

	for (int i = 0; i < m_numOfNodes; i++) {
		numOfTeam[i].x = 0;
		numOfTeam[i].y = 0;
	}

	for (Grid::Index cn_index : m_indices) {
		for (int i = 0; i < charHandler->getNrOfPlayers(); i++) {
			bool playerColliding = false;
			std::vector<Grid::Index> indices = Grid::convertToIndexed(charHandler->getCharacter(i)->getBoundingBox());
			for (Grid::Index p_index : indices) {
				if (p_index.x == cn_index.x && p_index.y == cn_index.y) {
					playerColliding = true;
				}
			}

			if (playerColliding) {
				/* UNCOMMENT WHEN SUPPORT FOR TEAMS HAS BEEN ADDED */
				//if (charHandler->getCharacter(i)->getTeam() + 1 == 1)
				//	numOfTeam[index].x += 1;
				//else
				//	numOfTeam[index].y += 1;
				numOfTeam[index].x += (i + 1) % 2;
				numOfTeam[index].y += i % 2;
			}
		}

		m_controlNodes[index]->capture(numOfTeam[index].x, numOfTeam[index].y);
		m_controlNodes[index]->updateNodeTimer(dt);
		index++;
	}
}

void PayloadGamemode::draw() {
	for (const auto& cn : m_controlNodes)
		cn->draw();
}