#pragma once

#include "../GameInfo.h"
#include "../gamemodes/Gamemode.h"
#include "../objects/common/Object.h"
#include "AnimatedTransform.h"
#include "../objects/InstancedBlocks.h"
#include "../../sail/graphics/shader/deferred/DynBlockDeferredInstancedGeometryShader.h"

class TimeVisualization : public Object {

public:
	TimeVisualization(Level* level, Gamemode* gamemode);
	~TimeVisualization();

	void update(float dt);
	void draw();

private:
	Gamemode * m_gamemode;
	Level* m_level;
	GameInfo* m_info;
	Model* m_blockModel;

	std::unique_ptr<InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>> m_instancedBlocks;
	std::vector<std::unique_ptr<AnimatedTransform>> m_timeTransforms;
	std::vector<bool> m_toRemove;
	
	float m_timeLeft;
	float m_maxTime;
	float m_secondsPerBlock;
	bool m_run;
	unsigned int m_numOfBlocks;

	float m_rightTopBottom;
	float m_leftTopBottom;
	float m_leftRight;
	float m_topBottom;
	float m_leftRightTopBottom;

	DirectX::SimpleMath::Vector3 m_middle;

};