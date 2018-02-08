#include "GameState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


GameState::GameState(StateStack& stack)
: State(stack)
, m_cam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
, m_camController(&m_cam)
, m_fpsText(&m_font, L"")
, m_debugCamText(&m_font, L"")
, m_flyCam(false)
, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
{
	// Get the Application instance
	m_app = Application::getInstance();
	Application::GameSettings* settings = &m_app->getGameSettings();

	// Set up handlers
	m_level = std::make_unique<Level>("asd.level");
	m_projHandler = std::make_unique<ProjectileHandler>();
	m_characterHandler = std::make_unique<CharacterHandler>(m_projHandler.get());
	m_collisionHandler = std::make_unique <CollisionHandler>(m_level.get(), m_characterHandler.get(), m_projHandler.get());

	// Set up camera with controllers
	m_cam.setPosition(Vector3(0.f, 5.f, -7.0f));
	Vector2 mapSize = m_level->getGridWorldSize();
	m_playerCamController = std::make_unique<PlayerCameraController>(&m_cam, &mapSize);
	
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

	// Set character spawn points
	m_characterHandler->addSpawnPoint(1, Vector3(2, 2, 0));
	m_characterHandler->addSpawnPoint(1, Vector3(3, 2, 0));
	m_characterHandler->addSpawnPoint(2, Vector3(10, 2, 0));
	m_characterHandler->addSpawnPoint(2, Vector3(11, 2, 0));

	// Add the characters for rendering and respawn them
	for (size_t i = 0; i < m_characterHandler->getNrOfPlayers(); i++) {
		m_scene.addObject(m_characterHandler->getCharacter(i));
		m_characterHandler->respawnPlayer(i);
	}

	// Give the cam controller targets to follow
	m_playerCamController->setTargets(
		m_characterHandler->useableTarget(0) ? m_characterHandler->getCharacter(0) : nullptr,
		m_characterHandler->useableTarget(1) ? m_characterHandler->getCharacter(1) : nullptr,
		m_characterHandler->useableTarget(2) ? m_characterHandler->getCharacter(2) : nullptr,
		m_characterHandler->useableTarget(3) ? m_characterHandler->getCharacter(3) : nullptr
	);

	m_playerCamController->setPosition(Vector3(10, 10, 0));
}

GameState::~GameState() {

}

// Process input for the state
bool GameState::processInput(float dt) {

	const Keyboard::KeyboardStateTracker& kbTracker = m_app->getInput().getKbStateTracker();
	auto& gamePad = m_app->getInput().getGamePad();

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
	}

	if (kbTracker.pressed.T) {
		m_characterHandler->killPlayer(0);
	}


	// Pause menu
	m_app->getInput().processAllGamepads([&](auto& state, auto& tracker) {
		if (tracker.menu == GamePad::ButtonStateTracker::PRESSED) {
			for (int u = 0; u < 4; u++)
				gamePad.SetVibration(u, 0, 0);
			requestStackPush(States::Pause);
		}
	});
	/*
		for(size_t i = 0; i < m_characterHandler->getNrOfPlayers(); i++) {
			int port = m_characterHandler->getCharacter(i)->getPort();

			const GamePad::State& padState = m_app->getInput().getGamePadState(i);
			const GamePad::ButtonStateTracker& padTracker = m_app->getInput().getGpStateTracker(i);

			if (padState.IsConnected()) {
				if (padTracker.menu == GamePad::ButtonStateTracker::PRESSED) {
					for(int u = 0; u < 4; u++)
						gamePad.SetVibration(u, 0,	0);
					requestStackPush(States::Pause);
				}
			}
		}*/
	
	m_characterHandler->processInput();

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

	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));

	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

	m_characterHandler->update(dt);

	if(!m_flyCam)
		m_playerCamController->update(dt);
	
	m_projHandler->update(dt);
	

	m_playerCamController->setTargets(
		m_characterHandler->useableTarget(0) ? m_characterHandler->getCharacter(0) : nullptr,
		m_characterHandler->useableTarget(1) ? m_characterHandler->getCharacter(1) : nullptr,
		m_characterHandler->useableTarget(2) ? m_characterHandler->getCharacter(2) : nullptr,
		m_characterHandler->useableTarget(3) ? m_characterHandler->getCharacter(3) : nullptr
	);



	return true;
}
// Renders the state
bool GameState::render(float dt) {

	// Clear back buffer
	m_app->getDXManager()->clear({0.0, 0.0, 0.0, 0.0});

	// Draw the scene using deferred rendering
	m_scene.draw(dt, m_cam, m_level.get(), m_projHandler.get());

	// Draw HUD
	m_scene.drawHUD();

	return true;
}