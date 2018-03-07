#include "ScoreVisualization.h"

using namespace DirectX::SimpleMath;

ScoreVisualization::ScoreVisualization(Level* level, Gamemode* currentGamemode) {
	m_currentGamemode = currentGamemode;
	m_currentLevel = level;
	m_numberOfTeams = currentGamemode->getScore().size();

	Application* app = Application::getInstance();
	m_blockModel = app->getResourceManager().getFBXModel("block").getModel();

	//Score pillars
	//float scale = 5.0f;
	//for (unsigned int i = 0; i < m_numberOfTeams; i++) {
	//	for (int j = 0; j < 10; j++) {
	//		m_scoreBlocks.push_back(std::make_unique<Block>(m_blockModel));
	//		m_scoreBlocks.back()->getTransform().setScale(scale);
	//		//Dynamic placing of blocks to fit level
	//		m_scoreBlocks.back()->setPosition(Vector3((float)i * (3.0f * scale) + m_currentLevel->getGridWidth() / 2.0f - 1.5f * scale * (m_numberOfTeams - 1), (float)j * scale + m_currentLevel->getGridHeight() / 2.0f - 6.0f * scale, 100.0f));
	//	}
	//}

	//Animated random blocks






	float scale = 3.0f;
	m_randomBlocksPerTeam = 20;
	m_randomBlocks.resize(m_numberOfTeams);
	m_targetPositions.resize(m_numberOfTeams);
	m_accelerations.resize(m_numberOfTeams);
	m_velocities.resize(m_numberOfTeams);
	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		for (unsigned int j = 0; j < m_randomBlocksPerTeam; j++) {
			m_randomBlocks[i].push_back(std::make_unique<Block>(m_blockModel));
			m_randomBlocks[i].back()->getTransform().setScale(scale);
			m_randomBlocks[i].back()->setColor(GameInfo::getInstance()->convertedGameSettings.teams[i].color * 0.7f);
			//Dynamic starting positions and target positions to fit level (not as in width and height of target window but as in placing the window in the middle of the level)
			m_randomBlocks[i].back()->setPosition(Vector3(m_currentLevel->getGridWidth() / 2.0f + Utils::rnd() * 110.0f - 55.0f, m_currentLevel->getGridHeight() / 2.0f + Utils::rnd() * 70.0f - 35.0f, 130.0f + Utils::rnd() * 50.0f - 25.0f));
			m_targetPositions[i].push_back(Vector3(m_currentLevel->getGridWidth() / 2.0f + Utils::rnd() * 110.0f - 55.0f, m_currentLevel->getGridHeight() / 2.0f + Utils::rnd() * 70.0f - 35.0f, 130.0f + Utils::rnd() * 50.0f - 25.0f));
			//Set accelerations and velocity to 0 as from the beginning
			m_accelerations[i].push_back(0.0f);
			m_velocities[i].push_back(Vector3(0.0f, 0.0f, 0.0f));
		}
	}
	m_animationClock = 0.0f;
}

ScoreVisualization::~ScoreVisualization() {

}

