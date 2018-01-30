#pragma once

#include "PostProcessStage.h"
#include "../../shader/ShaderSet.h"
#include "../../shader/component/ConstantBuffer.h"
#include "../../shader/component/Sampler.h"
#include "../../../Application.h"

class VGaussianBlurStage : public PostProcessStage {
public:
	VGaussianBlurStage(UINT width, UINT height, Model* fullscreenQuad);
	virtual ~VGaussianBlurStage();

	void run(RenderableTexture& inputTexture);

private:

	std::unique_ptr<VertexShader> m_VS;
	std::unique_ptr<PixelShader> m_PS;

	struct CBuffer {
		float texWidth;
		float texHeight;
		float padding[2];
	};

	// Components
	std::unique_ptr<ShaderComponent::ConstantBuffer> m_cBuffer;
	std::unique_ptr<ShaderComponent::Sampler> m_sampler;


};