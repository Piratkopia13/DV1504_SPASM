#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include "../ShaderSet.h"
#include "../component/ConstantBuffer.h"
#include "../component/Sampler.h"
#include "../../geometry/Material.h"
#include "../../../Application.h"

class SimpleTextureShader : public ShaderSet {
public:
	SimpleTextureShader();
	~SimpleTextureShader();

	void bind() override;

	virtual void draw(Model& model, bool bindFirst = true);

	virtual void createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data);

	virtual void updateCamera(Camera& cam);
	virtual void setClippingPlane(const DirectX::SimpleMath::Vector4& clippingPlane);

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;
	};

private:
	void updateBuffer(const DirectX::SimpleMath::Vector4& color, const DirectX::SimpleMath::Matrix& w, const DirectX::SimpleMath::Matrix& vp) const;

private:
	// Input element description
	static D3D11_INPUT_ELEMENT_DESC IED[2];
	// Input layout
	ID3D11InputLayout* m_inputLayout;

	// Transform constant buffer structure
	struct ModelDataBuffer {
		DirectX::SimpleMath::Vector4 modelColor;
		DirectX::SimpleMath::Matrix mWorld;
		DirectX::SimpleMath::Matrix mVP;
	};
	struct ClippingPlaneBuffer {
		DirectX::SimpleMath::Vector4 clippingPlane;
	};
	DirectX::SimpleMath::Matrix m_vpMatrix;

	// Components

	// Transform constant buffer structure
	std::unique_ptr<ShaderComponent::ConstantBuffer> m_transformBuffer;
	// Texture sampler
	std::unique_ptr<ShaderComponent::Sampler> m_sampler;
	// Clipping plane constant buffer structure
	std::unique_ptr<ShaderComponent::ConstantBuffer> m_clippingPlaneBuffer;

};
