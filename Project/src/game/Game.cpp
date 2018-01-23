#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Game::Game(HINSTANCE hInstance)
	: Application(1280, 720, "3D Project", hInstance)
	, m_cam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
	, m_quadtreeCam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
	, m_hudCam(1280.f, 720.f, -1.f, 1.f)
	, m_camController(&m_cam)
	, m_fpsText(&m_font, L"")
	, m_debugText(&m_font, L"")
	, m_debugCamText(&m_font, L"")
	, m_debugParticleText(&m_font, L"")
	, m_playerCamController(&m_cam)
	, m_flyCam(true)
	, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
{

	// Load in textures from file
	getResourceManager().LoadDXTexture("sand/diffuse.tga");
	getResourceManager().LoadDXTexture("sand/normal.tga");
	getResourceManager().LoadDXTexture("sand/specular.tga");
	getResourceManager().LoadDXTexture("ped_m_sold_aa_hr_diffuse.tga");
	getResourceManager().LoadDXTexture("ped_m_sold_aa_hr_normal.tga");
	getResourceManager().LoadDXTexture("ped_m_sold_aa_hr_specular.tga");
	getResourceManager().LoadDXTexture("grass.tga");
	getResourceManager().LoadDXTexture("shrine/diffuse.tga");
	getResourceManager().LoadDXTexture("shrine/normal.tga");

	// Update the hud shader
	m_hudShader.updateCamera(m_hudCam);

	m_cam.setPosition(Vector3(0.f, 5.f, -7.0f));	

	// Init terrain
	m_terrain = std::make_unique<Terrain>("terrain.tga", &m_scene.getDeferredRenderer().getGeometryShader(), 200, 20.f);
	m_terrain->getModel().getMaterial()->setDiffuseTexture("sand/diffuse.tga");
	m_terrain->getModel().getMaterial()->setNormalTexture("sand/normal.tga");
	m_terrain->getModel().getMaterial()->setSpecularTexture("sand/specular.tga");
	m_terrain->getModel().getTransform().translate(Vector3(-100.f, 0.f, 100.f));
	m_terrain->getModel().getMaterial()->setKs(1.f);
	m_terrain->getModel().getMaterial()->setShininess(10.f);

	//m_tessTerrainGrass = std::make_unique<Terrain>("terrain.tga", &m_tessShader, 200, 20.f);
	//m_tessTerrainGrass->getModel().getMaterial()->setDiffuseTexture("grass.tga");
	//m_tessTerrainGrass->getModel().getTransform().translate(Vector3(-100.f, 0.f, 100.f));

	m_timer.startTimer();

	// Add water to the scene
	m_scene.addWater(1.0f);

	// Add skybox to the scene
	m_scene.addSkybox(L"skybox2_512.dds");

	auto& l = m_scene.getLights();
	auto dl = l.getDL();
	dl.color = Vector3(0.9f, 0.9f, 0.9f);
	dl.direction = Vector3(0.4f, -0.6f, 1.0f);
	//dl.direction = Vector3(0.f, -1.f, 0.f);
	dl.direction.Normalize();
	l.setDirectionalLight(dl);

	m_scene.setShadowLight();

	Lights::PointLight pl;
	pl.setColor(Vector3(0.1f, 0.9f, 0.1f));
	pl.setPosition(Vector3(0.f, 4.f, -2.f));
	pl.setAttenuation(1.f, 1.f, 1.f);
	l.addPointLight(pl);

	// Deferred renderer testing
	// Place a bunch of random positioned lights on the terrain
	//for (UINT i = 0; i < 500; i++) {
	//	float x = Utils::rnd() * 200.f - 100.f;
	//	float z = Utils::rnd() * 200.f - 100.f;
	//	Lights::PointLight pl;
	//	pl.setColor(Vector3(Utils::rnd(), Utils::rnd(), Utils::rnd()));
	//	//pl.setColor(Vector3(1.f, 1.f, 1.f));
	//	pl.setPosition(Vector3(x, m_terrain->getHeightAtWorldCoords(x, z) + 1.0f, z));
	//	pl.setAttenuation(0.01f, 0.01f, 0.2f);
	//	l.addPointLight(pl);
	//}


	m_matShader.updateLights(m_scene.getLights());

	auto water = m_scene.getWater();
	if (water)
		water->getShader()->updateLights(m_scene.getLights());


	Vector3 halfSizes(0.5f, 0.5f, 0.5f);
	m_tessSphere = std::make_unique<FbxModel>("sphere.fbx");
	m_tessSphere->getModel()->buildBufferForShader(&m_tessShader);
	m_tessSphere->getModel()->getMaterial()->setDiffuseTexture("grass.tga");
	m_tessSphere->getModel()->getTransform().translate(Vector3(-2.f, 2.f, 0.f));
	m_tessSphere->getModel()->getTransform().scaleUniformly(2.f);
	m_tessSphere->getModel()->getMaterial()->setKs(0.f);

	m_sphere = std::make_unique<FbxModel>("sphere.fbx");
	m_sphere->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());
	m_sphere->getModel()->getMaterial()->setDiffuseTexture("grass.tga");
	m_sphere->getModel()->getTransform().translate(Vector3(-2.f, 2.f, 0.f));
	m_sphere->getModel()->getTransform().scaleUniformly(2.f);


	m_plane = ModelFactory::PlaneModel::Create(Vector2(1.0f, 1.0f));
	m_plane->getMaterial()->setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_plane->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());
	m_plane->getTransform().translate(Vector3(0.f, 3.f, -2.f));

	float windowWidth = (float)getWindow()->getWindowWidth();
	float windowHeight = (float)getWindow()->getWindowHeight();
	float texPlaneHeight = windowHeight / 2.5f;
	Vector2 texPlaneHalfSize(texPlaneHeight / 2.f * (windowWidth/windowHeight), texPlaneHeight / 2.f);
	m_texturePlane = ModelFactory::PlaneModel::Create(texPlaneHalfSize);
	m_texturePlane->buildBufferForShader(&m_hudShader);
	m_texturePlane->getMaterial()->setTextures(m_scene.getDLShadowMap().getSRV(), 1);
	m_texturePlane->getTransform().rotateAroundX(-XM_PIDIV2);
	m_texturePlane->getTransform().rotateAroundY(XM_PI);
	m_texturePlane->getTransform().translate(Vector3(-windowWidth / 2.f + texPlaneHalfSize.x, 0.f, -windowHeight / 2.f + texPlaneHalfSize.y));

	m_texturePlane2 = ModelFactory::PlaneModel::Create(texPlaneHalfSize);
	m_texturePlane2->buildBufferForShader(&m_hudShader);
	m_texturePlane2->getMaterial()->setTextures(m_scene.getDeferredRenderer().getGBufferSRV(DeferredRenderer::DEPTH_GBUFFER), 1);
	m_texturePlane2->getTransform().rotateAroundX(-XM_PIDIV2);
	m_texturePlane2->getTransform().rotateAroundY(XM_PI);
	m_texturePlane2->getTransform().translate(Vector3(windowWidth / 2.f - texPlaneHalfSize.x, 0.f, -windowHeight / 2.f + texPlaneHalfSize.y));

	m_quadtreeCamtexPlane = ModelFactory::PlaneModel::Create(texPlaneHalfSize);
	m_quadtreeCamtexPlane->buildBufferForShader(&m_hudShader);
	m_quadtreeCamtexPlane->getMaterial()->setDiffuseTexture(*m_quadtreeCamTex.getColorSRV());
	m_quadtreeCamtexPlane->getTransform().rotateAroundX(-XM_PIDIV2);
	m_quadtreeCamtexPlane->getTransform().rotateAroundY(XM_PI);
	m_quadtreeCamtexPlane->getTransform().translate(Vector3(windowWidth / 2.f - texPlaneHalfSize.x, 0.f, -windowHeight / 2.f + texPlaneHalfSize.y));


	m_fbxModel = std::make_unique<FbxModel>("soldier_disguise.fbx");
	m_fbxModel->getModel()->getMaterial()->setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_fbxModel->getModel()->getMaterial()->setDiffuseTexture("ped_m_sold_aa_hr_diffuse.tga");
	m_fbxModel->getModel()->getMaterial()->setNormalTexture("ped_m_sold_aa_hr_normal.tga");
	m_fbxModel->getModel()->getMaterial()->setSpecularTexture("ped_m_sold_aa_hr_specular.tga");
	m_fbxModel->getModel()->buildBufferForShader(&m_tessShader);
	m_fbxModel->getModel()->getTransform().setScale(0.1f);
	Vector2 fbxPos(0.f, 0.f);
	m_fbxModel->getModel()->getTransform().setTranslation(Vector3(fbxPos.x, m_terrain->getHeightAtWorldCoords(fbxPos.x / 10.f, fbxPos.y) * 10.f, fbxPos.y));
	//m_fbxModel->getModel()->getTransform().rotateAroundY(XM_PI);

	// Create a bunch of copies of the fbxmodel for quadtree performance testing
	/*Model::Data copiedBuildData = m_fbxModel->getModel()->getBuildData();
	for (int y = 0; y < 20; y++) {
		for (int x = 0; x < 50; x++) {
			m_modelCopies.push_back(new Model(copiedBuildData));
			m_modelCopies.back()->getMaterial()->setDiffuseTexture("ped_m_sold_aa_hr_diffuse.tga");
			m_modelCopies.back()->getMaterial()->setNormalTexture("ped_m_sold_aa_hr_normal.tga");
			m_modelCopies.back()->getMaterial()->setSpecularTexture("ped_m_sold_aa_hr_specular.tga");
			m_modelCopies.back()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());
			m_modelCopies.back()->getTransform().rotateAroundY(DirectX::XM_PI);
			m_modelCopies.back()->getTransform().setScale(0.1f);
			m_modelCopies.back()->getTransform().setTranslation(Vector3(10 * x + 0, 10 * y, 0.f));
			m_modelCopies.back()->updateAABB();
			m_scene.addModelViaQuadtree(m_modelCopies.back());
		}
	}*/

	m_particleSystem = std::make_unique<BillboardSystem>(m_particleShader, "smoke_white.tga");
	m_particleSystem->setEmitterLocation(Vector3(10.f, 10.f, 0.f));

	m_debugCamText.setPosition(Vector2(0.f, 20.f));
	m_debugText.setPosition(Vector2(0.f, 40.f));
	m_debugParticleText.setPosition(Vector2(0.f, 60.f));

	// Add models to the scene
	m_terrain->getModel().updateAABB();
	m_scene.addModelViaQuadtree(&m_terrain->getModel());
	m_plane->updateAABB();
	m_scene.addModelViaQuadtree(m_plane.get());
	m_sphere->getModel()->updateAABB();
	m_scene.addModelViaQuadtree(m_sphere->getModel());
	//m_fbxModel->getModel()->updateAABB();
	//m_scene.addModelViaQuadtree(m_fbxModel->getModel());

	// Add texts to the scene
	m_scene.addText(&m_fpsText);
	m_scene.addText(&m_debugText);
	m_scene.addText(&m_debugCamText);
	m_scene.addText(&m_debugParticleText);

	std::wcout << L"Terrain min: " << Utils::vec3ToWStr(m_terrain->getModel().getAABB().getMinPos()) << L"  max: " << Utils::vec3ToWStr(m_terrain->getModel().getAABB().getMaxPos()) << std::endl;

}

