#include "MenuState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MenuState::MenuState(StateStack& stack) 
	: State(stack)
	, m_cam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
	, m_fpsText(&m_font, L"")
	, m_debugCamText(&m_font, L"")
	, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
{
	// Get the Application instance
	m_app = Application::getInstance();

	// Set up camera with controllers
	m_playerCamController = std::make_unique<PlayerCameraController>(&m_cam);


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



	m_menuBlock = std::make_unique<FbxModel>("block.fbx");
	m_menuBlock->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());

	m_menuStart = std::make_unique<FbxModel>("startButton.fbx");
	m_menuStart->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());

	m_menuOptions = std::make_unique<FbxModel>("optionsButton.fbx");
	m_menuOptions->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());

	m_menuExit = std::make_unique<FbxModel>("exitButton.fbx");
	m_menuExit->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());

	m_player = std::make_unique<FbxModel>("fisk.fbx");
	m_player->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());

	m_backGround = std::make_unique<FbxModel>("menu_screen.fbx");
	m_backGround->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());
	

	m_activeMenu = 0;
	m_activeSubMenu = 0;
	m_selector = 0;

	m_menu = -1;
	
	this->background = new MenuItem(m_backGround->getModel(), Vector3(0.f, -2.3f, 0.5f));
	this->background->useColor = 1;
	this->background->setLightColor(Vector4(0.5f,0.5f,0.5f,0.5f));
	m_scene.addObject(background);


	MenuItem* start = new MenuItem(m_menuStart->getModel(), Vector3(0.f, 2.f, 7.f));
	MenuItem* options = new MenuItem(m_menuOptions->getModel(), Vector3(0.f, 0.f, 7.f));
	MenuItem* exit = new MenuItem(m_menuExit->getModel(), Vector3(0.f, -2.f, 7.f));

	MenuItem* player1 = new MenuItem(m_player->getModel(), Vector3(4.0f, 0.0f, 2.25f));
	MenuItem* player2 = new MenuItem(m_player->getModel(), Vector3(4.0f, 0.0f, 0.75f));
	MenuItem* player3 = new MenuItem(m_player->getModel(), Vector3(4.0f, 0.0f, -0.75f));
	MenuItem* player4 = new MenuItem(m_player->getModel(), Vector3(4.0f, 0.0f, -2.25f));

	MenuItem* option1 = new MenuItem(m_menuOptions->getModel(), Vector3(-7.0f, 1.5f, 0.f));
	MenuItem* option2 = new MenuItem(m_menuOptions->getModel(), Vector3(-7.0f, 0.f, 0.f));
	MenuItem* option3 = new MenuItem(m_menuOptions->getModel(), Vector3(-7.0f, -1.5f, 0.f));

	this->onColor = Vector4(1.f, 1.f, 1.f, 1.f);
	this->offColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);


	MenuItem* maps[5];
	int extraY = 0;
	for (size_t i = 0; i < 5; i++) {

		maps[i] = new MenuItem(m_menuBlock->getModel(), Vector3(3.f - float(i) * 1.5f, 0.f, -7.f));
		maps[i]->setLightColor(this->offColor);
		
		this->mapMenu.push_back(maps[i]);
	}


	this->menuList.push_back(start);
	this->menuList.push_back(options);
	this->menuList.push_back(exit);

	this->playerMenu.push_back(player1);
	this->playerMenu.push_back(player2);
	this->playerMenu.push_back(player3);
	this->playerMenu.push_back(player4);

	this->optionsMenuList.push_back(option1);
	this->optionsMenuList.push_back(option2);
	this->optionsMenuList.push_back(option3);

	for (int i = 0; i < 4; i++) {
		this->players[i] = OFFLINE;
		this->setColor(i, this->offColor);
	}

	for (size_t i = 0; i < this->menuList.size(); i++) {
		menuList[i]->setLightColor(this->offColor);
		m_scene.addObject(menuList[i]);
	}
	for (size_t i = 0; i < this->playerMenu.size(); i++) {
		this->playerMenu[i]->setLightColor(this->offColor);
		this->playerMenu[i]->getTransform().setRotations(Vector3(0.0f, -1.62f, 0.0f));
		m_scene.addObject(playerMenu[i]);
	}
	for (size_t i = 0; i < this->mapMenu.size(); i++) {
		
		m_scene.addObject(mapMenu[i]);
	}

	for (size_t i = 0; i < 3; i++) {
		this->optionsMenuList[i]->getTransform().setRotations(Vector3(0.0f, -1.62f, 0.0f));
		this->optionsMenuList[i]->setLightColor(this->offColor);
		m_scene.addObject(this->optionsMenuList[i]);
	}


	this->changeMenu(0, MAINMENU);

	m_playerCamController->setUseExtraZ(false);
	m_playerCamController->setTargets(this->menuList[0]);
	m_playerCamController->setOffset(Vector3(0,0,0));
	m_playerCamController->setMoving(false);
	m_playerCamController->setPosition(Vector3(0,0,0));
	m_playerCamController->setFollowSpeed(8);

	m_app->getGameSettings().reset();
}


