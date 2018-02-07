#include "PayloadGamemode.h"

#include "../../CharacterHandler.h"

PayloadGamemode::PayloadGamemode(std::vector<Grid::Index>& indices, DeferredRenderer& deferredRenderer)
: Gamemode(){
	m_controlNodeModel = std::make_unique<FbxModel>("capture_point.fbx");
	m_controlNodeModel->getModel()->buildBufferForShader(&deferredRenderer.getGeometryShader());
	m_indices = indices;
	m_numOfNodes = indices.size();
	m_radius = 2;

	for (Grid::Index index : indices) {
		float x = Level::DEFAULT_BLOCKSIZE * (index.x + 0.5f);
		float y = Level::DEFAULT_BLOCKSIZE * (index.y);
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

			for (int x = cn_index.x - m_radius; x < cn_index.x + m_radius + 1; x++) 
				for (int y = cn_index.y; y < cn_index.y + m_radius + 1; y++) 
					for (Grid::Index p_index : indices) 
						if (p_index.x == x && p_index.y == y) 
							playerColliding = true;
				
			if (playerColliding) {
				// UNCOMMENT WHEN SUPPORT FOR TEAMS HAS BEEN ADDED
				//if (charHandler->getCharacter(i)->getTeam() + 1 == 1)
				//	numOfTeam[index].x += 1;
				//else
				//	numOfTeam[index].y += 1;
				numOfTeam[index].x += (i + 1) % 2;
				numOfTeam[index].y += i % 2;
			}
		}

		if (index == 2) {
			//std::cout << m_controlNodes[index]->getAsString() << std::endl;
			//std::cout << numOfTeam[index].x << "   " << numOfTeam[index].y << std::endl;
		}

		m_controlNodes[index]->capture(numOfTeam[index].x, numOfTeam[index].y);
		if (m_controlNodes[index]->updateNodeTimer(dt)) {
			Gamemode::addScore(1, m_controlNodes[index]->getTeam());
			std::cout << "Score update from point " << index << " Team Red: " << Gamemode::getScore(1) << " Team Blue: " << Gamemode::getScore(2) << std::endl;
		}
		index++;
	}
}

void PayloadGamemode::draw() {
	for (const auto& cn : m_controlNodes)
		cn->draw();
}