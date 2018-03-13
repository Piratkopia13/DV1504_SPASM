#include "MapPreview.h"
#include "../../level/Grid.h"

using namespace DirectX::SimpleMath;

MapPreview::MapPreview() {
	m_blocks = std::make_unique<InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>>(10000U);
}

MapPreview::~MapPreview() {

}

void MapPreview::setMap(const std::string & newMap) {
	m_blocks.reset(new InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>(10000U));
	m_map = newMap;
	m_level.reset(new Level(m_map));
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

