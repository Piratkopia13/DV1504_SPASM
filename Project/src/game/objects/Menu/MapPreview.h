#pragma once

#include "../common/Object.h"
#include "../InstancedBlocks.h"
#include "../../background/AnimatedTransform.h"
#include "../../level/Level.h"

class MapPreview: public Object {
public:
	MapPreview();
	~MapPreview();

	void setMap(const std::string& newMap);
	void activate();
	void deActivate();

	void update(float dt);

	void draw();
private:
	std::string m_map;
	std::unique_ptr<Level> m_level;
	std::vector<AnimatedTransform> m_transforms;
	std::unique_ptr<InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>> m_blocks;
	bool m_isActive;
	unsigned int m_previousNumberOfBlocks;
	DirectX::SimpleMath::Matrix m_rotTrans;
	float m_previousScale, m_targetScale, m_currentScale;
	float m_animationTime, m_animationTimer;
	std::vector<float> m_blockVariations;
};