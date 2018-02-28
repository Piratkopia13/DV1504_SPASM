#include "ScoreVisualization.h"

using namespace DirectX::SimpleMath;

ScoreVisualization::ScoreVisualization(Level* level, Gamemode* currentGamemode) {
	m_currentGamemode = currentGamemode;
	//m_numberOfTeams = currentGamemode.getScore().size();
	m_numberOfTeams = 2;

	Application* app = Application::getInstance();
	m_blockModel = app->getResourceManager().getFBXModel("block").getModel();

	GameInfo * info = GameInfo::getInstance();

	float scale = 5.0f;
	for (unsigned int i = 0; i < m_numberOfTeams; i++) {
		for (int j = 0; j < 10; j++) {
			m_blocks.push_back(std::make_unique<Block>(m_blockModel));
			m_blocks.back()->getTransform().setScale(scale);
			m_blocks.back()->setPosition(Vector3((float) i * (5.0f * scale) + level->getGridWidth() / 2.0f - 2.5f * scale * (m_numberOfTeams - 1), (float) j * scale + level->getGridHeight() / 2.0f - 6.0f * scale, 100.0f));
			m_blocks.back()->setColor(info->getDefaultColor(info->gameSettings.teams[i].color, 0));
		}
	}
}

ScoreVisualization::~ScoreVisualization() {

}

void ScoreVisualization::update(float dt) {

}

void ScoreVisualization::draw() {
	for (unsigned int i = 0; i < m_blocks.size(); i++) {
		m_blocks[i]->draw();
	}
}