MenuState::~MenuState()
{
	delete this->background;
	for (size_t i = 0; i < this->menuList.size(); i++) {
		delete this->menuList[i];
	}
	for (size_t i = 0; i < this->playerMenu.size(); i++) {
		delete this->playerMenu[i];
	}
	for (size_t i = 0; i < this->mapMenu.size(); i++) {
		delete this->mapMenu[i];
	}
	for (size_t i = 0; i < this->optionsMenuList.size(); i++) {
		delete this->optionsMenuList[i];
	}
}



// Process input for the state
bool MenuState::processInput(float dt) {

	static Keyboard::KeyboardStateTracker kbTracker;
	static GamePad::ButtonStateTracker gpTracker[4];
	for (int i = 0; i < 4; i++)
		gpTracker[i].Update(m_app->getInput().gamepadState[i]);
	kbTracker.Update(m_app->getInput().keyboardState);

	//DirectX::Keyboard::State& keyState;
	
	

	for (int i = 0; i < 4; i++) {
		DirectX::GamePad::State& padState = m_app->getInput().gamepadState[i];
		GamePad::ButtonStateTracker& padTracker = gpTracker[i];
		
		int a = 0;
		int b = 0;
		int down = 0;
		int up = 0;
		int left = 0;
		int right = 0;
		int menu = 0;
		int back = 0;
		int pressed = 0;
		int spacePressed = 0;
		if (i == 0) {

			if (kbTracker.pressed.W) {
				up = 1;
				pressed = 1;
			}
			if (kbTracker.pressed.S) {
				down = 1;
				pressed = 1;
			} 
			if (kbTracker.pressed.D) {
				right = 1;
				pressed = 1;
			}
			if (kbTracker.pressed.A) {
				left = 1;
				pressed = 1;
			}
			if (kbTracker.pressed.Space) {
				a = 1;
				pressed = 1;
				spacePressed = 1;
			}
			if (kbTracker.pressed.Tab) {
				b = 1;
				pressed = 1;
			}
		}


		



		if (padState.IsConnected()) {

			if (padTracker.a == GamePad::ButtonStateTracker::PRESSED) {
				a = 1;
				pressed = 1;
			}
			if (padTracker.b == GamePad::ButtonStateTracker::PRESSED || padTracker.back == GamePad::ButtonStateTracker::PRESSED) {
				b = 1;
				pressed = 1;
			}
			if (padTracker.dpadUp == GamePad::ButtonStateTracker::PRESSED || padTracker.leftStickUp == GamePad::ButtonStateTracker::PRESSED) {
				up = 1;
				pressed = 1;
			}
			if (padTracker.dpadDown == GamePad::ButtonStateTracker::PRESSED || padTracker.leftStickDown == GamePad::ButtonStateTracker::PRESSED) {
				down = 1;
				pressed = 1;
			}
			if (padTracker.dpadRight == GamePad::ButtonStateTracker::PRESSED || padTracker.leftStickRight == GamePad::ButtonStateTracker::PRESSED) {
				right = 1;
				pressed = 1;
			}
			if (padTracker.dpadLeft == GamePad::ButtonStateTracker::PRESSED || padTracker.leftStickLeft == GamePad::ButtonStateTracker::PRESSED) {
				left = 1;
				pressed = 1;
			}
		}
		if(pressed)
			switch (m_activeMenu) {
				case MAINMENU:
					if (down)
						this->changeMenu(1, MAINMENU);
					if (up)
						this->changeMenu(-1, MAINMENU);
					if (a) {
						switch (m_selector) {
							case START:
								this->m_playerCamController->setTargets(this->playerMenu[0], this->playerMenu[1], 
									this->playerMenu[2], this->playerMenu[3]);
								this->menuList[m_selector]->setLightColor(this->offColor);
								m_selector = 0;

								m_activeMenu = STARTMENU;
								break;
							case OPTIONS:
								m_activeMenu = OPTIONSMENU;
								this->menuList[m_selector]->setLightColor(this->offColor);
								m_selector = 0;
								this->changeMenu(0, OPTIONSMENU);

								break;

							case EXIT:
								PostQuitMessage(0);
								break;
						}
					}


					break;
				case STARTMENU:
					switch (m_activeSubMenu) {
						case PLAYERSELECT:
							switch (this->players[i]) {
								case OFFLINE:
									if (a) {
										this->players[i] = ONLINE;
										this->setColor(i, this->onColor);
									}
									if (b) {
										for (size_t u = 0; u < 4; u++) {
											this->players[u] = OFFLINE;
											this->setColor(u, Vector4(0, 0, 0, 1));
											this->m_playerCamController->setTargets(this->menuList[0]);

											m_activeMenu = MAINMENU;
											m_selector = 0;
											this->changeMenu(0, MAINMENU);
										}
									}



									break;
								case ONLINE:
									switch (this->playersReady[i]) {
										case NOTREADY:
											if(a) {
												this->playersReady[i] = READY;
												int nrOnline = 0;
												int nrReady = 0;
												for (size_t u = 0; u < 4; u++) {
													if (this->players[u] == ONLINE) 
														nrOnline++;
													if (this->playersReady[u] == READY) 
														nrReady++;
												}
												if (nrReady == nrOnline) {
													m_activeSubMenu = MAPSELECT;
													m_selector = 0;
													this->changeMenu(0, STARTMENU);
													this->m_playerCamController->setTargets(this->mapMenu[0]);				
													break;
												}
											}
											if (b) {
											this->players[i] = OFFLINE;
											this->setColor(i, this->offColor);

											}

											if (right) {
												this->setColor(i, Utils::getRandomColor());
											}
											if (left) {
												this->setColor(i, Utils::getRandomColor());
											}



											break;
										case READY:
											if (b) {
												this->players[i] = OFFLINE;
												this->setColor(i, this->offColor);

											}
												

											
											break;

									}

									break;
							}
							break;
						case MAPSELECT:
							if (right) {
								this->changeMenu(1, STARTMENU);
							}
							if (left) {
								this->changeMenu(-1, STARTMENU);
							}
							if (a) {	
								// START GAME

								m_app->getGameSettings().level = m_selector;
								m_app->getGameSettings().gamemode = m_gamemode;
								m_app->getGameSettings().level = m_map;
								for (size_t u = 0; u < 4; u++) {
									if (playersReady[u]) {
										Application::GameSettings::player player;
										if (spacePressed && u == 0) {
											player.port = -1;
										}
										else
											player.port = u;
										player.color = this->playerColor[u];
										player.model = m_playerModel[u];
										player.team = m_playerTeam[u];
										m_app->getGameSettings().players.push_back(player);

									}
								}

									
								requestStackPop();
								requestStackPush(States::Game);
							}
							if (b) {
								m_activeSubMenu = PLAYERSELECT;
								this->m_playerCamController->setTargets(this->playerMenu[0], this->playerMenu[1],
									this->playerMenu[2], this->playerMenu[3]);
								for (size_t u = 0; u < 4; u++) {
									this->playersReady[u] = NOTREADY;
								}
								this->mapMenu[m_selector]->setLightColor(this->offColor);
								m_selector = 0;
							}
							break;			
					}

					break;
				case OPTIONSMENU:
					if (b) {
						this->m_activeMenu= MAINMENU;
						this->optionsMenuList[m_selector]->setLightColor(this->offColor);
						m_selector = 0;
						this->changeMenu(0, MAINMENU);

					}
					if (down) {
						this->changeMenu(1, OPTIONSMENU);
					}
					if (up) {
						this->changeMenu(-1, OPTIONSMENU);
					}


					break;
			}
		}
	
	return true;
}

