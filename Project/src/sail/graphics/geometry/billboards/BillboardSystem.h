#pragma once

#include <vector>
#include <memory>

#include "../../shader/basic/ParticleShader.h"
#include "Billboard.h"
#include "../Model.h"

class ParticleShader;

class BillboardSystem {
public:
	BillboardSystem(ParticleShader& shader, const std::string& textureFilename);
	~BillboardSystem();

	void emitBillboards(UINT numBatches);
	void update();

	void setEmitterLocation(const DirectX::SimpleMath::Vector3& emitterLocation);
	void setRandEmitVector(const DirectX::SimpleMath::Vector3& randVec);

	UINT getNumParticles();

private:
	UINT m_numElements;
	UINT m_maxNumElements;

	DirectX::SimpleMath::Vector3 m_emitterLocation;
	DirectX::SimpleMath::Vector3 m_randEmitVector;

	bool m_emitterUpdated,
		m_randVecUpdated;
	
	float m_dt;

	ParticleShader& m_shader;
};