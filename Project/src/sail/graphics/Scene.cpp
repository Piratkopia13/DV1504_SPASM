#include "Scene.h"
#include "../../game/objects/common/Object.h"
#include "../../game/objects/Block.h"

using namespace std;

Scene::Scene(const AABB& worldSize)
	:
	//m_dirLightShadowMap(16384, 8640),
	m_dirLightShadowMap(8192, 4320)
	//, m_dirLightShadowMap(4096, 2160)
	, m_doPostProcessing(false)
{
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(Application::getInstance()->getDXManager()->getDeviceContext());
	m_timer.startTimer();

	// Camera rotation
	m_rotation = 0.f;

	auto window = Application::getInstance()->getWindow();
	UINT width = window->getWindowWidth();
	UINT height = window->getWindowHeight();

	m_prePostTex = std::unique_ptr<RenderableTexture>(new RenderableTexture(1U, width, height, false));
	m_postProcOutputTex = std::unique_ptr<RenderableTexture>(new RenderableTexture(1U, width, height, false, false, D3D11_BIND_UNORDERED_ACCESS));

	m_gaussianBlurShader.setInputSRV(m_prePostTex->getColorSRV());
	m_gaussianBlurShader.setOutputTexture(m_postProcOutputTex->getTexture2D());

	createFullscreenQuad();
	m_postProcessfullScreenPlane.getMaterial()->setTextures(m_postProcOutputTex->getColorSRV(), 1);

}
Scene::~Scene() {}

void Scene::addObject(Object* newObject) {
	m_objects.push_back(newObject);
}

void Scene::addText(Text* text) {
	m_texts.push_back(text);
}

void Scene::addSkybox(const std::wstring& filename) {
	m_cubeMapShader = make_unique<CubeMapShader>();
	m_skybox = make_unique<Skybox>(filename, m_cubeMapShader.get());
}

void Scene::resize(int width, int height) {
	// Resize textures
	m_deferredRenderer.resize(width, height);
	m_prePostTex->resize(width, height);
	m_postProcOutputTex->resize(width, height);
 	m_gaussianBlurShader.resize(width, height);

	// Reset resource views to map to the new textures created by the resize
	m_gaussianBlurShader.setInputSRV(m_prePostTex->getColorSRV());
	m_gaussianBlurShader.setOutputTexture(m_postProcOutputTex->getTexture2D());

	m_postProcessfullScreenPlane.getMaterial()->setTextures(m_postProcOutputTex->getColorSRV(), 1);

}

// Draws the scene
void Scene::draw(float dt, Camera& cam, Level& level) {

	auto* dxm = Application::getInstance()->getDXManager();

	//dxm->getDeviceContext()->ClearState();

	if (m_doPostProcessing) {
		// Render skybox to the prePostTex
		m_prePostTex->clear({ 0.f, 0.f, 0.f, 0.0f });
		dxm->getDeviceContext()->OMSetRenderTargets(1, m_prePostTex->getRenderTargetView(), dxm->getDepthStencilView());
	}

	// Update and render skybox if one is set
	// The skybox needs to be rendered first in the scene since it should be behind all models
	if (m_skybox)
		m_skybox->draw(cam);
		

	// Renders the depth of the scene out of the directional lights position

	//To-do: Fix shadow pass to work with draw call from object
	m_deferredRenderer.beginLightDepthPass(*m_dirLightShadowMap.getDSV());
	dxm->getDeviceContext()->RSSetViewports(1, m_dirLightShadowMap.getViewPort());
	m_depthShader.bind();
	dxm->enableFrontFaceCulling();

	auto& blocks = level.getBlockList();
	for (auto& b : blocks) {
		b->getModel()->setTransform( &b->getTransform() );
		m_depthShader.draw(*b->getModel(), false);
	}
	dxm->enableBackFaceCulling();

// 	OrthographicCamera dl = m_lights.getDirectionalLightCamera();
// 	DirectX::SimpleMath::Vector3 temp = dl.getPosition();
//  	m_rotation += 0.00005f * dt;
// 	temp = DirectX::SimpleMath::Vector3::Transform(temp, DirectX::SimpleMath::Matrix::CreateRotationY(m_rotation));
// 	dl.setPosition(temp);
// 	temp = dl.getDirection();
// 	temp = DirectX::SimpleMath::Vector3::TransformNormal(temp, DirectX::SimpleMath::Matrix::CreateRotationY(m_rotation));
// 	dl.setDirection(temp);
// 	m_lights.setDirectionalLightCamera(dl);
// 	Lights::DirectionalLight _dl;
// 	_dl.direction = dl.getDirection();
// 	//_dl.color = DirectX::SimpleMath::Vector3(0.99f, 0.36f, 0.21f);
// 	m_lights.setDirectionalLight(_dl);
//  	m_depthShader.updateCamera(dl);

	if (m_doPostProcessing) {
		m_deferredRenderer.beginGeometryPass(cam, *m_prePostTex->getRenderTargetView());
	}
	else {
		m_deferredRenderer.beginGeometryPass(cam, *dxm->getBackBufferRTV());
	}

	m_timer.getFrameTime();
	/* draw level here */
	level.draw();
	for (Object* m : m_objects)
		m->draw();
	double time = m_timer.getFrameTime();
// 	std::cout << "Rendering took: " << time * 1000.f << "ms" << std::endl << std::endl;

	// Switch render target to where the deferred output should be
	if (m_doPostProcessing) {
		//m_prePostTex->clear({ 0.f, 0.f, 0.f, 0.0f });
		dxm->getDeviceContext()->OMSetRenderTargets(1, m_prePostTex->getRenderTargetView(), dxm->getDepthStencilView());
	}
	else {
		dxm->renderToBackBuffer();
	}

	// Do the light pass (using additive blending)
	m_deferredRenderer.doLightPass(m_lights, cam, m_dirLightShadowMap);

	// Change active depth buffer to the one used in the deferred geometry pass
	dxm->getDeviceContext()->OMSetRenderTargets(1, dxm->getBackBufferRTV(), m_deferredRenderer.getDSV());

	if (m_doPostProcessing) {
		// Do post processing
		m_postProcOutputTex->clear({ 0.f, 0.f, 0.f, 0.0f });
		m_gaussianBlurShader.draw();

		// Flush post processing effects to back buffer
		m_postProcessfullScreenPlane.draw();
	}

// 	dxm->renderToBackBuffer();

}

