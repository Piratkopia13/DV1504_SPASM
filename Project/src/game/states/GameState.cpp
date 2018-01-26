#include "GameState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

GameState::GameState(StateStack& stack)
: State(stack)
, m_cam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
, m_hudCam(1280.f, 720.f, -1.f, 1.f)
, m_camController(&m_cam)
, m_fpsText(&m_font, L"")
, m_debugText(&m_font, L"")
, m_debugCamText(&m_font, L"")
, m_debugParticleText(&m_font, L"")
, m_playerCamController(&m_cam)
, m_flyCam(false)
, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
{

	m_app = Application::getInstance();

	m_currLevel = std::make_unique<Level>("the_void.level", m_scene.getDeferredRenderer());	// Load in textures from file
	m_app->getResourceManager().LoadDXTexture("sand/diffuse.tga");
	m_app->getResourceManager().LoadDXTexture("sand/normal.tga");
	m_app->getResourceManager().LoadDXTexture("sand/specular.tga");
	m_app->getResourceManager().LoadDXTexture("ped_m_sold_aa_hr_diffuse.tga");
	m_app->getResourceManager().LoadDXTexture("ped_m_sold_aa_hr_normal.tga");
	m_app->getResourceManager().LoadDXTexture("ped_m_sold_aa_hr_specular.tga");
	m_app->getResourceManager().LoadDXTexture("grass.tga");
	m_app->getResourceManager().LoadDXTexture("shrine/diffuse.tga");
	m_app->getResourceManager().LoadDXTexture("shrine/normal.tga");
	

	// Update the hud shader
	m_hudShader.updateCamera(m_hudCam);

	m_cam.setPosition(Vector3(0.f, 5.f, -7.0f));

	m_timer.startTimer();

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


	m_matShader.updateLights(m_scene.getLights());


	Vector3 halfSizes(0.5f, 0.5f, 0.5f);

	/* Planes for debugging */
	float windowWidth = (float)m_app->getWindow()->getWindowWidth();
	float windowHeight = (float)m_app->getWindow()->getWindowHeight();
	float texPlaneHeight = windowHeight / 2.5f;
	Vector2 texPlaneHalfSize(texPlaneHeight / 2.f * (windowWidth / windowHeight), texPlaneHeight / 2.f);
	m_texturePlane = ModelFactory::PlaneModel::Create(texPlaneHalfSize);
	m_texturePlane->buildBufferForShader(&m_hudShader);
	//m_texturePlane->getMaterial()->setTextures(m_scene.getDLShadowMap().getSRV(), 1);
	//m_texturePlane->getMaterial()->setTextures(m_scene.getPreProcessRenderableTexture().getColorSRV(), 1);
	m_texturePlane->getMaterial()->setTextures(m_scene.getDeferredRenderer().getGBufferSRV(DeferredRenderer::DIFFUSE_GBUFFER), 1);
	m_texturePlane->getTransform().rotateAroundX(-XM_PIDIV2);
	m_texturePlane->getTransform().rotateAroundY(XM_PI);
	m_texturePlane->getTransform().translate(Vector3(-windowWidth / 2.f + texPlaneHalfSize.x, 0.f, -windowHeight / 2.f + texPlaneHalfSize.y));

	m_texturePlane2 = ModelFactory::PlaneModel::Create(texPlaneHalfSize);
	m_texturePlane2->buildBufferForShader(&m_hudShader);
	m_texturePlane2->getMaterial()->setTextures(m_scene.getDeferredRenderer().getGBufferSRV(DeferredRenderer::DEPTH_GBUFFER), 1);
	m_texturePlane2->getTransform().rotateAroundX(-XM_PIDIV2);
	m_texturePlane2->getTransform().rotateAroundY(XM_PI);
	m_texturePlane2->getTransform().translate(Vector3(windowWidth / 2.f - texPlaneHalfSize.x, 0.f, -windowHeight / 2.f + texPlaneHalfSize.y));
	/* Planes for debugging */

	m_debugCamText.setPosition(Vector2(0.f, 20.f));
	m_debugText.setPosition(Vector2(0.f, 40.f));

	// Add texts to the scene
	m_scene.addText(&m_fpsText);
	m_scene.addText(&m_debugText);
	m_scene.addText(&m_debugCamText);
	m_scene.addText(&m_debugParticleText);

	m_characterModel = std::make_unique<FbxModel>("spasm.fbx");
	m_characterModel->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	m_WeaponModel1 = std::make_unique<FbxModel>("weapon.fbx");
	m_WeaponModel1->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	

	for (int i = 0; i < 4; i++) {
		this->weapons[i] = new Weapon(m_WeaponModel1->getModel(), i % 2);
		this->player[i] = new Character(m_characterModel->getModel());
		this->player[i]->setController(1);
		this->player[i]->setControllerPort(i);
		this->player[i]->setWeapon(this->weapons[i]);

		m_scene.addObject(player[i]);
	}
}

