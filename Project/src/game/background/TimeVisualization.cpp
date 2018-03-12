#include "TimeVisualization.h"

#include <cmath>

TimeVisualization::TimeVisualization(Level* level, Gamemode* gamemode) {
	GameInfo* info = GameInfo::getInstance();

	m_level = level;
	m_gamemode = gamemode;

	m_timeLeft = gamemode->getGametime();
	m_run = false;

	if (m_timeLeft < 60000000.f) {

		m_instancedBlocks = std::make_unique<InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>>();

		m_timeTransforms.resize(120);
		m_toRemove.resize(m_timeTransforms.size());
		m_instancedBlocks->init(m_timeTransforms.size());

		m_blockModel = Application::getInstance()->getResourceManager().getFBXModel("block").getModel();

		m_numOfBlocks = m_timeTransforms.size();

		m_run = true;
		m_maxTime = m_timeLeft;
		m_secondsPerBlock = m_maxTime / float(m_timeTransforms.size());

		float scale = 2.f;

		float paj = DirectX::XM_PI / (float(m_timeTransforms.size()) / 2.f);

		/*
		// Normal blocks
		DynBlockDeferredInstancedGeometryShader::InstanceData instanceData;
		instanceData.modelMatrix = Matrix::CreateTranslation(Vector3(float(x + 0.5f) * DEFAULT_BLOCKSIZE, float(y + 0.5f) * DEFAULT_BLOCKSIZE, 0.f));
		m_grid->addBlock(&m_instancedBlocks->addInstance(instanceData), x, y);
		m_blocks[x][y].data = m_grid->getBlock(x, y);
		//m_blocks[x][y].setHP
		instanceData.color = Vector3::One;*/

		// Middle of map
		m_middle = DirectX::SimpleMath::Vector3((m_level->getGridWidth() * m_level->DEFAULT_BLOCKSIZE) / 2.f, (m_level->getGridHeight() * m_level->DEFAULT_BLOCKSIZE) / 2.f - level->DEFAULT_BLOCKSIZE / 2.f, 3.f);
		
		// 5/16
		m_rightTopBottom = 0.3125f;
		// 3/16
		m_leftTopBottom = 0.1875f;
		// 11/16
		m_topBottom = 0.6875f;
		// 1/16
		m_leftRightTopBottom = 0.0625f;

		DynBlockDeferredInstancedGeometryShader::InstanceData instanceData;
		instanceData.blockVariationOffset = m_topBottom;

		for (unsigned int i = 0; i < m_timeTransforms.size(); i++) {
			m_timeTransforms[i] = std::make_unique<AnimatedTransform>();
			m_timeTransforms[i]->setScale(scale);
			
			// Sets the size of the clock depending on the number of teams in play, aswell as the scale of the clock
			float numOfTeams = float(m_gamemode->getScore().size());
			float x = cos(i * paj + DirectX::XM_PI / 2.f) * 8.f * numOfTeams * scale + m_middle.x;
			float y = sin(i * paj + DirectX::XM_PI / 2.f) * 16.f * scale + m_middle.y;
			m_timeTransforms[i]->setTranslation(m_middle);
			DirectX::SimpleMath::Vector3 rot = DirectX::SimpleMath::Vector3(y - m_middle.y, -(x - m_middle.x), 0.f);
			rot.Normalize();

			m_timeTransforms[i]->rotateAroundZ(atan2(rot.y, rot.x));

			instanceData.color = DirectX::SimpleMath::Vector3(0.5f, 0.5f, 0.5f);
			instanceData.modelMatrix = m_timeTransforms[i]->getMatrix();
			m_instancedBlocks->addInstance(instanceData);

			m_timeTransforms[i]->setTargetPos(DirectX::SimpleMath::Vector3(x, y, 50.f * scale), 3.f);

			m_toRemove[i] = false;
		}
		m_instancedBlocks->getInstanceData(0).blockVariationOffset = m_leftTopBottom;
	}
}

TimeVisualization::~TimeVisualization() {

}


void TimeVisualization::update(float dt) {
	if (!m_run)
		return;

	m_timeLeft = m_gamemode->getGametime();

	// Depending on the amount of blocks left, removes x number of blocks to represent the time
	float numToRemove = 0;
	while (m_timeLeft < m_secondsPerBlock * float(m_numOfBlocks - 1) && m_numOfBlocks > 0) {
		numToRemove++;
		m_numOfBlocks--;
	}

	// Blocks flying away
	for (unsigned int i = m_numOfBlocks - 1; i < m_timeTransforms.size(); i++) {
		if (!m_toRemove[i]) {
			m_toRemove[i] = true;
			DirectX::SimpleMath::Vector3 dir = m_timeTransforms[i]->getTranslation() - m_middle;
			DirectX::SimpleMath::Vector3 targetPos = dir * (max(m_middle.x / 2.f, m_middle.y / 2.f));
			targetPos = m_timeTransforms[i]->getTranslation();
			targetPos.z = 1000.f;
			m_timeTransforms[i]->setTargetPos(targetPos, 4.f);
			m_instancedBlocks->getInstanceData(m_numOfBlocks - 1).blockVariationOffset = m_leftRightTopBottom;

			if (m_numOfBlocks - 2 >= 0) {
				m_instancedBlocks->getInstanceData(m_numOfBlocks - 2).blockVariationOffset = m_rightTopBottom;
			}
		}
	}

	// Updates all live blocks
	for (unsigned int i = 0; i < m_timeTransforms.size(); i++) {
		m_timeTransforms[i]->update(dt);
		m_timeTransforms[i]->rotateAroundX(dt);

		m_instancedBlocks->getInstanceData(i).modelMatrix = m_timeTransforms[i]->getMatrix();
	}

	// Removes all blocks that have flown into the sunset
	unsigned int mtrSize = m_toRemove.size() - 1;
	while (m_toRemove[mtrSize] && m_timeTransforms[mtrSize]->atTargetPos()) {
		m_toRemove.pop_back();
		mtrSize = m_toRemove.size() - 1;
		m_timeTransforms[mtrSize]->setScale(0.f);
		m_instancedBlocks->getInstanceData(mtrSize).modelMatrix = m_timeTransforms[mtrSize]->getMatrix();
	}

}

void TimeVisualization::draw() {
	if (!m_run)
		return;

	m_instancedBlocks->draw();
	/*for (unsigned int i = 0; i < m_timeTransforms.size(); i++)
		m_timeTransforms[i]->draw();*/
}