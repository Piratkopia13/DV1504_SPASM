#include "PayloadGamemode.h"

#include "../../CharacterHandler.h"
#include "../../objects/Block.h"

PayloadGamemode::PayloadGamemode(std::vector<Grid::Index>& indices, std::vector<std::vector<Block*>> & blocks, const int levelWidth, const int levelHeight)
: Gamemode()
, m_blocks(blocks) 
{
	Model* controlpointModel = Application::getInstance()->getResourceManager().getFBXModel("capture_point").getModel();
	m_indices = indices;
	m_numOfNodes = indices.size();
	m_radius = 2;
	m_teamWin = 0;
	m_scoreToWin = 100.f;
	m_levelWidth = levelWidth;
	m_levelHeight = levelHeight;

	m_teamOneColor = Application::getInstance()->getGameSettings().teamOneColor;
	m_teamTwoColor = Application::getInstance()->getGameSettings().teamTwoColor;

	// Add default scores
	addScore(50, 1);
	addScore(50, 2);

	Gamemode::setGametime(60000000000000000.f);

	for (Grid::Index index : indices) {
		float x = Level::DEFAULT_BLOCKSIZE * (index.x + 0.5f);
		float y = Level::DEFAULT_BLOCKSIZE * (index.y);
		m_controlNodes.push_back(std::make_unique<ControlNode>(controlpointModel));
		m_controlNodes.back()->getTransform().setTranslation(DirectX::SimpleMath::Vector3(x, y, 0.f));
	}
}

PayloadGamemode::~PayloadGamemode() {}

void PayloadGamemode::update(CharacterHandler* charHandler, float dt) {
	int index = 0;

	Gamemode::update(nullptr, dt);

	float redScore = Gamemode::getScore(1);
	float blueScore = Gamemode::getScore(2);

	float totScore = redScore + blueScore;
	
	float teamOneBlocks = (redScore / totScore) * float(m_levelWidth);
	float teamTwoBlocks = (blueScore / totScore) * float(m_levelWidth);

	DirectX::SimpleMath::Vector4 blockColor;

	for (int x = 0; x < m_levelWidth; x++) {
		blockColor.x = min(m_teamOneColor.x * 1.2f, max(0.f, (teamOneBlocks - float(x))) * m_teamOneColor.x);
		blockColor.y = min(m_teamOneColor.y * 1.2f, max(0.f, (teamOneBlocks - float(x))) * m_teamOneColor.y);
		blockColor.z = min(m_teamOneColor.z * 1.2f, max(0.f, (teamOneBlocks - float(x))) * m_teamOneColor.z);

		blockColor.x += min(m_teamTwoColor.x * 1.2f, max(0.f, (teamTwoBlocks - float((m_levelWidth) - x)) * m_teamTwoColor.x));
		blockColor.y += min(m_teamTwoColor.y * 1.2f, max(0.f, (teamTwoBlocks - float((m_levelWidth) - x)) * m_teamTwoColor.y));
		blockColor.z += min(m_teamTwoColor.z * 1.2f, max(0.f, (teamTwoBlocks - float((m_levelWidth) - x)) * m_teamTwoColor.z));

		for (int y = 0; y < m_levelHeight; y++) {
			if (m_blocks[x][y]) {
				m_blocks[x][y]->setColor(DirectX::SimpleMath::Vector4(
					blockColor.x,
					blockColor.y,
					blockColor.z,
					1.f));
			}
		}
	}


	
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
		//Logger::log(m_controlNodes[0]->getAsString());
		// Points per x seconds
		//if (m_controlNodes[index]->updateNodeTimer(dt)) {
		//	Gamemode::addScore(1, m_controlNodes[index]->getTeam());
		//}
		// Points every update
		m_controlNodes[index]->updateNodeTimer(dt);
		int owningTeam = m_controlNodes[index]->getTeam();
		if (owningTeam != 0) {
			Gamemode::addScore(dt, owningTeam);
			Gamemode::addScore(-dt, (owningTeam == 1) ? 2 : 1);
		}
		index++;
	}
}

void PayloadGamemode::draw() {
	for (const auto& cn : m_controlNodes)
		cn->draw();
}

void PayloadGamemode::setTeamColor(const int team, const DirectX::SimpleMath::Vector4 & color) {
	for (const auto& cn : m_controlNodes)
		cn->setTeamColor(team, color);
}

int PayloadGamemode::checkWin() {
	float redScore = Gamemode::getScore(1);
	float blueScore = Gamemode::getScore(2);

	m_teamWin = 0;
	if (redScore > m_scoreToWin)
		m_teamWin = 1;
	if (blueScore > m_scoreToWin)
		m_teamWin = 2;
	if (m_teamWin != 0 && blueScore == redScore)
		m_teamWin = -1;

	/*if (Gamemode::getGametime() <= 0.f) {
		if (redScore > blueScore) {
			m_teamWin = 1;
		}
		else if (blueScore > redScore) {
			m_teamWin = 2;
		}
		else {
			m_teamWin = -1;
		}
	}*/

	return m_teamWin;
}