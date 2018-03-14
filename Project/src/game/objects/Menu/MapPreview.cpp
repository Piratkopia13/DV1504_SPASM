#include "MapPreview.h"
#include "../../level/Grid.h"

using namespace DirectX::SimpleMath;

MapPreview::MapPreview() {
	m_previousNumberOfBlocks = 0;
}

MapPreview::~MapPreview() {

}

void MapPreview::setMap(const std::string & newMap) {
	m_map = newMap;

	for (unsigned int i = 0; i < m_previousNumberOfBlocks; i++) {
		m_previousTransforms[i] = m_currentTransforms[i];
	}

	m_level.reset(new Level(m_map));
	m_blocks.reset(new InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>(max(m_level->getNumberOfBlocks(), m_previousNumberOfBlocks)));
	float tempScale = min(8.0f/m_level->getGridWidth(), 6.0f/m_level->getGridHeight());
	for (int x = 0; x < m_level->getGridWidth(); x++) {
		for (int y = 0; y < m_level->getGridHeight(); y++) {
			auto* instanceData = m_level->getGrid()->getBlock(x, y);
			if (instanceData) {
				DynBlockDeferredInstancedGeometryShader::InstanceData newInstance;
				newInstance.modelMatrix = Matrix::CreateTranslation(Vector3(-m_level->getGridWidth() * 0.5f + x, -m_level->getGridHeight() * 0.5f + y, 0.0f));
				newInstance.modelMatrix *= Matrix::CreateScale(tempScale);
				newInstance.modelMatrix *= Matrix::CreateRotationY(-1.57f);
				newInstance.modelMatrix *= Matrix::CreateTranslation(-5.0f, -0.5f, 0.0f);
				newInstance.color = Vector3::One;
				newInstance.blockVariationOffset = instanceData->blockVariationOffset;
				m_blocks->addInstance(newInstance);
			}
		}
	}
	m_previousNumberOfBlocks = m_level->getNumberOfBlocks();
}

void MapPreview::activate() {
	m_isActive = true;
}

void MapPreview::deActivate() {
	m_isActive = false;
}

void MapPreview::update(float dt) {
	if (!m_isActive) {
		return;
	}
}

void MapPreview::draw() {
	if (!m_isActive) {
		return;
	}
	m_blocks->draw();
}

