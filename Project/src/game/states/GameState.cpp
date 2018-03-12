#include "GameState.h"
#include "../objects/Block.h"
#include "../gamemodes/payload/PayloadGamemode.h"
#include "../gamemodes/deathmatch/Deathmatch.h"
#include "../gamemodes/tdm/TeamDeathmatch.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


GameState::GameState(StateStack& stack)
: State(stack)
, m_cam(30.f, 1280.f / 720.f, 0.1f, 5000.f)
, m_camController(&m_cam)
, m_fpsText(&m_font, L"")
, m_debugCamText(&m_font, L"")
, m_flyCam(false)
, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
//, m_testBlocks(250)
{

	// Get the Application instance
	m_app = Application::getInstance();
	m_info = GameInfo::getInstance();
	Application::GameSettings* settings = &m_app->getGameSettings();

	m_app->getResourceManager().LoadDXTexture("background_tile2.tga");

	// Set up handlers


	GameInfo * info = GameInfo::getInstance();

	/*VIB REMOVAL*/
	auto& gamePad = m_app->getInput().getGamePad();
	for (int u = 0; u < 4; u++)
		gamePad.SetVibration(u, 0, 0);

#ifdef _DEBUG
	if (info->gameSettings.teams.size() == 0) {
		info->gameSettings.teams.push_back({ 0, 0 });
		info->gameSettings.teams.push_back({ 1, 0 });
		info->convertGameSettings();
	}
	if (info->getPlayers().size() == 0) {
		info->addPlayer({ &info->getProfiles()[0], 0, 0, 0, 0, 0, 0, 0, 0 });
		info->addPlayer({ &info->getProfiles()[1], 1, 1, 1, 0, 0, 0, 0, 0 });
	} else if (info->getPlayers().size() == 1) {
		info->addPlayer({ &info->getProfiles()[1], 1, 0, 0, 0, 0, 0, 0, 0 });
		info->addPlayer({ &info->getProfiles()[2], 2, 1, 1, 0, 0, 0, 0, 0 });
		info->addPlayer({ &info->getProfiles()[3], 3, 1, 1, 0, 0, 0, 0, 0 });
	}
#endif
	std::string map = info->convertedGameSettings.map;

	m_level = std::make_unique<Level>(map);
	

	// Set up handlers
	m_particleHandler = std::make_unique<ParticleHandler>(&m_cam);
	m_projHandler = std::make_unique<ProjectileHandler>(m_particleHandler.get());
	m_characterHandler = std::make_unique<CharacterHandler>(m_particleHandler.get(), m_projHandler.get());
	m_upgradeHandler = std::make_unique<UpgradeHandler>();
	m_collisionHandler = std::make_unique<CollisionHandler>(m_level.get(), m_characterHandler.get(), m_projHandler.get(), m_upgradeHandler.get());

	switch (info->convertedGameSettings.gamemode) {
	case GameInfo::DOMINATION: {
		m_gamemode = std::make_unique<PayloadGamemode>(m_level->getGrid()->getControlpointIndices(), m_level->getGrid()->getAllBlocks(), m_level->getGridWidth(), m_level->getGridHeight());
		PayloadGamemode* gamemode = dynamic_cast<PayloadGamemode*>(m_gamemode.get());
		if (gamemode) {
			gamemode->setParticleHandler(m_particleHandler.get());
		}

		

	}break;
	case GameInfo::DEATHMATCH: {
		m_gamemode = std::make_unique<Deathmatch>();
	}break;
	case GameInfo::TEAMDEATHMATCH: {
		m_gamemode = std::make_unique<TeamDeathmatch>();
	}break;
	default: {
		m_gamemode = nullptr;
		Logger::Error("No gamemode were properly set.");
	}break;
	}
	

	m_scoreVisualization = std::make_unique<ScoreVisualization>(m_level.get(), m_gamemode.get());
	m_scene.addObject(m_scoreVisualization.get());

	// Set up camera with controllers
	m_cam.setPosition(Vector3(0.f, 5.f, -7.0f));
	Vector2 mapSize = m_level->getGridWorldSize();
	m_playerCamController = std::make_unique<PlayerCameraController>(&m_cam, &mapSize);
	
	// Set up the scene
	//m_scene.addSkybox(L"skybox_space_512.dds");
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
	std::vector<Grid::Index> playerSpawnPoints = m_level->getGrid()->getPlayerSpawnPointIndices();
	if (playerSpawnPoints.size() > 0) {
		for (Grid::Index index : playerSpawnPoints) {
			if (float(index.x) <= m_level->getGridWidth() / 2.f)
				m_characterHandler->addSpawnPoint(0, Vector3((float(index.x) + 0.5f) * Level::DEFAULT_BLOCKSIZE, float(index.y * Level::DEFAULT_BLOCKSIZE), 0.f));
			else
				m_characterHandler->addSpawnPoint(1, Vector3((float(index.x) + 0.5f) * Level::DEFAULT_BLOCKSIZE, float(index.y * Level::DEFAULT_BLOCKSIZE), 0.f));
		}
	}
	else {
		m_characterHandler->addSpawnPoint(0, Vector3(2, 2, 0));
		m_characterHandler->addSpawnPoint(0, Vector3(3, 2, 0));
		m_characterHandler->addSpawnPoint(1, Vector3(14, 2, 0));
		m_characterHandler->addSpawnPoint(1, Vector3(15, 2, 0));
	}


	std::vector<Grid::Index> upgradeSpawnPoints = m_level->getGrid()->getUpgradeSpawnPointIndices();
	int index = 0;
	if (upgradeSpawnPoints.size() > 0) {
		for (Grid::Index gIndex : upgradeSpawnPoints) {
			m_upgradeHandler->addSpawn(Vector3((float(gIndex.x) + 0.5f) * Level::DEFAULT_BLOCKSIZE, float(gIndex.y) * Level::DEFAULT_BLOCKSIZE, 0.f), Upgrade::RANDOM, 10);
			m_scene.addObject(m_upgradeHandler->getSpawn(index));
			index++;
		}
	}
	else {
		m_upgradeHandler->addSpawn(Vector3(5, 1.0f, 0), Upgrade::AUTO_FIRE, 10);
		m_upgradeHandler->addSpawn(Vector3(6, 1.0f, 0), Upgrade::PROJECTILE_KNOCKBACK, 10);
		m_upgradeHandler->addSpawn(Vector3(7, 1.0f, 0), Upgrade::EXTRA_DAMAGE, 10);
		m_upgradeHandler->addSpawn(Vector3(8, 1.0f, 0), Upgrade::EXTRA_PROJECTILES, 10);
		m_upgradeHandler->addSpawn(Vector3(9, 1.0f, 0), Upgrade::NO_GRAVITY, 10);

		m_scene.addObject(m_upgradeHandler->getSpawn(0));
		m_scene.addObject(m_upgradeHandler->getSpawn(1));
		m_scene.addObject(m_upgradeHandler->getSpawn(2));
		m_scene.addObject(m_upgradeHandler->getSpawn(3));
		m_scene.addObject(m_upgradeHandler->getSpawn(4));
	}



	// Add the characters for rendering and respawn them
	for (size_t i = 0; i < m_characterHandler->getNrOfPlayers(); i++) {
		m_scene.addObject(m_characterHandler->getCharacter(i));
		m_characterHandler->respawnPlayer(i);
		// SETS TEAMS PER INDEX
		//m_characterHandler->getCharacter(i)->setTeam(i % 2 + 1);
	}

	// Give the cam controller targets to follow
	m_playerCamController->setCharacterHandler(m_characterHandler.get());

	m_playerCamController->setPosition(Vector3(10, 10, 0));


	// Set up background infinity planes
	m_infinityPlane = ModelFactory::PlaneModel::Create(Vector2(10000.f, 10000.f), Vector2(400.f, 300.f));
	m_infinityPlane->getTransform().translate(Vector3(10.f, -50.f, 0.f));
	m_infinityPlane->buildBufferForShader(&m_app->getResourceManager().getShaderSet<SimpleTextureShader>());
	m_infinityPlane->getMaterial()->setDiffuseTexture("background_tile2.tga");

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


	m_app->getResourceManager().getSoundManager()->playAmbientSound(SoundManager::Ambient::Battle_Sound, true, 0.10f);
}

