#include "PostProcessPass.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PostProcessPass::PostProcessPass()
{

	createFullscreenQuad();

	Application* app = Application::getInstance();
	UINT windowWidth = app->getWindow()->getWindowWidth();
	UINT windowHeight = app->getWindow()->getWindowHeight();

	m_gaussPass1Scale = 1.f / 1.f;
	m_gaussPass2Scale = 1.f / 1.5f;
	m_gaussPass3Scale = 1.f / 2.f;
	m_brightnessCutoffScale = 1.f / 2;

	m_hGaussStage = std::make_unique<HGaussianBlurStage>(UINT(windowWidth * m_gaussPass1Scale), UINT(windowHeight * m_gaussPass1Scale), &m_fullscreenQuad);
	m_vGaussStage = std::make_unique<VGaussianBlurStage>(UINT(windowWidth * m_gaussPass1Scale), UINT(windowHeight * m_gaussPass1Scale), &m_fullscreenQuad);
	m_hGaussStage2 = std::make_unique<HGaussianBlurStage>(UINT(windowWidth * m_gaussPass2Scale), UINT(windowHeight * m_gaussPass2Scale), &m_fullscreenQuad);
	m_vGaussStage2 = std::make_unique<VGaussianBlurStage>(UINT(windowWidth * m_gaussPass2Scale), UINT(windowHeight * m_gaussPass2Scale), &m_fullscreenQuad);
	m_hGaussStage3 = std::make_unique<HGaussianBlurStage>(UINT(windowWidth * m_gaussPass3Scale), UINT(windowHeight * m_gaussPass3Scale), &m_fullscreenQuad);
	m_vGaussStage3 = std::make_unique<VGaussianBlurStage>(UINT(windowWidth * m_gaussPass3Scale), UINT(windowHeight * m_gaussPass3Scale), &m_fullscreenQuad);

	m_brightnessCutoffStage = std::make_unique<BrightnessCutoffStage>(UINT(windowWidth * m_brightnessCutoffScale), UINT(windowHeight * m_brightnessCutoffScale), &m_fullscreenQuad);

	m_toneMapHackStage = std::make_unique<ToneMapHackStage>(UINT(windowWidth), UINT(windowHeight), &m_fullscreenQuad);

}

void PostProcessPass::resize(UINT width, UINT height) {
	m_hGaussStage->resize(UINT(width * m_gaussPass1Scale), UINT(height * m_gaussPass1Scale));
	m_vGaussStage->resize(UINT(width * m_gaussPass1Scale), UINT(height * m_gaussPass1Scale));
	m_hGaussStage2->resize(UINT(width * m_gaussPass2Scale), UINT(height * m_gaussPass2Scale));
	m_vGaussStage2->resize(UINT(width * m_gaussPass2Scale), UINT(height * m_gaussPass2Scale));
	m_hGaussStage3->resize(UINT(width * m_gaussPass3Scale), UINT(height * m_gaussPass3Scale));
	m_vGaussStage3->resize(UINT(width * m_gaussPass3Scale), UINT(height * m_gaussPass3Scale));
	m_brightnessCutoffStage->resize(UINT(width * m_brightnessCutoffScale), UINT(height * m_brightnessCutoffScale));
	m_toneMapHackStage->resize(UINT(width), UINT(height));
}

PostProcessPass::~PostProcessPass() {
}

void PostProcessPass::run(RenderableTexture& baseTexture, RenderableTexture& inputTexture) {

	auto* dxm = Application::getInstance()->getDXManager();

	dxm->disableDepthBuffer();
	//// Disable conservatiec rasterization to avoid wierd graphical artifacts
	//dxm->disableConservativeRasterizer();

	m_brightnessCutoffStage->run(inputTexture);
	m_hGaussStage->run(m_brightnessCutoffStage->getOutput());
	m_vGaussStage->run(m_hGaussStage->getOutput());

	m_hGaussStage2->run(m_vGaussStage->getOutput());
	m_vGaussStage2->run(m_hGaussStage2->getOutput());

	m_hGaussStage3->run(m_vGaussStage2->getOutput());
	m_vGaussStage3->run(m_hGaussStage3->getOutput());

	m_toneMapHackStage->run(m_vGaussStage3->getOutput());

	// Blend last output together with the baseTexture to produce the final image
	dxm->renderToBackBuffer();
	//m_toneMapHackStage->getOutput().begin();

	dxm->enableAdditiveBlending();
	m_fullscreenQuad.getMaterial()->setTextures(baseTexture.getColorSRV(), 1);
	m_fullscreenQuad.draw();

	// Draw bloom using additive blending
	m_fullscreenQuad.getMaterial()->setTextures(m_toneMapHackStage->getOutput().getColorSRV(), 1);
	m_fullscreenQuad.draw();
	//m_fullscreenQuad.draw();
	dxm->disableAlphaBlending();

	dxm->enableDepthBuffer();
	//// Re-enable conservative rasterization
	//dxm->enableBackFaceCulling();

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
