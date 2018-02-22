#include "PostProcessPass.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PostProcessPass::PostProcessPass()
{

	createFullscreenQuad();

	Application* app = Application::getInstance();
	UINT windowWidth = app->getWindow()->getWindowWidth();
	UINT windowHeight = app->getWindow()->getWindowHeight();

	m_gaussPass1Scale = 1.f / 1;
	m_gaussPass2Scale = 1.f / 2;
	m_brightnessCutoffScale = 1.f / 2;

	m_FXAAPass = false;

	m_FXAAStage = std::make_unique<FXAAStage>(windowWidth, windowHeight, &m_fullscreenQuad);

	m_hGaussStage = std::make_unique<HGaussianBlurStage>(UINT(windowWidth * m_gaussPass1Scale), UINT(windowHeight * m_gaussPass1Scale), &m_fullscreenQuad);
	m_vGaussStage = std::make_unique<VGaussianBlurStage>(UINT(windowWidth * m_gaussPass1Scale), UINT(windowHeight * m_gaussPass1Scale), &m_fullscreenQuad);

	m_hGaussStage2 = std::make_unique<HGaussianBlurStage>(UINT(windowWidth * m_gaussPass2Scale), UINT(windowHeight * m_gaussPass2Scale), &m_fullscreenQuad);
	m_vGaussStage2 = std::make_unique<VGaussianBlurStage>(UINT(windowWidth * m_gaussPass2Scale), UINT(windowHeight * m_gaussPass2Scale), &m_fullscreenQuad);

	m_brightnessCutoffStage = std::make_unique<BrightnessCutoffStage>(UINT(windowWidth * m_brightnessCutoffScale), UINT(windowHeight * m_brightnessCutoffScale), &m_fullscreenQuad);

}

void PostProcessPass::resize(UINT width, UINT height) {
	m_FXAAStage->resize(width, height);
	m_hGaussStage->resize(UINT(width * m_gaussPass1Scale), UINT(height * m_gaussPass1Scale));
	m_vGaussStage->resize(UINT(width * m_gaussPass1Scale), UINT(height * m_gaussPass1Scale));
	m_hGaussStage2->resize(UINT(width * m_gaussPass2Scale), UINT(height * m_gaussPass2Scale));
	m_vGaussStage2->resize(UINT(width * m_gaussPass2Scale), UINT(height * m_gaussPass2Scale));
	m_brightnessCutoffStage->resize(UINT(width * m_brightnessCutoffScale), UINT(height * m_brightnessCutoffScale));
}

PostProcessPass::~PostProcessPass() {
}

void PostProcessPass::run(RenderableTexture& baseTexture, RenderableTexture& inputTexture) {

	auto* dxm = Application::getInstance()->getDXManager();
	auto& kbState = Application::getInstance()->getInput().getKbStateTracker();

	dxm->disableDepthBuffer();

	if (kbState.pressed.H) {
		m_FXAAPass = !m_FXAAPass;
	}


	if (m_FXAAPass) {
		m_FXAAStage->run(baseTexture);
	}
	m_brightnessCutoffStage->run(inputTexture);

	m_hGaussStage->run(m_brightnessCutoffStage->getOutput());
	m_vGaussStage->run(m_hGaussStage->getOutput());

	m_hGaussStage2->run(m_vGaussStage->getOutput());
	m_vGaussStage2->run(m_hGaussStage2->getOutput());

	// Blend last output together with the baseTexture to produce the final image
	dxm->renderToBackBuffer();

	dxm->enableAdditiveBlending();
	if (m_FXAAPass) {
		m_fullscreenQuad.getMaterial()->setTextures(m_FXAAStage->getOutput().getColorSRV(), 1);
	}
	else
	{
		m_fullscreenQuad.getMaterial()->setTextures(baseTexture.getColorSRV(), 1);
	}
	m_fullscreenQuad.draw();

	// Draw bloom using additive blending
	m_fullscreenQuad.getMaterial()->setTextures(m_vGaussStage2->getOutput().getColorSRV(), 1);



	
	m_fullscreenQuad.draw();
	m_fullscreenQuad.draw();
	dxm->disableAlphaBlending();

	dxm->enableDepthBuffer();

}

void PostProcessPass::createFullscreenQuad() {

	Vector2 halfSizes(1.f, 1.f);

	const int numVerts = 4;
	Vector3* positions = new Vector3[numVerts]{
		Vector3(-halfSizes.x, -halfSizes.y, 0.f),
		Vector3(-halfSizes.x, halfSizes.y, 0.f),
		Vector3(halfSizes.x, -halfSizes.y, 0.f),
		Vector3(halfSizes.x, halfSizes.y, 0.f),
	};

	const int numIndices = 6;
	ULONG* indices = new ULONG[numIndices]{
		0, 1, 2, 2, 1, 3
	};

	// Tex coords not used in shader, only set to get rid of warning
	Vector2* texCoords = new Vector2[numVerts]{
		Vector2(0.f, 1.f),
		Vector2(0.f, 0.f),
		Vector2(1.f, 1.f),
		Vector2(1.f, 0.f)
	};

	Model::Data data;
	data.numVertices = numVerts;
	data.numIndices = numIndices;
	data.positions = positions;
	data.indices = indices;
	data.texCoords = texCoords;

	m_fullscreenQuad.setBuildData(data);
	m_fullscreenQuad.buildBufferForShader(&m_flushShader);
}
