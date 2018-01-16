#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include "../ShaderSet.h"
#include "../component/ConstantBuffer.h"
#include "../component/Sampler.h"
#include "../../../Application.h"
#include "../../Lights.h"
#include "../../geometry/Material.h"

class TessellationTestShader : public ShaderSet {
public:
	TessellationTestShader();
	~TessellationTestShader();

	void bind() override;

	virtual void draw(Model& model, bool bindFirst = true);

	virtual void createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data);

	virtual void updateCamera(Camera& cam, bool waterReflection = false);

	virtual void setClippingPlane(const DirectX::SimpleMath::Vector4& clippingPlane) {};

	struct Vertex {
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 normal;
		DirectX::SimpleMath::Vector2 texCoords;
	};

private:
	void updateModelDataBuffer(const DirectX::SimpleMath::Matrix& w, const DirectX::SimpleMath::Matrix& vp) const;

private:
	// Input element description
	static D3D11_INPUT_ELEMENT_DESC IED[3];
	// Input layout
	ID3D11InputLayout* m_inputLayout;

	// Transform constant buffer structure
	struct ModelDataBuffer {
		DirectX::SimpleMath::Matrix mWorld;
		DirectX::SimpleMath::Matrix mVP;
	};
	DirectX::SimpleMath::Matrix m_mVP;

	// Components

	std::unique_ptr<ShaderComponent::ConstantBuffer> m_modelDataBuffer;
	std::unique_ptr<ShaderComponent::Sampler> m_sampler;

};
