#include "ScoreVisualization.h"

using namespace DirectX::SimpleMath;

ScoreVisualization::ScoreVisualization(Level* level, Gamemode* currentGamemode) {
	m_currentGamemode = currentGamemode;
	m_currentLevel = level;
	m_numberOfTeams = currentGamemode->getScore().size();

	Application* app = Application::getInstance();
	m_blockModel = app->getResourceManager().getFBXModel("block").getModel();

	float scale = 5.0f;
	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		for (int j = 0; j < 10; j++) {
			m_scoreBlocks.push_back(std::make_unique<Block>(m_blockModel));
			m_scoreBlocks.back()->getTransform().setScale(scale);
			m_scoreBlocks.back()->setPosition(Vector3((float)i * (3.0f * scale) + m_currentLevel->getGridWidth() / 2.0f - 1.5f * scale * (m_numberOfTeams - 1), (float)j * scale + m_currentLevel->getGridHeight() / 2.0f - 6.0f * scale, 100.0f));
		}
	}

	scale = 2.0f;
	for (unsigned int i = 0; i < 20; i++) {
		m_randomBlocks.push_back(std::make_unique<Block>(m_blockModel));
		m_randomBlocks.back()->getTransform().setScale(scale);
		m_randomBlocks.back()->setPosition(Vector3(m_currentLevel->getGridWidth() / 2.0f + Utils::rnd() * 40.0f - 20.0f, m_currentLevel->getGridHeight() / 2.0f + Utils::rnd() * 30.0f - 15.0f, 80.0f + Utils::rnd() * 20.0f - 10.0f));
		m_targetPositions.push_back(Vector3(m_currentLevel->getGridWidth() / 2.0f + Utils::rnd() * 40.0f - 20.0f, m_currentLevel->getGridHeight() / 2.0f + Utils::rnd() * 30.0f - 15.0f, 80.0f + Utils::rnd() * 20.0f - 10.0f));
		m_accelerations.push_back(0.0f);
		m_velocities.push_back(Vector3(0.0f, 0.0f, 0.0f));
	}

	m_animationClock = 0.0f;
}

ScoreVisualization::~ScoreVisualization() {

}

void ScoreVisualization::update(float dt) {
	GameInfo* info = GameInfo::getInstance();

	unsigned int currentGamemode = info->convertedGameSettings.gamemode;

	float targetScore = info->convertedGameSettings.scoreLimit;

	if (currentGamemode == 0) {
		targetScore *= 1.0f;
	}

	std::vector<float> finalScore = m_currentGamemode->getScore();

	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		finalScore[i] = (finalScore[i] / targetScore) * 10.0f;
	}

	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
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
	}

	for (unsigned int i = 0; i < m_randomBlocks.size(); i++) {
		Vector3 tempPos = m_randomBlocks[i]->getTransform().getTranslation();
		m_accelerations[i] = Utils::rnd() * 0.2f;
		m_velocities[i] += (m_targetPositions[i] - tempPos) * m_accelerations[i] * dt;
		float m_tempVelocity = min(m_velocities[i].Length(), 7.0f);
		m_velocities[i].Normalize();
		m_velocities[i] *= m_tempVelocity;
		m_randomBlocks[i]->getTransform().setTranslation(tempPos + m_velocities[i] * dt);
	}

	if (m_animationClock > 4.0f) {
		for (unsigned int i = 0; i < m_targetPositions.size(); i++) {
			m_targetPositions[i] = Vector3(m_currentLevel->getGridWidth() / 2.0f + Utils::rnd() * 90.0f - 45.0f, m_currentLevel->getGridHeight() / 2.0f + Utils::rnd() * 50.0f - 25.0f, 90.0f + Utils::rnd() * 50.0f - 25.0f);
		}
		m_animationClock = 0.0f;
	}

	m_animationClock += dt;
}

void ScoreVisualization::draw() {
	for (unsigned int i = 0; i < m_scoreBlocks.size(); i++) {
		m_scoreBlocks[i]->draw();
	}

	for (unsigned int i = 0; i < m_randomBlocks.size(); i++) {
		m_randomBlocks[i]->draw();
	}
}