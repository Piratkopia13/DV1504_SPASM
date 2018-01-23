#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include "../component/ConstantBuffer.h"
#include "../component/Sampler.h"
#include "../../camera/Camera.h"
#include "../../geometry/billboards/BillboardSystem.h"
#include "../../../Application.h"
#include "../ShaderSet.h"

class ParticleShader : public ShaderSet {
public:
	ParticleShader();
	~ParticleShader();

	void bind() override;

	void createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data) {}

	void createBuffers();

	virtual void draw(bool bindFirst = true);

	virtual void updateCamera(Camera& cam);
	void updateModelDataBuffer() const;
	void updateParticleParameterBuffer(const DirectX::SimpleMath::Vector3& EmitterLocation = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f),
		const DirectX::SimpleMath::Vector3& RandomVector = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f));
	void updateParticleCountBuffer(UINT numParticles);

	void setTextureSRV(ID3D11ShaderResourceView* srv);

	void updateParticles(UINT& numParticles);
	void emitParticles(UINT& numBatches);

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 direction;
	};


private:
	void updateConstantBuffer() const;

private:

	ID3D11Buffer* m_currStateBuffer;
	ID3D11ShaderResourceView* m_currSRV;
	ID3D11UnorderedAccessView* m_currUAV;

	ID3D11Buffer* m_nextStateBuffer;
	ID3D11ShaderResourceView* m_nextSRV;
	ID3D11UnorderedAccessView* m_nextUAV;

	ID3D11Buffer* m_indirectArgsBuffer;

	ID3D11ShaderResourceView* m_texSRV;

	// Camera buffer layout
	struct CameraDataBuffer {
		DirectX::SimpleMath::Vector3 up;
		float padding0;
		DirectX::SimpleMath::Vector3 world_pos;
		float padding1;
	};

	// Model buffer layout
	struct ModelDataBuffer {
		DirectX::SimpleMath::Matrix mVP;
	};

	struct ParticleCountBuffer {
		UINT ParticleCount[4];
	};

	struct ParticleParametersBuffer {
		DirectX::SimpleMath::Vector3 EmitterLocation;
		float padding0;
		DirectX::SimpleMath::Vector3 RandomVector;
		float padding1;
	};

	struct IndirectArgs {
		UINT vertexCount, 
			instanceCount, 
			startVertexLocation, 
			startInstanceLocation;
	};

	DirectX::SimpleMath::Vector3 m_camUp;
	DirectX::SimpleMath::Vector3 m_camPos;

	bool m_cameraDataHasChanged;

	UINT m_maxNumParticles;

	// View projection matrix
	DirectX::SimpleMath::Matrix m_vpMatrix;

	// Components

	std::unique_ptr<ShaderComponent::ConstantBuffer> m_cameraDataBuffer;
	std::unique_ptr<ShaderComponent::ConstantBuffer> m_modelDataBuffer;
	std::unique_ptr<ShaderComponent::ConstantBuffer> m_particleCountBuffer;
	std::unique_ptr<ShaderComponent::ConstantBuffer> m_particleParameterBuffer;
	std::unique_ptr<ShaderComponent::Sampler> m_sampler;


};
