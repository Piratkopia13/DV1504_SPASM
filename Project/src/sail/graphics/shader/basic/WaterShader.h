#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include "../ShaderSet.h"
#include "../component/ConstantBuffer.h"
#include "../component/Sampler.h"
#include "../../geometry/Material.h"
#include "../../../Application.h"
#include "../../Lights.h"

class WaterShader : public ShaderSet {
public:
	WaterShader();
	~WaterShader();

	void bind() override;

	virtual void draw(Model& model, bool bindFirst = true);

	virtual void createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data);

	virtual void updateCamera(Camera& cam, bool waterReflection = false);

	void updateLights(const Lights& lights);
	void updateWaves(float dt);

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;
	};

private:
	void updateBuffer(const DirectX::SimpleMath::Matrix& w, const DirectX::SimpleMath::Matrix& vp) const;
	void updateCameraBuffer();

private:
	// Input element description
	static D3D11_INPUT_ELEMENT_DESC IED[2];
	// Input layout
	ID3D11InputLayout* m_inputLayout;

	// Transform constant buffer structure
	struct ModelDataBuffer {
		DirectX::SimpleMath::Matrix mWorld;
		DirectX::SimpleMath::Matrix mVP;
	};
	struct WaterStuffBuffer {
		float dudvMoveAmount;
		float padding1, padding2, padding3;
	};
	struct LightBuffer {
		Lights::DirectionalLight dirLight;
	};
	struct CameraBuffer {
		DirectX::SimpleMath::Vector3 camPos;
		float padding;
	};
	DirectX::SimpleMath::Matrix m_vpMatrix;
	DirectX::SimpleMath::Vector3 m_camPos;
	bool m_camPosHasChanged;

	// Components

	std::unique_ptr<ShaderComponent::ConstantBuffer> m_transformBuffer;
	std::unique_ptr<ShaderComponent::ConstantBuffer> m_cameraBuffer;
	std::unique_ptr<ShaderComponent::ConstantBuffer> m_waterBuffer;
	std::unique_ptr<ShaderComponent::ConstantBuffer> m_lightBuffer;
	std::unique_ptr<ShaderComponent::Sampler> m_sampler;

	static const float WAVE_SPEED;
	float m_dudvMoveAmount;

};
