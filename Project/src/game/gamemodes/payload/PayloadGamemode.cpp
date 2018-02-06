#include "PayloadGamemode.h"


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

void PayloadGamemode::update(Character* player, float dt) {
	int index = 0;
	
	std::vector<Grid::Index> numOfTeam;
	numOfTeam.resize(m_numOfNodes);

	for (int i = 0; i < m_numOfNodes; i++) {
		numOfTeam[i].x = 0;
		numOfTeam[i].y = 0;
	}

	for (Grid::Index cn_index : m_indices) {
		bool playerColliding = false;
		std::vector<Grid::Index> indices = Grid::convertToIndexed(player->getBoundingBox());
		for (Grid::Index p_index : indices) {
			if (p_index.x == cn_index.x && p_index.y == cn_index.y) {
				playerColliding = true;
			}
		}
		// REPLACE WITH TEAM OF PLAYER LATER
		if (playerColliding) {
			//player.getTeam()
			numOfTeam[index].x += 1;
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