void Scene::drawHUD() {
	auto* dxm = Application::getInstance()->getDXManager();

	// 2D rendering stuffs
	// Beginning the spritebatch will disable depth testing
	if (!m_texts.empty()) {
		m_spriteBatch->Begin();
		for (Text* text : m_texts)
			text->draw(m_spriteBatch.get());
		m_spriteBatch->End();
	}

	// Re-enable the depth buffer and rasterizer state after 2D rendering
	dxm->enableDepthBuffer();
	dxm->enableBackFaceCulling();
}

std::map<ShaderSet*, std::vector<Model*>> Scene::mapModelsToShaders(std::vector<Quadtree::Element*>& elements) {

	std::map<ShaderSet*, std::vector<Model*>> mappedModels;

	for (Quadtree::Element* e : elements) {

		ShaderSet* shader = e->model->getShader();

		auto it = mappedModels.find(shader);
		if (it == mappedModels.end())
			mappedModels.insert({ shader,{ e->model } });
		else
			it->second.push_back(e->model);

	}

	return mappedModels;

}

void Scene::setShadowLight() {
	//4096, 2160
	OrthographicCamera dlCam(270.f, 270.f / 1.9f, -105.f, 110.f);
	//OrthographicCamera dlCam(25.f, 25.f / 1.9f, -25.f, 25.f);
	dlCam.setPosition(-m_lights.getDL().direction * 5.f);
	//dlCam.setPosition(DirectX::SimpleMath::Vector3(-110.f, 78.f, -131));
	dlCam.setDirection(m_lights.getDL().direction);
	//dlCam.setDirection(DirectX::SimpleMath::Vector3(0.703f, -0.3f, 0.65f));
	//dlCam.setDirection(DirectX::SimpleMath::Vector3(1.f, 0, 0));
	m_lights.setDirectionalLightCamera(dlCam);

	//Lights::DirectionalLight temp = m_lights.getDL();
	//temp.color = DirectX::SimpleMath::Vector3(0.99f, 0.36f, 0.21f);
	//m_lights.setDirectionalLight(temp);

	m_depthShader.updateCamera(dlCam);
}

void Scene::createFullscreenQuad() {

	DirectX::SimpleMath::Vector2 halfSizes(1.f, 1.f);

	const int numVerts = 4;
	DirectX::SimpleMath::Vector3* positions = new DirectX::SimpleMath::Vector3[numVerts]{
		DirectX::SimpleMath::Vector3(-halfSizes.x, -halfSizes.y, 0.f),
		DirectX::SimpleMath::Vector3(-halfSizes.x, halfSizes.y, 0.f),
		DirectX::SimpleMath::Vector3(halfSizes.x, -halfSizes.y, 0.f),
		DirectX::SimpleMath::Vector3(halfSizes.x, halfSizes.y, 0.f),
	};

	const int numIndices = 6;
	ULONG* indices = new ULONG[numIndices]{
		0, 1, 2, 2, 1, 3
	};

	// Tex coords not used in shader, only set to get rid of warning
	DirectX::SimpleMath::Vector2* texCoords = new DirectX::SimpleMath::Vector2[numVerts]{
		DirectX::SimpleMath::Vector2(0.f, 1.f),
		DirectX::SimpleMath::Vector2(0.f, 0.f),
		DirectX::SimpleMath::Vector2(1.f, 1.f),
		DirectX::SimpleMath::Vector2(1.f, 0.f)
	};

	Model::Data data;
	data.numVertices = numVerts;
	data.numIndices = numIndices;
	data.positions = positions;
	data.indices = indices;
	data.texCoords = texCoords;

	m_postProcessfullScreenPlane.setBuildData(data);
	m_postProcessfullScreenPlane.buildBufferForShader(&m_postProcessFlushShader);
}

Lights& Scene::getLights() {
	return m_lights;
}

DeferredRenderer& Scene::getDeferredRenderer() {
	return m_deferredRenderer;
}

DirLightShadowMap& Scene::getDLShadowMap() {
	return m_dirLightShadowMap;
}

RenderableTexture& Scene::getPreProcessRenderableTexture() {
	return *m_prePostTex.get();
}