// Process window resizing for the state
bool MenuState::resize(int width, int height) {
	m_cam.resize(width, height);
	m_scene.resize(width, height);

	return true;
}


// Updates the state
bool MenuState::update(float dt) {

	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));

	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

	m_playerCamController->update(dt);

	return true;
}
// Renders the state
bool MenuState::render(float dt) {
	// Clear back buffer
	m_app->getDXManager()->clear(DirectX::Colors::Black);

	// Draw the scene
	m_scene.draw(dt, m_cam, nullptr, nullptr);

	// Draw HUD
	m_scene.drawHUD();

	return true;
}




void MenuState::changeMenu(int change, int active)
{
	int max = 0;
	if (active == MAINMENU) {
		this->menuList[m_selector]->setLightColor(this->offColor);
		max = (int)this->menuList.size()-1;
	}
	if (active == OPTIONSMENU) {
		this->optionsMenuList[m_selector]->setLightColor(this->offColor);
		max = (int)this->menuList.size() - 1;
	}
	if (active == STARTMENU && m_activeSubMenu == MAPSELECT) {

		this->mapMenu[m_selector]->setLightColor(this->offColor);
		max = (int)this->mapMenu.size() - 1;
	}


	m_selector += change;
	if (m_selector < 0)
		m_selector = max;
	if (m_selector > max)
		m_selector = 0;
	if (active == MAINMENU) {
		this->menuList[m_selector]->setLightColor(this->onColor);
		m_playerCamController->setTargets(
			this->menuList[m_selector],
			nullptr,
			nullptr,
			nullptr
		);
	}
	if (active == OPTIONSMENU) {
		this->optionsMenuList[m_selector]->setLightColor(this->onColor);
		m_playerCamController->setTargets(
			this->optionsMenuList[m_selector],
			nullptr,
			nullptr,
			nullptr
		);
	}
	if (active == STARTMENU && m_activeSubMenu == MAPSELECT) {

		this->mapMenu[m_selector]->setLightColor(this->onColor);
		m_playerCamController->setTargets(
			this->mapMenu[m_selector],
			nullptr,
			nullptr,
			nullptr
		);
	}
	
}
