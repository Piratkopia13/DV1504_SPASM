#include "PayloadGamemode.h"

#include "../../CharacterHandler.h"

PayloadGamemode::PayloadGamemode(std::vector<Grid::Index>& indices)
: Gamemode(){
	m_controlNodeModel = Application::getInstance()->getResourceManager().getFBXModel("capture_point").getModel();
	m_indices = indices;
	m_numOfNodes = indices.size();
	m_radius = 2;

	for (Grid::Index index : indices) {
		float x = Level::DEFAULT_BLOCKSIZE * (index.x + 0.5f);
		float y = Level::DEFAULT_BLOCKSIZE * (index.y);
		m_controlNodes.push_back(std::make_unique<ControlNode>(m_controlNodeModel));
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

	// Checks the indices all around each player to see if it's within a controlpoints radius
	for (Grid::Index cn_index : m_indices) {
		for (unsigned int i = 0; i < charHandler->getNrOfPlayers(); i++) {

			bool playerColliding = false;
			std::vector<Grid::Index> indices = Grid::convertToIndexed(charHandler->getCharacter(i)->getBoundingBox());

			for (int x = cn_index.x - m_radius; x < cn_index.x + m_radius + 1; x++) 
				for (int y = cn_index.y; y < cn_index.y + m_radius + 1; y++) 
					for (Grid::Index p_index : indices) 
						if (p_index.x == x && p_index.y == y) 
							playerColliding = true;
				
			if (playerColliding) {
				if (charHandler->getCharacter(i)->getTeam() == 1)
					numOfTeam[index].x += 1;
				else
					numOfTeam[index].y += 1;
			}
		}

		m_controlNodes[index]->capture(numOfTeam[index].x, numOfTeam[index].y);
		if (m_controlNodes[index]->updateNodeTimer(dt)) {
			Gamemode::addScore(1, m_controlNodes[index]->getTeam());
		}
		index++;
	}
}

void PayloadGamemode::draw() {
	for (const auto& cn : m_controlNodes)
		cn->draw();
}