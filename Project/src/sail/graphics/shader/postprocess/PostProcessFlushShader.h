#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include "../ShaderSet.h"
#include "../component/ConstantBuffer.h"
#include "../component/Sampler.h"
#include "../../../Application.h"
#include "../../Lights.h"
#include "../../geometry/Material.h"

class PostProcessFlushShader : public ShaderSet {
public:
	PostProcessFlushShader();
	~PostProcessFlushShader();

	void bind() override;

	virtual void draw(Model& model, bool bindFirst = true);

	virtual void createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data);

	struct Vertex {
		DirectX::SimpleMath::Vector3 position;
	};

private:
	// Input element description
	static D3D11_INPUT_ELEMENT_DESC IED[1];
	// Input layout
	ID3D11InputLayout* m_inputLayout;

	// Components

	std::unique_ptr<ShaderComponent::Sampler> m_sampler;

};
