#include "GameState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


GameState::GameState(StateStack& stack)
: State(stack)
, m_cam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
, m_hudCam(1280.f, 720.f, -1.f, 1.f)
, m_camController(&m_cam)
, m_fpsText(&m_font, L"")
, m_debugCamText(&m_font, L"")
, m_flyCam(false)
, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
{
	// Get the Application instance
	m_app = Application::getInstance();

	// Set up handlers
	m_level = std::make_unique<Level>("speedrun.level", m_scene.getDeferredRenderer());
	m_projHandler = std::make_unique<ProjectileHandler>(m_scene.getDeferredRenderer());

	// Set up camera with controllers
	m_cam.setPosition(Vector3(0.f, 5.f, -7.0f));
	Vector2 mapSize = m_level->getGridWorldSize();
	m_playerCamController = std::make_unique<PlayerCameraController>(&m_cam, &mapSize);
	

	// Update the hud shader
	//m_hudShader.updateCamera(m_hudCam);


	// Set up the scene
	m_scene.addSkybox(L"skybox_space_512.dds");
	// Add a directional light
	Vector3 color(0.9f, 0.9f, 0.9f);
 	Vector3 direction(0.4f, -0.6f, 1.0f);
	direction.Normalize();
	m_scene.setUpDirectionalLight(Lights::DirectionalLight(color, direction));

	// Set up HUD texts
	m_debugCamText.setPosition(Vector2(0.f, 20.f));
	// Add texts to the scene
	m_scene.addText(&m_fpsText);
#ifdef _DEBUG
	m_scene.addText(&m_debugCamText);
#endif

	// TODO: move all model loading to a ModelManager
	m_characterModel = std::make_unique<FbxModel>("fisk.fbx");
	m_characterModel->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	m_WeaponModel1 = std::make_unique<FbxModel>("weapon.fbx");
	m_WeaponModel1->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	m_hookModel = std::make_unique<FbxModel>("projectile.fbx");
	m_hookModel->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());
	

	for (int i = 0; i < 4; i++) {
		m_weapons[i] = new Weapon(m_WeaponModel1->getModel(), m_projHandler, i % 2);
		m_player[i] = new Character(m_characterModel->getModel());
		m_hooks[i] = new Hook(m_hookModel->getModel(), m_currLevel->getGrid());
		m_player[i]->setController(1);
		m_player[i]->setControllerPort(i);
		m_player[i]->setWeapon(m_weapons[i]);
		m_player[i]->setCurrentLevel(m_currLevel.get());		
		m_player[i]->setHook(m_hooks[i]);
		m_player[i]->setLightColor(Vector4(0.0f, 0.9f, 1.0f, 1.0f));
		m_scene.addObject(m_player[i]);
	}

	m_playerCamController->setTargets(
		this->m_player[0],
		this->m_player[1],
		this->m_player[2],
		this->m_player[3]
	);
	m_playerCamController->setPosition(this->m_player[0]->getTransform().getTranslation());

	m_playerCamController->setMoveSpeed(5);


	//m_playerCamController.setTargets(
	//	this->player[0],
	//	this->player[1],
	//	nullptr,
	//	nullptr
	//);


}

GameState::~GameState() {
	for (int i = 0; i < 4; i++)
	{
		delete m_weapons[i];
		delete m_player[i];
		delete m_hooks[i];
	}
	delete m_projHandler;
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
			this->m_player[i]->addVibration(0, 1);
			this->m_player[i]->addVibration(1, 1);
			this->m_player[i]->addVibration(2, 1);
			this->m_player[i]->addVibration(3, 1);
		}
	

	for(int i = 0; i < 4; i++) {
		DirectX::GamePad::State& padState = m_app->getInput().gamepadState[this->m_player[i]->getPort()];
		GamePad::ButtonStateTracker& padTracker = gpTracker[this->m_player[i]->getPort()];

		if (padState.IsConnected()) {
			if (padTracker.menu == GamePad::ButtonStateTracker::PRESSED) {
				//requestStackPop();


				auto& pad = m_app->getInput().gamepad;
				for(int u = 0; u < 4; u++)
					pad->SetVibration(u,
						0,
						0,
						0,
						0);


				requestStackPush(States::Pause);
			}

		}



		this->m_player[i]->input(
			padState,
			gpTracker[this->m_player[i]->getPort()],
			m_app->getInput().keyboardState, 
			kbTracker);



	}

	// Update the camera controller from input devices

	if (m_flyCam)
		m_camController.update(dt);


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

	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L"  NumNodes: " + std::to_wstring(Quadtree::numNodes));

	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

	for (int i = 0; i < 4; i++)
		this->m_player[i]->update(dt);


	if(!m_flyCam)
		m_playerCamController->update(dt);
	
	m_projHandler->update(dt);
	
	return true;
}
// Renders the state
bool GameState::render(float dt) {
	// Clear the buffer where the deferred light pass will render to
	m_app->getDXManager()->clear({0.0, 0.0, 0.0, 0.0});
	// Clear back buffer

	// Draw the scene
	// before rendering the final output to the back buffer
	m_scene.draw(dt, m_cam, m_currLevel.get(), m_projHandler);

	//m_app->getDXManager()->enableAlphaBlending();
	//m_colorShader.updateCamera(m_cam);
	//for(int i = 0; i < 4; i++)
	//	m_player[i]->draw();

	//m_projHandler->draw();


	// Draw HUD
	m_scene.drawHUD();

	///* Debug Stuff */
	//m_app->getDXManager()->disableDepthBuffer();
	//m_app->getDXManager()->disableAlphaBlending();
	//m_texturePlane->draw();
	//m_texturePlane2->draw();
	//m_quadtreeCamtexPlane->draw();
	//m_app->getDXManager()->enableDepthBuffer();
	/* Debug Stuff */

	// Swap backbuffers
	//m_app->getDXManager()->present(false);

	return true;
}