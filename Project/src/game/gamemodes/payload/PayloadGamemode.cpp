#include "PayloadGamemode.h"

#include "../../CharacterHandler.h"
#include "../../objects/Block.h"

using namespace DirectX::SimpleMath;

PayloadGamemode::PayloadGamemode(std::vector<Grid::Index>& indices, std::vector<std::vector<Grid::BlockInstance*>>& blocks, const int levelWidth, const int levelHeight)
: Gamemode()
, m_blocks(blocks) 
{
	Model* controlpointModel = Application::getInstance()->getResourceManager().getFBXModel("capture_point").getModel();
	m_indices = indices;
	m_numOfNodes = indices.size();
	m_radius = 2;
	m_teamWin = -1;
	m_scoreToWin = 100.f;
	m_levelWidth = levelWidth;
	m_levelHeight = levelHeight;

	GameInfo * info = GameInfo::getInstance();
	
	m_teamOneColor = info->getDefaultColor(info->gameSettings.teams[0].color, 0);
	m_teamTwoColor = info->getDefaultColor(info->gameSettings.teams[1].color, 0);

	// Add default scores
	addScore(50, 0);
	addScore(50, 1);

	Gamemode::setGametime(60000000000000000.f);

	for (Grid::Index index : indices) {
		float x = Level::DEFAULT_BLOCKSIZE * (index.x + 0.5f);
		float y = Level::DEFAULT_BLOCKSIZE * (index.y);
		m_controlNodes.push_back(std::make_unique<ControlNode>(controlpointModel));
		m_controlNodes.back()->getTransform().setTranslation(Vector3(x, y, 0.f));
	}


	setTeamColor(0, info->getDefaultColor(info->gameSettings.teams[0].color, 0));
	setTeamColor(1, info->getDefaultColor(info->gameSettings.teams[1].color, 0));

	m_currentActivePoint = static_cast<int>(floor(Utils::rnd() * m_controlNodes.size()));
	replacePoint();

	m_emitterPos = m_controlNodes[m_currentActivePoint]->getTransform().getTranslation();

	m_pointEmitter = std::shared_ptr<ParticleEmitter>(new ParticleEmitter(ParticleEmitter::EXPLOSION, m_emitterPos,
		Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f), 400.f, 800, 0.1f, 1.5f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.f, 0U, true));

	m_pointEmitter2 = std::shared_ptr<ParticleEmitter>(new ParticleEmitter(ParticleEmitter::EXPLOSION, m_emitterPos,
		Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f), 400.f, 800, 0.1f, 1.5f, Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.f, 0U, true));


	m_emitterRotation = 0.0f;
}

PayloadGamemode::~PayloadGamemode() {}

void PayloadGamemode::update(CharacterHandler* charHandler, float dt) {

	Gamemode::update(nullptr, dt);

	float redScore = Gamemode::getScore(0);
	float blueScore = Gamemode::getScore(1);

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
				m_blocks[x][y]->color = DirectX::SimpleMath::Vector4(
					blockColor.x,
					blockColor.y,
					blockColor.z,
					1.f);
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
			if (charHandler->getCharacter(i)->getTeam() == 0)
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
	if (owningTeam != -1) {
		Gamemode::addScore(dt, owningTeam);
		Gamemode::addScore(-dt, (owningTeam == 0) ? 1 : 0);
	}


	Vector3 newEmitterPos = m_controlNodes[m_currentActivePoint]->getTransform().getTranslation();
	Vector3 distanceVector = m_emitterPos - m_controlNodes[m_currentActivePoint]->getTransform().getTranslation();
	float tempLength = distanceVector.Length();
	distanceVector.Normalize();
	if (tempLength > 0.5f) {
		newEmitterPos = m_controlNodes[m_currentActivePoint]->getTransform().getTranslation() + distanceVector * (tempLength - 20.0f * dt);
	}

	m_emitterRotation += 6.28f * dt;
	m_emitterRotation = fmod(m_emitterRotation, 6.28f);
	Matrix rotationMatrix = Matrix::CreateRotationY(m_emitterRotation);

	m_emitterPos = newEmitterPos;

	m_pointEmitter->updateEmitPosition(m_emitterPos
		+ Vector3(XMVector4Transform(Vector4((float)m_radius, 0.0f, 0.0f, 1.0f), rotationMatrix)) + Vector3(0.0f, m_controlNodes[m_currentActivePoint]->getCapturePercentage() * 2.0f + 0.2f, 0.0f));

	m_pointEmitter2->updateEmitPosition(m_emitterPos
		+ Vector3(XMVector4Transform(Vector4((float)-m_radius, 0.0f, 0.0f, 1.0f), rotationMatrix)) + Vector3(0.0f, m_controlNodes[m_currentActivePoint]->getCapturePercentage() * 2.0f + 0.2f, 0.0f));

	Vector4 tempColor(1.0f, 1.0f, 1.0f, 1.0f);
	int tempTeam = m_controlNodes[m_currentActivePoint]->getTeam();
	if (tempTeam == 0) {
		tempColor = m_teamOneColor;
	}
	else if (tempTeam == 1) {
		tempColor = m_teamTwoColor;
	}
	m_pointEmitter->updateColor(tempColor);
	m_pointEmitter2->updateColor(tempColor);
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

void PayloadGamemode::setParticleHandler(ParticleHandler* newParticleHandler) {
	m_particleHandler = newParticleHandler;
	m_particleHandler->addEmitter(m_pointEmitter);
	m_particleHandler->addEmitter(m_pointEmitter2);
}

int PayloadGamemode::checkWin() {
	float redScore = Gamemode::getScore(0);
	float blueScore = Gamemode::getScore(1);

	m_teamWin = -1;
	if (redScore > m_scoreToWin)
		m_teamWin = 0;
	if (blueScore > m_scoreToWin)
		m_teamWin = 1;
	if (m_teamWin != -1 && blueScore == redScore)
		m_teamWin = -1;

	return m_teamWin;
}

void PayloadGamemode::replacePoint() {
	if (m_controlNodes.size() > 1) {
		int newIndex = static_cast<int>(floor(Utils::rnd() * m_controlNodes.size()));
		while (newIndex == m_currentActivePoint) {
			newIndex = static_cast<int>(floor(Utils::rnd() * m_controlNodes.size()));
		}

		m_currentActivePoint = newIndex;
	}
}