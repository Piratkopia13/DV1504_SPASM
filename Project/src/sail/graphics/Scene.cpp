#include "Scene.h"
#include "../../game/objects/common/Object.h"

using namespace std;

Scene::Scene(const AABB& worldSize)
	:
	//m_dirLightShadowMap(16384, 8640),
	m_dirLightShadowMap(8192, 4320)
	//, m_dirLightShadowMap(4096, 2160)
{
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(Application::getInstance()->getDXManager()->getDeviceContext());
	m_timer.startTimer();

	// Camera rotation
	m_rotation = 0.f;

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
	m_deferredRenderer.resize(width, height);
}

// Draws the scene
void Scene::draw(float dt, Camera& cam, Level& level) {

	auto* dxm = Application::getInstance()->getDXManager();

	// Update and render skybox if one is set
	// The skybox needs to be rendered first in the scene since it should be behind all models
	if (m_skybox)
		m_skybox->draw(cam);
		

	// Renders the depth of the scene out of the directional lights position

	//To-do: Fix shadow pass to work with draw call from object
	/*m_deferredRenderer.beginLightDepthPass(*m_dirLightShadowMap.getDSV());
	dxm->getDeviceContext()->RSSetViewports(1, m_dirLightShadowMap.getViewPort());
	m_depthShader.bind();
	dxm->enableFrontFaceCulling();
	for (Object* m : m_objects)
		m->draw();
	dxm->enableBackFaceCulling();*/

	//OrthographicCamera dl = m_lights.getDirectionalLightCamera();
	//DirectX::SimpleMath::Vector3 temp = dl.getPosition();
	////m_rotation += 0.00000005f * dt;
	//temp = DirectX::SimpleMath::Vector3::Transform(temp, DirectX::SimpleMath::Matrix::CreateRotationY(m_rotation));
	//dl.setPosition(temp);
	//temp = dl.getDirection();
	//temp = DirectX::SimpleMath::Vector3::TransformNormal(temp, DirectX::SimpleMath::Matrix::CreateRotationY(m_rotation));
	//dl.setDirection(temp);
	//m_lights.setDirectionalLightCamera(dl);
	//Lights::DirectionalLight _dl;
	//_dl.direction = dl.getDirection();
	////_dl.color = DirectX::SimpleMath::Vector3(0.99f, 0.36f, 0.21f);
	//m_lights.setDirectionalLight(_dl);
	//m_depthShader.updateCamera(dl);

	m_deferredRenderer.beginGeometryPass(cam, *dxm->getBackBufferRTV());

	m_timer.getFrameTime();
	/* draw level here */
	level.draw();
	for (Object* m : m_objects)
		m->draw();
	double time = m_timer.getFrameTime();
	//std::cout << "Rendering took: " << time * 1000.f << "ms" << std::endl << std::endl;

	// Switch render target to where the deferred output should be
	dxm->renderToBackBuffer();

	// Do the light pass
	m_deferredRenderer.doLightPass(m_lights, cam, m_dirLightShadowMap);

	// Change active depth buffer to the one used in the deferred geometry pass
	dxm->getDeviceContext()->OMSetRenderTargets(1, dxm->getBackBufferRTV(), m_deferredRenderer.getDSV());

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

Lights& Scene::getLights() {
	return m_lights;
}

DeferredRenderer& Scene::getDeferredRenderer() {
	return m_deferredRenderer;
}

DirLightShadowMap& Scene::getDLShadowMap() {
	return m_dirLightShadowMap;
}