GameState::~GameState() {
	for (int i = 0; i < 4; i++)
	{
		delete weapons[i];
		delete player[i];
	}
}

// Process input for the state
bool GameState::processInput(float dt) {

	static Keyboard::KeyboardStateTracker kbTracker;
	static GamePad::ButtonStateTracker gpTracker[4];
	for(int i = 0; i < 4; i++)
		gpTracker[i].Update(m_app->getInput().gamepadState[i]);
	kbTracker.Update(m_app->getInput().keyboardState);



	// Toggle camera controller on 'F' key or 'Y' btn
	if (kbTracker.pressed.F)
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
	}


	
	if(kbTracker.pressed.Q)
		for (int i = 0; i < 4; i++) {
			this->player[i]->addVibration(0, 1);
			this->player[i]->addVibration(1, 1);
			this->player[i]->addVibration(2, 1);
			this->player[i]->addVibration(3, 1);
		}
	

	for(int i = 0; i < 4; i++)
		this->player[i]->input(
			m_app->getInput().gamepadState[this->player[i]->getPort()], 
			gpTracker[this->player[i]->getPort()], 
			m_app->getInput().keyboardState, 
			kbTracker);


	// Update the camera controller from input devices
	if (m_flyCam)
		m_camController.update(dt);
	else
		m_playerCamController.update(dt, *player[0]);


	return true;
}

// Process window resizing for the state
bool GameState::resize(int width, int height) {
	m_cam.resize(width, height);
	m_scene.resize(width, height);

	return true;
}


// Updates the state
bool GameState::update(float dt) {


	static float counter = 0.f;
	counter += dt;

	//m_cube->getModel()->getTransform().rotateAroundY(0.005f);

	//m_plane->getTransform().rotateAroundY(0.005f);

	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));

	auto& cPos = m_cam.getPosition();

	std::wstring flying(L"Flying (shift for boost, rmouse to toggle cursor)");
	std::wstring Walking(L"Walking (rmouse to toggle cursor)");
	m_debugText.setText(std::wstring(L"Cam controller (F to toggle): ") + ((m_flyCam) ? flying : Walking));
	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L"  NumNodes: " + std::to_wstring(Quadtree::numNodes));

	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

	for (int i = 0; i < 4; i++)
		this->player[i]->update(dt);



	return true;
}
// Renders the state
bool GameState::render(float dt) {
	// Clear the buffer where the deferred light pass will render to
	m_app->getDXManager()->clear(DirectX::Colors::Teal);
	// Clear back buffer

	// Draw the scene
	// before rendering the final output to the back buffer
	m_scene.draw(dt, m_cam, *m_currLevel.get());

	//m_app->getDXManager()->enableAlphaBlending();
	/*m_colorShader.updateCamera(m_cam);
	for(int i = 0; i < 4; i++)
		player[i]->draw();*/

	// Draw HUD
	//m_scene.drawHUD();

	///* Debug Stuff */
	//m_app->getDXManager()->disableDepthBuffer();
	//m_app->getDXManager()->disableAlphaBlending();
	//m_texturePlane->draw();
	//m_texturePlane2->draw();
	//m_quadtreeCamtexPlane->draw();
	//m_app->getDXManager()->enableDepthBuffer();
	/* Debug Stuff */

	// Swap backbuffers
	m_app->getDXManager()->present(false);

	return true;
}