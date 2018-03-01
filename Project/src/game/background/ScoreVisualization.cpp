#include "ScoreVisualization.h"

using namespace DirectX::SimpleMath;

ScoreVisualization::ScoreVisualization(Level* level, Gamemode* currentGamemode) {
	m_currentGamemode = currentGamemode;
	m_numberOfTeams = currentGamemode->getScore().size();

	Application* app = Application::getInstance();
	m_blockModel = app->getResourceManager().getFBXModel("block").getModel();

	float scale = 5.0f;
	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		for (int j = 0; j < 10; j++) {
			m_blocks.push_back(std::make_unique<Block>(m_blockModel));
			m_blocks.back()->getTransform().setScale(scale);
			m_blocks.back()->setPosition(Vector3((float) i * (3.0f * scale) + level->getGridWidth() / 2.0f - 1.5f * scale * (m_numberOfTeams - 1), (float) j * scale + level->getGridHeight() / 2.0f - 6.0f * scale, 100.0f));
		}
	}
}

ScoreVisualization::~ScoreVisualization() {

}

void ScoreVisualization::update(float dt) {
	float totScore = 0.0f;

	std::vector<float> finalScore = m_currentGamemode->getScore();

	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		totScore += finalScore[i];
	}

	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		finalScore[i] = (finalScore[i] / totScore) * 10.0f;
	}

	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		for (int j = 0; j < 10; j++) {
			if (finalScore[i] - (float) j > 1) {
				m_blocks[i * 10 + j]->setColor(GameInfo::getInstance()->convertedGameSettings.teams[i].color * 0.5f);
			}
			else if (finalScore[i] - (float) j > 0) {
				m_blocks[i * 10 + j]->setColor(GameInfo::getInstance()->convertedGameSettings.teams[i].color * 0.5f * (finalScore[i] - (float) j));
			}
			else {
				m_blocks[i * 10 + j]->setColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}
}

void ScoreVisualization::draw() {
	for (unsigned int i = 0; i < m_blocks.size(); i++) {
		m_blocks[i]->draw();
	}
}