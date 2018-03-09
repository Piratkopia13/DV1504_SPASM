#include "TimeVisualization.h"

#include <cmath>

#define PI 3.14159265

TimeVisualization::TimeVisualization(Level* level, Gamemode* gamemode) {
	GameInfo* info = GameInfo::getInstance();

	m_level = level;
	m_gamemode = gamemode;

	m_timeLeft = gamemode->getGametime();
	m_run = false;

	if (m_timeLeft < 60000000.f) {

		m_timeBlocks.resize(120);
		m_toRemove.resize(m_timeBlocks.size());

		m_blockModel = Application::getInstance()->getResourceManager().getFBXModel("block").getModel();

		m_numOfBlocks = m_timeBlocks.size();

		m_run = true;
		m_maxTime = m_timeLeft;
		m_secondsPerBlock = m_maxTime / float(m_timeBlocks.size());

		float scale = 3.f;

		float paj = PI / (float(m_timeBlocks.size()) / 2.f);
		m_blockModel->getTransform().setScale(scale);
		m_middle = DirectX::SimpleMath::Vector3((m_level->getGridWidth() * m_level->DEFAULT_BLOCKSIZE) / 2.f, (m_level->getGridHeight() * m_level->DEFAULT_BLOCKSIZE) / 2.f - level->DEFAULT_BLOCKSIZE / 2.f, 3.f);
		for (int i = 0; i < m_timeBlocks.size(); i++) {
			m_timeBlocks[i] = std::make_unique<AnimatedObject>(m_blockModel);
			
			float numOfTeams = float(m_gamemode->getScore().size());
			float x = cos(i * paj + PI / 2.f) * 2.5 * numOfTeams * scale + m_middle.x;
			float y = sin(i * paj + PI / 2.f) * 5.f * scale + m_middle.y;
			m_timeBlocks[i]->setPosition(m_middle);
			DirectX::SimpleMath::Vector3 rot = DirectX::SimpleMath::Vector3(y - m_middle.y, -(x - m_middle.x), 0.f);
			rot.Normalize();

			m_timeBlocks[i]->getTransform().rotateAroundZ(atan2(rot.y, rot.x));

			m_timeBlocks[i]->setTargetPos(DirectX::SimpleMath::Vector3(x, y, 15.f * scale), 3.f);

			m_toRemove[i] = false;
		}
	}
}

TimeVisualization::~TimeVisualization() {

}


void TimeVisualization::update(float dt) {
	if (!m_run)
		return;

	m_timeLeft = m_gamemode->getGametime();

	float numToRemove = 0;
	while (m_timeLeft < m_secondsPerBlock * float(m_numOfBlocks - 1) && m_numOfBlocks > 0) {
		numToRemove++;
		m_numOfBlocks--;
	}

	for (int i = m_numOfBlocks - 1; i < m_timeBlocks.size(); i++) {
		if (!m_toRemove[i]) {
			m_toRemove[i] = true;
			DirectX::SimpleMath::Vector3 dir = m_timeBlocks[i]->getTransform().getTranslation() - m_middle;
			DirectX::SimpleMath::Vector3 targetPos = dir * (max(m_middle.x / 2.f, m_middle.y / 2.f));
			targetPos = m_timeBlocks[i]->getTransform().getTranslation();
			targetPos.z = 1000.f;
			m_timeBlocks[i]->setTargetPos(targetPos, 4.f);
		}
	}

	for (int i = 0; i < m_timeBlocks.size(); i++) {
		m_timeBlocks[i]->update(dt);
		m_timeBlocks[i]->getTransform().rotateAroundX(dt);
	}

	while (m_toRemove[m_toRemove.size() - 1] && m_timeBlocks.back()->atTargetPos()) {
		m_toRemove.pop_back();
		m_timeBlocks.pop_back();
	}

}

void TimeVisualization::draw() {
	if (!m_run)
		return;

	for (int i = 0; i < m_timeBlocks.size(); i++)
		m_timeBlocks[i]->draw();
}