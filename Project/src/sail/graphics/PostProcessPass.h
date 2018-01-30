#pragma once

#include "RenderableTexture.h"
#include "geometry/Model.h"

#include "shader/postprocess/PostProcessStage.h"
#include "shader/postprocess/GaussianBlurCShader.h"
#include "shader/postprocess/PostProcessFlushShader.h"

class PostProcessPass {
public:
	PostProcessPass();
	~PostProcessPass();

	void run(RenderableTexture& inputTexture);

private:
	void createFullscreenQuad();

private:
	// Post process stages - TODO: make this a map
	PostProcessFlushShader m_flushStage;
	GaussianBlurCShader m_gaussianBlurStage;



	// TODO make enableEffect(enum) and disableEffect(enum) and a map that binds the enums to enabled/disabled bool

	Model m_fullScreenQuad;

};