Game::~Game() {	}

int Game::run() {
	
	// Start the game loop and return when game exits
	return startGameLoop();

}

void Game::resize(int width, int height) {
	m_cam.resize(width, height);
	m_scene.resize(width, height);

	// Update textureplane SRVs since they got resized
	//m_texturePlane->getMaterial()->setDiffuseTexture(*m_scene.getWater()->getReflection()->getColorSRV());
	//m_texturePlane2->getMaterial()->setDiffuseTexture(*m_scene.getWater()->getRefraction()->getColorSRV());
}

void Game::processInput(float dt) {

	static Keyboard::KeyboardStateTracker kbTracker;
	static GamePad::ButtonStateTracker gpTracker;
	kbTracker.Update(getInput().keyboardState);
	gpTracker.Update(getInput().gamepadState);

	// Toggle camera controller on 'F' key or 'Y' btn
	if (kbTracker.pressed.F || gpTracker.y == gpTracker.PRESSED)
		m_flyCam = !m_flyCam;

	// Add red point light at camera pos
	if (kbTracker.pressed.E) {
		Lights::PointLight pl;
		pl.setColor(Vector3(Utils::rnd(), Utils::rnd(), Utils::rnd()));
		//pl.setColor(Vector3(1.f, 1.f, 1.f));
		pl.setPosition(m_cam.getPosition());
		pl.setAttenuation(.0f, 0.1f, 0.02f);
		m_scene.getLights().addPointLight(pl);

		m_matShader.updateLights(m_scene.getLights());
		if (m_scene.getWater())
			m_scene.getWater()->getShader()->updateLights(m_scene.getLights());
	}

	if (kbTracker.pressed.C) {
		Vector3 halfSizes(.2f, .2f, .2f);
		auto model = ModelFactory::CubeModel::Create(halfSizes);
		model->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());
		model->getTransform().setTranslation(m_cam.getPosition());
		model->updateAABB();
		models.push_back(std::move(model));

		m_scene.addModelViaQuadtree(models.back().get());
	}

