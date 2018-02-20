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

	m_currentActivePoint = static_cast<int>(floor(Utils::rnd() * m_controlNodes.size()));
	replacePoint();
}

PayloadGamemode::~PayloadGamemode() {}

void PayloadGamemode::update(CharacterHandler* charHandler, float dt) {

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


	
	Grid::Index numOfTeam = { 0, 0 };

	Grid::Index cnIndex = m_indices[m_currentActivePoint];
	for (unsigned int i = 0; i < charHandler->getNrOfPlayers(); i++) {
		bool playerColliding = false;
		std::vector<Grid::Index> indices = Grid::convertToIndexed(charHandler->getCharacter(i)->getBoundingBox());

		for (int x = cnIndex.x - m_radius; x < cnIndex.x + m_radius + 1; x++)
			for (int y = cnIndex.y; y < cnIndex.y + m_radius + 1; y++)
				for (Grid::Index p_index : indices)
					if (p_index.x == x && p_index.y == y)
						playerColliding = true;

		if (playerColliding) {
			if (charHandler->getCharacter(i)->getTeam() == 1)
				numOfTeam.x += 1;
			else
				numOfTeam.y += 1;
		}
	}

	m_controlNodes[m_currentActivePoint]->capture(numOfTeam.x, numOfTeam.y);

	// Points every update
	if (m_controlNodes[m_currentActivePoint]->updateNodeTimer(dt)) {
		replacePoint();
	}

	int owningTeam = m_controlNodes[m_currentActivePoint]->getTeam();
	if (owningTeam != 0) {
		Gamemode::addScore(dt, owningTeam);
		Gamemode::addScore(-dt, (owningTeam == 1) ? 2 : 1);
	}
}

void PayloadGamemode::draw() {
	//for (const auto& cn : m_controlNodes)
	//	cn->draw();
	m_controlNodes[m_currentActivePoint]->draw();
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

	return m_teamWin;
}

void PayloadGamemode::replacePoint() {
	int newIndex = static_cast<int>(floor(Utils::rnd() * m_controlNodes.size()));
	while (newIndex == m_currentActivePoint) {
		newIndex = static_cast<int>(floor(Utils::rnd() * m_controlNodes.size()));
	}

	m_currentActivePoint = newIndex;
}