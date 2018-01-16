#include "BillboardSystem.h"


BillboardSystem::BillboardSystem(ParticleShader& shader, const std::string& textureFilename)
: m_shader(shader){
	m_dt = 1 / 30.f;
	m_numElements = 0;
	m_maxNumElements = 1000000;

	m_emitterUpdated = true;
	m_randVecUpdated = true;

	if (!Application::getInstance()->getResourceManager().hasDXTexture(textureFilename)) {
		Application::getInstance()->getResourceManager().LoadDXTexture(textureFilename);
	}

	shader.setTextureSRV(*Application::getInstance()->getResourceManager().getDXTexture(textureFilename).getResourceView());
}

BillboardSystem::~BillboardSystem() {}

void BillboardSystem::emitBillboards(UINT numBatches) {

	if (m_emitterUpdated || m_randVecUpdated)
		m_shader.updateParticleParameterBuffer(m_emitterLocation, m_randEmitVector);

	if (m_numElements < m_maxNumElements) {
		m_shader.emitParticles(numBatches);
		// threads/particles per group
		int numThreadsPerGroup = 1;
		m_numElements += numBatches * numThreadsPerGroup;
	}

}

void BillboardSystem::update() {	

	m_shader.updateParticles(m_numElements);

}

void BillboardSystem::setEmitterLocation(const DirectX::SimpleMath::Vector3& emitterLocation) {
	m_emitterLocation = emitterLocation;
}

void BillboardSystem::setRandEmitVector(const DirectX::SimpleMath::Vector3& randVec) {
	m_randEmitVector = randVec;
}

UINT BillboardSystem::getNumParticles() {
	return m_numElements;
}