void ScoreVisualization::update(float dt) {
	GameInfo* info = GameInfo::getInstance();

	unsigned int currentGamemode = info->convertedGameSettings.gamemode;

	float targetScore = info->convertedGameSettings.scoreLimit;

	std::vector<float> finalScore = m_currentGamemode->getScore();

	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		finalScore[i] = (finalScore[i] / targetScore) * 10.0f;
	}

	//Color the pillars accoring to the current score of each team
	/*for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		for (int j = 0; j < 10; j++) {
			if (finalScore[i] - (float)j > 1) {
				m_scoreBlocks[i * 10 + j]->setColor(info->convertedGameSettings.teams[i].color * 0.5f);
			}
			else if (finalScore[i] - (float)j > 0) {
				m_scoreBlocks[i * 10 + j]->setColor(info->convertedGameSettings.teams[i].color * 0.5f * (finalScore[i] - (float)j));
			}
			else {
				m_scoreBlocks[i * 10 + j]->setColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}*/

	//Animate random blocks by giving them a target position and making them accelerate towards that position






	float scale = 3.0f;
	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		int pointBlocks = m_randomBlocksPerTeam * finalScore[i] * 0.1f;

		for (unsigned int j = 0; j < pointBlocks; j++) { //Blocks moving toward or are already in the score pillar (claimed points)
			Vector3 tempPos = m_randomBlocks[i][j]->getTransform().getTranslation();
			Vector3 tempRotations = m_randomBlocks[i][j]->getTransform().getRotations();
			m_targetPositions[i][j] = Vector3((float)i * (15.0f) + m_currentLevel->getGridWidth() / 2.0f - 7.5f * (m_numberOfTeams - 1) + (j % 2) * scale, floorf((float)j / 2.0f) * scale + m_currentLevel->getGridHeight() / 2.0f - 15.0f, 100.0f);
			
			if ((tempPos - m_targetPositions[i][j]).Length() < 0.5f) { //Block is in the right place in the score pillar
				m_accelerations[i][j] = 0.0f;
				m_velocities[i][j] = Vector3::Zero;
				m_randomBlocks[i][j]->getTransform().setRotations(Vector3(0.0f, 0.0f, 0.0f));
				m_randomBlocks[i][j]->getTransform().setTranslation(m_targetPositions[i][j]);
			}
			else { //Block is moving towards score pillar
				m_accelerations[i][j] = 10.0f;
				m_velocities[i][j] += (m_targetPositions[i][j] - tempPos) * m_accelerations[i][j] * dt;
				float m_tempVelocity = min(m_velocities[i][j].Length(), 15.0f);
				m_velocities[i][j].Normalize();
				m_velocities[i][j] *= m_tempVelocity;
				m_randomBlocks[i][j]->getTransform().setTranslation(tempPos + m_velocities[i][j] * dt);
				//Randomly rotate the blocks
				m_randomBlocks[i][j]->getTransform().rotateAroundX(Utils::rnd() * dt);
				m_randomBlocks[i][j]->getTransform().rotateAroundY(Utils::rnd() * dt);
				m_randomBlocks[i][j]->getTransform().rotateAroundZ(Utils::rnd() * dt);

				m_randomBlocks[i][j]->setColor(GameInfo::getInstance()->convertedGameSettings.teams[i].color);
			}
		}

		for (unsigned int j = pointBlocks; j < m_randomBlocks[i].size(); j++) { //Random blocks floating around (unclaimed points)
			Vector3 tempPos = m_randomBlocks[i][j]->getTransform().getTranslation();
			Vector3 tempRotations = m_randomBlocks[i][j]->getTransform().getRotations();
			m_accelerations[i][j] = Utils::rnd() * 0.2f;
			m_velocities[i][j] += (m_targetPositions[i][j] - tempPos) * m_accelerations[i][j] * dt;
			float m_tempVelocity = min(m_velocities[i][j].Length(), 7.0f);
			m_velocities[i][j].Normalize();
			m_velocities[i][j] *= m_tempVelocity;
			m_randomBlocks[i][j]->getTransform().setTranslation(tempPos + m_velocities[i][j] * dt);

			//Randomly rotate the blocks
			m_randomBlocks[i][j]->getTransform().rotateAroundX(Utils::rnd() * dt);
			m_randomBlocks[i][j]->getTransform().rotateAroundY(Utils::rnd() * dt);
			m_randomBlocks[i][j]->getTransform().rotateAroundZ(Utils::rnd() * dt);

			m_randomBlocks[i][j]->setColor(GameInfo::getInstance()->convertedGameSettings.teams[i].color * 0.7f);
		}
	}

	//Timer that keeps track of when a new target position should be given to the blocks
	if (m_animationClock > 4.0f) {
		for (unsigned int i = 0; i < m_numberOfTeams; i++) {
			int pointBlocks = m_randomBlocksPerTeam * finalScore[i] * 0.1f;
			for (unsigned int j = pointBlocks; j < m_targetPositions[i].size(); j++) {
				m_targetPositions[i][j] = Vector3(m_currentLevel->getGridWidth() / 2.0f + Utils::rnd() * 110.0f - 55.0f, m_currentLevel->getGridHeight() / 2.0f + Utils::rnd() * 70.0f - 35.0f, 130.0f + Utils::rnd() * 50.0f - 25.0f);
			}
		}
		m_animationClock = 0.0f;
	}

	m_animationClock += dt;
}

void ScoreVisualization::draw() {
	/*for (unsigned int i = 0; i < m_scoreBlocks.size(); i++) {
		m_scoreBlocks[i]->draw();
	}*/

	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		for (unsigned int j = 0; j < m_randomBlocks[i].size(); j++) {
			m_randomBlocks[i][j]->draw();
		}
	}
}