GameState::~GameState() {
	auto& gamePad = m_app->getInput().getGamePad();
	for (int u = 0; u < 4; u++)
		gamePad.SetVibration(u, 0, 0);
	/*GameInfo* info = GameInfo::getInstance();
	for (unsigned int i = 0; i < m_characterHandler->getNrOfPlayers(); i++) {
		std::cout << "Player " << (i + 1) << std::endl;
		std::cout << "Deaths: " << info->getScore().getPlayerStats(i).deaths << std::endl;
		std::cout << "Kills: " <<info->getScore().getPlayerStats(i).kills << std::endl << std::endl;
	}*/

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

	GameInfo* info = GameInfo::getInstance();
	if (kbTracker.pressed.O) {
		if (info->convertedGameSettings.gamemode == 0) {
			m_gamemode->addScore(10,0);
			m_gamemode->addScore(-10, 1);
			info->getScore().addKill(0);
			info->getScore().addDeath(2);
			info->getScore().addKill(1);
			info->getScore().addDeath(3);
			info->getScore().addKill(0);
			info->getScore().addDeath(3);

		}
		if (info->convertedGameSettings.gamemode == 1) {
			info->getScore().addCapture(0);
			info->getScore().addPoints(0, 10);
			info->getScore().addKill(0);
			info->getScore().addDeath(1);

		}

		if (info->convertedGameSettings.gamemode == 2) {
			info->getScore().addCapture(0);
			info->getScore().addPoints(0, 10);
			info->getScore().addKill(0);
			info->getScore().addDeath(1);

		}




	}

	if (kbTracker.pressed.P) {
		if (info->convertedGameSettings.gamemode == 0) {
			m_gamemode->addScore(10, 1);
			m_gamemode->addScore(-10, 0);
			info->getScore().addKill(2);
			info->getScore().addDeath(0);
			info->getScore().addKill(2);
			info->getScore().addDeath(1);
			info->getScore().addKill(3);
			info->getScore().addDeath(1);

		}

		if (info->convertedGameSettings.gamemode == 1) {

			info->getScore().addCapture(1);
			info->getScore().addPoints(1, 10);
			info->getScore().addKill(1);
			info->getScore().addDeath(0);

		}


		if (info->convertedGameSettings.gamemode == 2) {

			info->getScore().addCapture(1);
			info->getScore().addPoints(1, 10);
			info->getScore().addKill(1);
			info->getScore().addDeath(0);

		}



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
	static float epilepsyAmount = 0.1f;
	static float epilepsySpeed = 0.3f;
	static float counter = 0;
	counter += dt * epilepsySpeed;
	Vector4 infColor(fabs(sinf(counter)) * epilepsyAmount, fabs(sin(counter + 2.f)) * epilepsyAmount, fabs(sinf(counter + 4.f)) * epilepsyAmount, 1.f);
	m_infBottom->setColor(infColor);
	m_infTop->setColor(infColor);
	m_infLeft->setColor(infColor);
	m_infRight->setColor(infColor);


	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));

	auto& camPos = m_cam.getPosition();
	//m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()) + L" Particles: " + std::to_wstring(m_particleHandler->getParticleCount()));
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Emitters: " + std::to_wstring(m_particleHandler->getEmitterCount()) + L" Particles: " + std::to_wstring(m_particleHandler->getParticleCount()));

	// Projectiles needs to be updated before characters, otherwise projectiles will hit characters through blocks
	m_projHandler->update(dt);
	m_upgradeHandler->update(dt);

	m_characterHandler->update(dt);

	m_level->update(dt, m_characterHandler.get());
	m_gamemode->update(m_characterHandler.get(), dt);

	/*
		UPDATE DIS SHIET
	*/
	int checkWin = m_gamemode->checkWin();
	if (checkWin > Gamemode::NONE) {
		if (checkWin > Gamemode::DRAW && checkWin < (int)m_info->convertedGameSettings.teams.size())
			m_info->convertedGameSettings.teams[checkWin].winner = true;

		m_app->getResourceManager().getSoundManager()->stopAmbientSound(SoundManager::Ambient::Battle_Sound);
		
		requestStackClear();
		requestStackPush(States::Score);
	}

	if(!m_flyCam)
		m_playerCamController->update(dt);

	// Update camera in shaders
	m_app->getResourceManager().getShaderSet<SimpleTextureShader>().updateCamera(m_cam);
	m_app->getResourceManager().getShaderSet<ParticleShader>().updateCamera(m_cam);
	m_app->getResourceManager().getShaderSet<SimpleColorShader>().updateCamera(m_cam);
	m_app->getResourceManager().getShaderSet<DynBlockDeferredInstancedGeometryShader>().updateCamera(m_cam);
	m_app->getResourceManager().getShaderSet<DeferredInstancedGeometryShader>().updateCamera(m_cam);
	m_app->getResourceManager().getShaderSet<MaterialShader>().updateCamera(m_cam);

	// Resolve collisions, must be done before particleHandler updates since it can spawn new particles
	CollisionHandler::getInstance()->resolveProjectileCollision(dt);

	// Update particles
	m_particleHandler->update(dt);

	m_scoreVisualization->update(dt);

	return true;
}

// Renders the state
bool GameState::render(float dt) {

	// Clear back buffer
	m_app->getDXManager()->clear({0.05f, 0.05f, 0.05f, 1.0f});

	// Draw the scene using deferred rendering
	m_scene.draw(dt, m_cam, m_level.get(), m_projHandler.get(), m_gamemode.get(), m_particleHandler.get());

	//m_particleHandler->draw();

	// Draw HUD
	m_scene.drawHUD();

	return true;
}