#ifdef _DEBUG
	if (kbTracker.pressed.H) {
		auto nodes = m_scene.getQuadtree().getRoot().getNodesAsModels();
		for (auto* model : nodes) {
			model->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());
			model->getMaterial()->setColor(Vector4(Utils::rnd(), Utils::rnd(), Utils::rnd(), 0.3f));
			m_scene.addModelViaQuadtree(model);
		}
	}
#endif

	// Update the camera controller from input devices
	if (m_flyCam)
		m_camController.update(dt);
	else
		m_playerCamController.update(dt);

}

void Game::update(float dt) {

	static float counter = 0.f;
	counter += dt;
	
	//m_cube->getModel()->getTransform().rotateAroundY(0.005f);

	m_fbxModel->getModel()->getTransform().rotateAroundY(0.005f * dt);
	//m_plane->getTransform().rotateAroundY(0.005f);

	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(getFPS()));
	
	auto& cPos = m_cam.getPosition();
	m_quadtreeCam.setPosition(Vector3(cPos.x, cPos.y + 15.f, cPos.z));
	m_quadtreeCam.setDirection(Vector3(.1f, -1.f, .1f));

	std::wstring flying(L"Flying (shift for boost, rmouse to toggle cursor)");
	std::wstring Walking(L"Walking (rmouse to toggle cursor)");
	m_debugText.setText(std::wstring(L"Cam controller (F to toggle): ") + ((m_flyCam)? flying : Walking));
	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L"  NumNodes: " + std::to_wstring(Quadtree::numNodes));

	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

	m_debugParticleText.setText(L"Amount of particles in system: " + std::to_wstring(m_particleSystem->getNumParticles()));

	if (counter >= 0.01f) {
		float rnd1 = Utils::rnd() * 2.f - 1.f, rnd2 = Utils::rnd() * 2.f - 1.f, rnd3 = Utils::rnd() * 2.f - 1.f;
		DirectX::SimpleMath::Vector3 rndVec = DirectX::SimpleMath::Vector3(rnd1, rnd2, rnd3);
		rndVec.Normalize();
		rndVec *= 5.f;
		m_particleSystem->setRandEmitVector(rndVec);
		m_particleSystem->emitBillboards(1);
		counter = 0.f;
	}
	m_particleSystem->update();

}

void Game::render(float dt) {


	// Clear the buffer where the deferred light pass will render to
	getDXManager()->clear(DirectX::Colors::Teal);
	// Clear back buffer

	// Draw the scene
	// This will draw multiple passes (for water)
	// before rendering the final output to the back buffer
	m_scene.draw(dt, m_cam);

	getDXManager()->disableFaceCulling();
	m_tessShader.updateCamera(m_cam);
	m_tessSphere->getModel()->draw(true);
	m_fbxModel->getModel()->draw(true);
	getDXManager()->enableBackFaceCulling();

	m_particleShader.updateCamera(m_cam);
	getDXManager()->enableAlphaBlending();
	//getDXManager()->enableAdditiveBlending();
	getDXManager()->disableDepthBuffer();
	m_particleShader.draw(true);
	getDXManager()->enableDepthBuffer();

	// Draw HUD
	m_scene.drawHUD();

	//getDXManager()->disableDepthBuffer();
	//getDXManager()->disableAlphaBlending();
	//m_texturePlane->draw();
	//m_texturePlane2->draw();
	//m_quadtreeCamtexPlane->draw();
	//getDXManager()->enableDepthBuffer();

	// Swap backbuffers
	getDXManager()->present(false);

}