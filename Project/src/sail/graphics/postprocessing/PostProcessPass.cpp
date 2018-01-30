#include "PostProcessPass.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PostProcessPass::PostProcessPass() {

	createFullscreenQuad();

	Application* app = Application::getInstance();
	UINT windowWidth = app->getWindow()->getWindowWidth();
	UINT windowHeight = app->getWindow()->getWindowHeight();

	m_hGaussStage = std::make_unique<HGaussianBlurStage>(windowWidth / 4U, windowHeight / 4U, &m_fullscreenQuad);
	m_vGaussStage = std::make_unique<VGaussianBlurStage>(windowWidth / 4U, windowHeight / 4U, &m_fullscreenQuad);

	m_hGaussStage2 = std::make_unique<HGaussianBlurStage>(windowWidth / 8U, windowHeight / 8U, &m_fullscreenQuad);
	m_vGaussStage2 = std::make_unique<VGaussianBlurStage>(windowWidth / 8U, windowHeight / 8U, &m_fullscreenQuad);

	m_brightnessCutoffStage = std::make_unique<BrightnessCutoffStage>(windowWidth / 2U, windowHeight / 2U, &m_fullscreenQuad);

}

PostProcessPass::~PostProcessPass() {
}

void PostProcessPass::run(RenderableTexture & inputTexture) {

	auto* dxm = Application::getInstance()->getDXManager();

	dxm->disableDepthBuffer();

	m_brightnessCutoffStage->run(inputTexture);
	m_hGaussStage->run(m_brightnessCutoffStage->getOutput());
	m_vGaussStage->run(m_hGaussStage->getOutput());

	m_hGaussStage2->run(m_vGaussStage->getOutput());
	m_vGaussStage2->run(m_hGaussStage2->getOutput());



	//m_flushStage.run();

	dxm->renderToBackBuffer();

	dxm->enableAdditiveBlending();
	m_fullscreenQuad.getMaterial()->setTextures(inputTexture.getColorSRV(), 1);
	m_fullscreenQuad.draw();

	// Draw bloom using additive blending
	m_fullscreenQuad.getMaterial()->setTextures(m_vGaussStage2->getOutput().getColorSRV(), 1);
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
