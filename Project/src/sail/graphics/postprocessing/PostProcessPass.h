#pragma once

#include "../RenderableTexture.h"
#include "../geometry/Model.h"

#include "stages/PostProcessStage.h"
#include "stages/HGaussianBlurStage.h"
#include "stages/VGaussianBlurStage.h"
#include "stages/BrightnessCutoffStage.h"
#include "stages/FXAAStage.h"
//#include "../shader/postprocess/GaussianBlurCShader.h"
#include "../shader/postprocess/PostProcessFlushShader.h"

class PostProcessPass {
public:
	PostProcessPass();
	~PostProcessPass();

	void run(RenderableTexture& baseTexture, RenderableTexture& inputTexture);
	void resize(UINT width, UINT height);

private:
	void createFullscreenQuad();

private:
	// Post process stages - TODO: make this a map
	std::unique_ptr<FXAAStage> m_FXAAStage;
	std::unique_ptr<HGaussianBlurStage> m_hGaussStage;
	std::unique_ptr<VGaussianBlurStage> m_vGaussStage;
	std::unique_ptr<HGaussianBlurStage> m_hGaussStage2;
	std::unique_ptr<VGaussianBlurStage> m_vGaussStage2;
	std::unique_ptr<BrightnessCutoffStage> m_brightnessCutoffStage;

	PostProcessFlushShader m_flushShader;

	float m_gaussPass1Scale;
	float m_gaussPass2Scale;
	float m_brightnessCutoffScale;

	// TODO make enableEffect(enum) and disableEffect(enum) and a map that binds the enums to enabled/disabled bool

	Model m_fullscreenQuad;

};