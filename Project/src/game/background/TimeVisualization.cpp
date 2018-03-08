#include "TimeVisualization.h"

#include <cmath>

#define PI 3.14159265

TimeVisualization::TimeVisualization(Level* level, Gamemode* gamemode) {
	GameInfo* info = GameInfo::getInstance();

	m_level = level;
	m_gamemode = gamemode;

	m_timeBlocks.resize(20);
	m_run = false;
	m_timeLeft = gamemode->getGametime();

	if (m_timeLeft < 60000000.f) {
		m_blockModel = Application::getInstance()->getResourceManager().getFBXModel("block").getModel();

		m_run = true;
		m_maxTime = m_timeLeft;
		m_secondsPerBlock = m_maxTime / float(m_timeBlocks.size());

		float scale = 3.f;

		float paj = PI / (float(m_timeBlocks.size()) / 2.f);
		m_blockModel->getTransform().setScale(scale);
		for (int i = 0; i < m_timeBlocks.size(); i++) {
			m_timeBlocks[i] = std::make_unique<Block>(m_blockModel);
			
			float x = cos(i * paj) * 2.5f * scale + (m_level->getGridWidth() * m_level->DEFAULT_BLOCKSIZE) / 2.f - level->DEFAULT_BLOCKSIZE / 2.f;
			float y = sin(i * paj) * 2.5f * scale + (m_level->getGridHeight() * m_level->DEFAULT_BLOCKSIZE) / 2.f - level->DEFAULT_BLOCKSIZE / 2.f;
			m_timeBlocks[i]->setPosition(DirectX::SimpleMath::Vector3(x, y, 3.f * scale));
		}
	}
}

TimeVisualization::~TimeVisualization() {

}


void TimeVisualization::update(float dt) {
	if (!m_run)
		return;

	m_timeLeft = m_gamemode->getGametime();

	while (m_timeLeft < m_secondsPerBlock * float(m_timeBlocks.size() - 1))
		m_timeBlocks.pop_back();
}

void TimeVisualization::draw() {
	if (!m_run)
		return;

	for (int i = 0; i < m_timeBlocks.size(); i++)
		m_timeBlocks[i]->draw();
}