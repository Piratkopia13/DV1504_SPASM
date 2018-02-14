#include "GameState.h"
#include "../objects/Block.h"
#include "../../sail/graphics/geometry/factory/InstancedTestModel.h"
#include "../gamemodes/payload/PayloadGamemode.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


GameState::GameState(StateStack& stack)
: State(stack)
, m_cam(60.f, 1280.f / 720.f, 0.1f, 100000.f)
, m_camController(&m_cam)
, m_fpsText(&m_font, L"")
, m_debugCamText(&m_font, L"")
, m_flyCam(false)
, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
, m_particleEmitter(ParticleEmitter::EXPLOSION, Vector3(-1.f, 0.f, 0.f), Vector3(5.f, -2.f, 0.5f), 500.f, 10000, 1.f, 0.5f, Vector4::One, 1.f, &m_cam, false)
{

	// Get the Application instance
	m_app = Application::getInstance();
	Application::GameSettings* settings = &m_app->getGameSettings();

	m_app->getResourceManager().LoadDXTexture("background_tile.tga");

	// Set up handlers
	m_level = std::make_unique<Level>("speedrun.level");
	m_gamemode = std::make_unique<PayloadGamemode>(m_level->getGrid()->getControlpointIndices(), m_level->getGrid()->getAllBlocks(), m_level->getGridWidth(), m_level->getGridHeight());
	// **Upgrade to dynamic and only PayloadGamemode later**
	PayloadGamemode* gamemode = dynamic_cast<PayloadGamemode*>(m_gamemode.get());
	if (gamemode) {
		gamemode->setTeamColor(1, m_app->getGameSettings().teamOneColor);
		gamemode->setTeamColor(2, m_app->getGameSettings().teamTwoColor);
	}
	// **Upgrade to dynamic and only PayloadGamemode later**

	m_projHandler = std::make_unique<ProjectileHandler>();
	m_characterHandler = std::make_unique<CharacterHandler>(m_projHandler.get());
	m_upgradeHandler = std::make_unique<UpgradeHandler>();
	m_collisionHandler = std::make_unique <CollisionHandler>(m_level.get(), m_characterHandler.get(), m_projHandler.get(), m_upgradeHandler.get());


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
	m_characterHandler->addSpawnPoint(2, Vector3(14, 2, 0));
	m_characterHandler->addSpawnPoint(2, Vector3(15, 2, 0));

	m_upgradeHandler->addSpawn(Vector3(5, 1.0f, 0), Upgrade::AUTO_FIRE, 10);
	m_upgradeHandler->addSpawn(Vector3(6, 1.0f, 0), Upgrade::PROJECTILE_SPEED, 10);
	m_upgradeHandler->addSpawn(Vector3(7, 1.0f, 0), Upgrade::EXTRA_DAMAGE, 10);
	m_upgradeHandler->addSpawn(Vector3(8, 1.0f, 0), Upgrade::EXTRA_PROJECTILES, 10);
	m_upgradeHandler->addSpawn(Vector3(9, 1.0f, 0), Upgrade::NO_GRAVITY, 10);

	m_scene.addObject(m_upgradeHandler->getSpawn(0));
	m_scene.addObject(m_upgradeHandler->getSpawn(1));
	m_scene.addObject(m_upgradeHandler->getSpawn(2));
	m_scene.addObject(m_upgradeHandler->getSpawn(3));
	m_scene.addObject(m_upgradeHandler->getSpawn(4));


	// Add the characters for rendering and respawn them
	for (size_t i = 0; i < m_characterHandler->getNrOfPlayers(); i++) {
		m_scene.addObject(m_characterHandler->getCharacter(i));
		m_characterHandler->respawnPlayer(i);
		// SETS TEAMS PER INDEX
		//m_characterHandler->getCharacter(i)->setTeam(i % 2 + 1);
	}

	// Give the cam controller targets to follow
	m_playerCamController->setTargets(
		m_characterHandler->useableTarget(0) ? m_characterHandler->getCharacter(0) : nullptr,
		m_characterHandler->useableTarget(1) ? m_characterHandler->getCharacter(1) : nullptr,
		m_characterHandler->useableTarget(2) ? m_characterHandler->getCharacter(2) : nullptr,
		m_characterHandler->useableTarget(3) ? m_characterHandler->getCharacter(3) : nullptr
	);

	m_playerCamController->setPosition(Vector3(10, 10, 0));


	// Set up background infinity planes
	m_infinityPlane = ModelFactory::PlaneModel::Create(Vector2(10000.f, 10000.f), Vector2(400.f));
	m_infinityPlane->getTransform().translate(Vector3(10.f, -50.f, 0.f));
	m_infinityPlane->buildBufferForShader(&m_app->getResourceManager().getShaderSet<SimpleTextureShader>());
	m_infinityPlane->getMaterial()->setDiffuseTexture("background_tile.tga");

	m_infBottom = std::make_unique<Block>(m_infinityPlane.get());
	m_infTop = std::make_unique<Block>(m_infinityPlane.get());
	m_infLeft = std::make_unique<Block>(m_infinityPlane.get());
	m_infRight = std::make_unique<Block>(m_infinityPlane.get());

	m_infBottom->getTransform().setRotations(Vector3(0.f, 0.f, 0.f));
	m_infBottom->getTransform().setTranslation(Vector3(mapSize.x / 2.f, -70.f, 0.f));
	m_infLeft->getTransform().setRotations(Vector3(0.f, 0.f, -1.57f));
	m_infLeft->getTransform().setTranslation(Vector3(-70.f, 0.f, 0.f));
	m_infRight->getTransform().setRotations(Vector3(0.f, 0.f, 1.57f));
	m_infRight->getTransform().setTranslation(Vector3(mapSize.x + 70.f, 0.f, 0.f));
	m_infTop->getTransform().setRotations(Vector3(0.f, 0.f, 3.1415f));
	m_infTop->getTransform().setTranslation(Vector3(mapSize.x / 2.f, mapSize.y + 70.f, 0.f));

	m_scene.addObject(m_infBottom.get());
	m_scene.addObject(m_infTop.get());
	m_scene.addObject(m_infLeft.get());
	m_scene.addObject(m_infRight.get());


	// Instance test stuff
	//m_instancedModel = ModelFactory::InstancedTestModel::Create(10000);
	//m_instancedModel->buildBufferForShader(&m_app->getResourceManager().getShaderSet<ParticleShader>());

	m_notinstancedModel = ModelFactory::PlaneModel::Create(Vector2(0.5f, 0.5f));
	m_notinstancedModel->buildBufferForShader(&m_app->getResourceManager().getShaderSet<SimpleColorShader>());

	for (int i = 0; i < 10000; i++) {
		m_notinstancedBlocks.push_back(std::make_unique<Block>(m_notinstancedModel.get()));
		m_notinstancedBlocks.back()->getTransform().setTranslation(Vector3(Utils::rnd() * 5, Utils::rnd() * 5, Utils::rnd() * 5));
	}

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

	// Infinity planes color update
	static float epilepsyAmount = 1.5f;
	static float epilepsySpeed = 0.3f;
	static float counter = 0;
	counter += dt * epilepsySpeed;
	Vector4 infColor(-fabs(sinf(counter)) * epilepsyAmount, -fabs(sin(counter + 2.f)) * epilepsyAmount, -fabs(sinf(counter + 4.f)) * epilepsyAmount, 1.f);
	m_infBottom->setColor(infColor);
	m_infTop->setColor(infColor);
	m_infLeft->setColor(infColor);
	m_infRight->setColor(infColor);

	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));

	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

	m_characterHandler->update(dt);

	m_level->update(dt, m_characterHandler.get());
	m_gamemode->update(m_characterHandler.get(), dt);
	if (m_gamemode->checkWin()) {
		if (m_gamemode->checkWin() > 0)
			std::cout << "TEAM " << m_gamemode->checkWin() << " HAS WON!" << std::endl;
		else
			std::cout << "DRAW!" << std::endl;

		requestStackClear();
		requestStackPush(States::ID::MainMenu);
	}

	if(!m_flyCam)
		m_playerCamController->update(dt);
	
	m_projHandler->update(dt);
	m_upgradeHandler->update(dt);

	m_playerCamController->setTargets(
		m_characterHandler->useableTarget(0) ? m_characterHandler->getCharacter(0) : nullptr,
		m_characterHandler->useableTarget(1) ? m_characterHandler->getCharacter(1) : nullptr,
		m_characterHandler->useableTarget(2) ? m_characterHandler->getCharacter(2) : nullptr,
		m_characterHandler->useableTarget(3) ? m_characterHandler->getCharacter(3) : nullptr
	);

	// Update camera in shaders
	m_app->getResourceManager().getShaderSet<SimpleTextureShader>().updateCamera(m_cam);
	m_app->getResourceManager().getShaderSet<ParticleShader>().updateCamera(m_cam);
	m_app->getResourceManager().getShaderSet<SimpleColorShader>().updateCamera(m_cam);

	// Update particles
	//if (m_app->getInput().getKeyboardState().B)
	m_particleEmitter.updateEmitPosition(m_characterHandler->getCharacter(0)->getTransform().getTranslation() - Vector3(0.f, -0.3f, 0.f));
	m_particleEmitter.update(dt);

	return true;
}
// Renders the state
bool GameState::render(float dt) {

	// Clear back buffer
	m_app->getDXManager()->clear({0.0, 0.0, 0.0, 0.0});

	// Draw the scene using deferred rendering
	m_scene.draw(dt, m_cam, m_level.get(), m_projHandler.get(), m_gamemode.get());

	//m_app->getDXManager()->disableDepthBuffer();
	m_app->getDXManager()->enableAlphaBlending();
	m_particleEmitter.draw();
	//m_app->getDXManager()->disableAlphaBlending();
	//m_app->getDXManager()->enableDepthBuffer();
	//m_app->getDXManager()->enableBackFaceCulling();
	/*for (auto& b : m_notinstancedBlocks)
		b->draw();*/

	// Draw HUD
	m_scene.drawHUD();

	return true;
}