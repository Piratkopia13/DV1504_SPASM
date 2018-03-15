#include "MapPreview.h"
#include "../../level/Grid.h"

using namespace DirectX::SimpleMath;

MapPreview::MapPreview() {
	m_previousNumberOfBlocks = 0;
	m_previousScale = 0.01f;
	m_targetScale = 0.01f;
	m_animationTime = .8f;
	m_animationTimer = 0.0f;
}

MapPreview::~MapPreview() {

}

void MapPreview::setMap(const std::string & newMap) {
	m_map = newMap;
	m_level.reset(new Level(m_map));

	float defaultVariation = 1.f / 16.f;

	m_previousScale = m_currentScale;
	m_targetScale = min(8.0f / m_level->getGridWidth(), 6.0f / m_level->getGridHeight());

	m_currentScale = m_previousScale;
	m_rotTrans = Matrix::CreateTranslation(0.f, -0.5f, 5.0f);
	m_rotTrans *= Matrix::CreateRotationY(-1.57f);

	UINT newNumberOfBlocks = m_level->getNumberOfBlocks();
	UINT newSize = max(newNumberOfBlocks, m_previousNumberOfBlocks);
	m_transforms.resize(newSize);
	m_blockVariations.resize(newSize);

	if (newNumberOfBlocks > m_previousNumberOfBlocks) {
		// New blocks
		for (unsigned int i = m_previousNumberOfBlocks; i < newNumberOfBlocks; i++) {
			//m_transforms[i].setStartPos(Vector3(0.f, 0.f, -(Utils::rnd() * 10.f + 6.f) / m_targetScale));
			float rndX = (Utils::rnd() - 0.5f) * 10.f;
			rndX = (rndX > 0) ? rndX + 12.f : rndX - 12.f;
			float rndY = (Utils::rnd() - 0.5f) * 7.f;
			rndY = (rndY > 0) ? rndY + 10.f : rndY - 10.f;
			m_transforms[i].setStartPos(Vector3(rndX / m_targetScale, rndY / m_targetScale, 0.f));
		}
	} else {
		// Set target position for "extra blocks" to outside the area somewhere
		for (unsigned int i = newNumberOfBlocks; i < m_previousNumberOfBlocks; i++) {
			float rndX = (Utils::rnd() - 0.5f) * 10.f;
			rndX = (rndX > 0) ? rndX + 12.f : rndX - 12.f;
			float rndY = (Utils::rnd() - 0.5f) * 7.f;
			rndY = (rndY > 0) ? rndY + 10.f : rndY - 10.f;
			m_transforms[i].setTargetPos(Vector3(rndX / m_targetScale, rndY / m_targetScale, 0.f));

			//m_transforms[i].setTargetPos(Vector3(0.f, 0.f, -(Utils::rnd() * 10.f + 6.f) / m_targetScale), m_animationTime);
			//m_transforms[i].setAcceleration(Vector3(Utils::rnd() * 10.f));
		}
	}

	m_blocks.reset(new InstancedBlocks<DynBlockDeferredInstancedGeometryShader, DynBlockDeferredInstancedGeometryShader::InstanceData>(newSize));

	unsigned int index = 0;
	for (int x = 0; x < m_level->getGridWidth(); x++) {
		for (int y = 0; y < m_level->getGridHeight(); y++) {
			auto* instanceData = m_level->getGrid()->getBlock(x, y);
			if (instanceData) {

				// Update visible block to move to its new position
				m_transforms[index].setTargetPos(Vector3(-m_level->getGridWidth() * 0.5f + x, -m_level->getGridHeight() * 0.5f + y, 0.0f), m_animationTime);

				DynBlockDeferredInstancedGeometryShader::InstanceData newInstance;
				newInstance.modelMatrix = Matrix::CreateTranslation(m_transforms[index].getTranslation());
				newInstance.modelMatrix = Matrix::CreateScale(m_currentScale);
				newInstance.modelMatrix *= m_rotTrans;
				newInstance.color = Vector3::One;
				newInstance.blockVariationOffset = defaultVariation;
				m_blocks->addInstance(newInstance);

				m_blockVariations[index] = instanceData->blockVariationOffset;

				index++;
			}
		}
	}

	for (unsigned int i = index; i < newSize; i++) {
		DynBlockDeferredInstancedGeometryShader::InstanceData newInstance;
		newInstance.modelMatrix = Matrix::CreateTranslation(m_transforms[i].getTranslation());
		newInstance.modelMatrix = Matrix::CreateScale(m_currentScale);
		newInstance.modelMatrix *= m_rotTrans;
		newInstance.color = Vector3::One;
		newInstance.blockVariationOffset = defaultVariation;
		m_blocks->addInstance(newInstance);
	}

	m_previousNumberOfBlocks = m_level->getNumberOfBlocks();
	m_animationTimer = 0.f;
}

void MapPreview::activate() {
	m_isActive = true;
	setMap(m_map);
}

void MapPreview::deActivate() {
	m_isActive = false;
	m_targetScale = 0.01f;
	m_previousScale = 0.01f;
	m_currentScale = 0.01f;
}

void MapPreview::update(float dt) {
	if (!m_isActive) {
		return;
	}

	//Scale interpolation
	if (m_animationTimer <= m_animationTime) {
		m_animationTimer += dt;
		m_currentScale = (Utils::smootherstep(0, 1, m_animationTimer / m_animationTime)) * (m_targetScale - m_previousScale) + m_previousScale;
	}

	for (unsigned int i = 0; i < m_transforms.size(); i++) {
		m_transforms[i].update(dt);
		m_blocks->getInstanceData(i).modelMatrix = Matrix::CreateTranslation(m_transforms[i].getTranslation());
		m_blocks->getInstanceData(i).modelMatrix *= Matrix::CreateScale(m_currentScale);
		m_blocks->getInstanceData(i).modelMatrix *= m_rotTrans;

		if (m_transforms[i].atTargetPos()) {
			m_blocks->getInstanceData(i).blockVariationOffset = m_blockVariations[i];
		}

	}

}

void MapPreview::draw() {
	if (!m_isActive) {
		return;
	}
	m_blocks->draw();
}

