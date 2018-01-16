#include "Water.h"
#include "shader/basic/WaterShader.h"
#include "Scene.h"

using namespace std;
using namespace DirectX;
using namespace SimpleMath;

Water::Water(float height, WaterShader* waterShader)
	: m_height(height)
	// The reflection will be half the size of the backbuffer for performance reasons
	, m_reflection(Application::getInstance()->getDXManager()->getAASamples(), Application::getInstance()->getWindow()->getWindowWidth(), Application::getInstance()->getWindow()->getWindowHeight())
	// The refraction needs to be the same size as the backbuffer to avoid white pixels on water edges
	, m_refraction(1, Application::getInstance()->getWindow()->getWindowWidth(), Application::getInstance()->getWindow()->getWindowHeight())
	, m_waterShader(waterShader)
	, m_dudvMap("water/dudv.tga")
	, m_normalMap("water/normal.tga")
{

	Vector2 halfSizes(100.f, 100.f);
	m_waterPlane = ModelFactory::PlaneModel::Create(halfSizes);
	m_waterPlane->buildBufferForShader(waterShader);
	m_waterPlane->getTransform().translate(Vector3(0.f, m_height, 0.f));
	// Set up shader resource views to match the shader
	m_srvs[0] = *m_reflection.getColorSRV();
	m_srvs[1] = *m_refraction.getDepthSRV();
	m_srvs[2] = *m_dudvMap.getResourceView();
	m_srvs[3] = *m_normalMap.getResourceView();
	m_waterPlane->getMaterial()->setTextures(m_srvs, 4);

}
Water::~Water() {

}

void Water::updateWaves(float dt) {
	m_waterShader->updateWaves(dt);
}

void Water::drawWaterTexture(Scene* scene, Camera& cam, Skybox* skybox, DirLightShadowMap& dirLight) {

	auto* dxm = Application::getInstance()->getDXManager();
	dxm->enableDepthBuffer();

	m_waterShader->updateLights(scene->getLights());

	beginReflectionRendering(cam);

	// Draw the skybox using forward rendering
	if (skybox)
		skybox->draw(cam);
	
	// Draw all models from the quadtree using deferred rendering
	dxm->enableDepthBuffer();
	dxm->disableAlphaBlending();
	scene->getDeferredRenderer().beginGeometryPass(cam, *m_reflection.getRenderTargetView());

	// Set the clipping plane to only render things above water height
	scene->getDeferredRenderer().getGeometryShader().setClippingPlane(Vector4(0.f, 1.f, 0.f, -getHeight() + 0.01f));

	std::vector<Model*> toBeDrawn = scene->getQuadtree().getRoot().query(cam.getFrustum());
	for (Model* m : toBeDrawn)
		m->draw(false);

	// Remove any clipping planes now, since we are done rendering refractions with this shader
	scene->getDeferredRenderer().getGeometryShader().setClippingPlane(Vector4::Zero);


	// Switch render target to where we want the output
	m_reflection.begin();

	scene->getDeferredRenderer().doLightPass(scene->getLights(), cam, dirLight);

	endReflectionRendering(cam);

	beginRefractionRendering();

	// Draw all models from the quadtree using deferred rendering
	dxm->enableDepthBuffer();
	dxm->disableAlphaBlending();
	scene->getDeferredRenderer().beginGeometryPass(cam, *m_refraction.getRenderTargetView(), *m_refraction.getDepthStencilView());

	// Set the clipping plane to only render things above water height
	scene->getDeferredRenderer().getGeometryShader().setClippingPlane(Vector4(0.f, -1.f, 0.f, getHeight() + 0.01f));

	toBeDrawn = scene->getQuadtree().getRoot().query(cam.getFrustum());
	for (Model* m : toBeDrawn)
		m->draw(false);

	// Remove any clipping planes now, since we are done rendering refractions with this shader
	scene->getDeferredRenderer().getGeometryShader().setClippingPlane(Vector4::Zero);

	// Switch render target to where we want the output
	m_refraction.begin();
	// Skip the light pass since only the depth part of the refraction texture is used
	//scene->getDeferredRenderer().doLightPass(scene->getLights());

	endRefractionRendering();

	// Revert render target to the back buffer
	dxm->renderToBackBuffer();

}

void Water::beginReflectionRendering(Camera& cam) {

	m_waterShader->updateCamera(cam);
	m_reflection.begin();
	m_reflection.clear(DirectX::Colors::Teal);

	m_oldCamPos = cam.getPosition();
	m_oldCamDirection = cam.getDirection();

	// Project direction onto up vector and use that to get the invert pitch
	DirectX::SimpleMath::Vector3 direction = m_oldCamDirection - 2.f * m_oldCamDirection.Dot(DirectX::SimpleMath::Vector3::Up)*DirectX::SimpleMath::Vector3::Up;
	float heightAboveWater = m_oldCamPos.y - m_height;
	cam.setPosition(Vector3(m_oldCamPos.x, m_oldCamPos.y - 2.f * heightAboveWater, m_oldCamPos.z));
	cam.setDirection(direction);

}
void Water::endReflectionRendering(Camera& cam) {

	cam.setPosition(m_oldCamPos);
	cam.setDirection(m_oldCamDirection);
	m_reflection.end();

}

void Water::beginRefractionRendering() {

	m_refraction.begin();
	m_refraction.clear(DirectX::Colors::Teal);

}
void Water::endRefractionRendering() {

	m_refraction.end();

}

void Water::resize(int width, int height) {
	// The reflection will be half the size of the backbuffer for performance reasons
	m_reflection.resize(width, height);
	// The refraction needs to be the same size as the backbuffer to avoid white pixels on water edges
	m_refraction.resize(width, height);

	// Update SRVs in our array since they got destroyed and remade during resizing
	m_srvs[0] = *m_reflection.getColorSRV();
	m_srvs[1] = *m_refraction.getDepthSRV();
}

Model& Water::getModel() {
	return *m_waterPlane;
}

RenderableTexture* Water::getReflection() {
	return &m_reflection;
}

RenderableTexture* Water::getRefraction() {
	return &m_refraction;
}

float Water::getHeight() const {
	return m_height;
}

WaterShader* Water::getShader() const {
	return m_waterShader;
}
