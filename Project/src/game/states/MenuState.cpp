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


	auto& resMan = m_app->getResourceManager();

	m_block = resMan.getFBXModel("block2").getModel();
	m_menuBlockModel = resMan.getFBXModel("block").getModel();
	m_menuStartModel = resMan.getFBXModel("startButton").getModel();
	m_menuOptionsModel = resMan.getFBXModel("optionsButton").getModel();
	m_menuExitModel = resMan.getFBXModel("exitButton").getModel();
	m_playerModel = resMan.getFBXModel("fisk").getModel();
	m_backGroundModel = resMan.getFBXModel("menu_screen").getModel();
	


	m_onColor = Vector4(1.f, 1.f, 1.f, 1.f);
	m_offColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	m_orangeColor = Vector4(1.0f, 0.5f, 0.0f, 1.0f);
	m_blueColor = Vector4(0.0f, 0.9f, 1.0f, 1.0f);

	m_activeMenu = 0;
	m_activeSubMenu = 0;
	m_selector = 0;

	m_menu = -1;
	
	this->background = new MenuItem(m_backGroundModel, Vector3(0.f, -2.3f, 0.5f));
	this->background->m_useColor = 1;
	this->background->setLightColor(Vector4(0.5f,0.5f,0.5f,0.5f));
	//m_scene.addObject(background);



	//MAINMENU
	initMain();

	//STARTMENU
	initGamemode();
	initCharacter();
	initMap();

	//CUSTOMISABILITY
	initProfile();
	initOptions();


	MenuItem* start = new MenuItem(m_menuStartModel, Vector3(0.f, 1.1f, 7.f));
	MenuItem* options = new MenuItem(m_menuOptionsModel, Vector3(0.f, 0.f, 7.f));
	MenuItem* exit = new MenuItem(m_menuExitModel, Vector3(0.f, -1.1f, 7.f));

	MenuItem* player1 = new MenuItem(m_playerModel, Vector3(4.0f, 0.0f, 2.25f ));
	MenuItem* player2 = new MenuItem(m_playerModel, Vector3(4.0f, 0.0f, 0.75f ));
	MenuItem* player3 = new MenuItem(m_playerModel, Vector3(4.0f, 0.0f, -0.75f));
	MenuItem* player4 = new MenuItem(m_playerModel, Vector3(4.0f, 0.0f, -2.25f));
		
	MenuItem* option1 = new MenuItem(m_menuOptionsModel, Vector3(-7.0f, 1.5f, 0.f));
	MenuItem* option2 = new MenuItem(m_menuOptionsModel, Vector3(-7.0f, 0.f, 0.f));
	MenuItem* option3 = new MenuItem(m_menuOptionsModel, Vector3(-7.0f, -1.5f, 0.f));


	MenuItem* maps[5];
	int extraY = 0;
	for (size_t i = 0; i < 5; i++) {

		maps[i] = new MenuItem(m_menuBlockModel, Vector3(3.f - float(i) * 1.5f, 0.f, -7.f));
		maps[i]->setLightColor(m_offColor);
		
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

	for (size_t i = 0; i < 4; i++) {
		//m_player[i] = { false, false, 0, i, {{0,0,0,0,0},0,0} };
		this->players[i] = OFFLINE;
		this->setColor(i, m_offColor);
	}

	for (size_t i = 0; i < this->menuList.size(); i++) {
		menuList[i]->setLightColor(m_offColor);
		//m_scene.addObject(menuList[i]);
	}
	for (size_t i = 0; i < this->playerMenu.size(); i++) {
		this->playerMenu[i]->setLightColor(m_offColor);
		this->playerMenu[i]->getTransform().setRotations(Vector3(0.0f, -1.62f, 0.0f));
		//m_scene.addObject(playerMenu[i]);
	}
	for (size_t i = 0; i < this->mapMenu.size(); i++) {
		
		//m_scene.addObject(mapMenu[i]);
	}

	for (size_t i = 0; i < 3; i++) {
		this->optionsMenuList[i]->getTransform().setRotations(Vector3(0.0f, -1.62f, 0.0f));
		this->optionsMenuList[i]->setLightColor(m_offColor);
		//m_scene.addObject(this->optionsMenuList[i]);
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
	Memory::safeDelete(m_mainMenu);
	Memory::safeDelete(m_gamemodeMenu);
	Memory::safeDelete(m_characterMenu);
	Memory::safeDelete(m_profileMenu);
	Memory::safeDelete(m_optionsMenu);


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
	for (size_t i = 0; i < m_menuText.size(); i++) {
		Memory::safeDelete(m_menuText[i]);
	}
	

}



// Process input for the state
bool MenuState::processInput(float dt) {

	for (int i = 0; i < 4; i++) {
		const GamePad::State& padState = m_app->getInput().getGamePadState(i);
		const GamePad::ButtonStateTracker& padTracker = m_app->getInput().getGpStateTracker(i);
		const Keyboard::KeyboardStateTracker& kbTracker = m_app->getInput().getKbStateTracker();
		
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

		if (kbTracker.pressed.F) {
			static int camP = 0;
			camP++;
			if (camP > 2)
				camP = 0;
			if (camP == 0) 
				m_playerCamController->setPosition(Vector3(0, 0, 0));			
			if(camP == 1)
				m_playerCamController->setPosition(Vector3(-10, 15, -10));
			if(camP == 2)
				m_playerCamController->setPosition(Vector3(10, 15, 10));

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
					if (down) {
						m_mainMenu->next();
						m_playerCamController->setTargets(m_mainMenu, m_mainMenu->getTarget());
					}
					if (up) {
						m_mainMenu->back();
						m_playerCamController->setTargets(m_mainMenu, m_mainMenu->getTarget());
					}
					if (a) {
						switch (m_mainMenu->getActive()) {
							case START:
								setMainSelect(false);
								setGamemodeSelect(true);
								break;
							case PROFILE:
								setMainSelect(false);
								setProfileMenu(true);
								break;
							case OPTIONS:
								setMainSelect(false);
								setOptionsMenu(true);
								break;

							case EXIT:
								PostQuitMessage(0);
								break;
						}
					}


					break;
				case STARTMENU:
					switch (m_activeSubMenu) {
						case GAMEOPTIONSELECT:
							if (a) {
								int gamemodeOption = m_gamemodeMenu->getOptionAt(GAMEMODE);
								int TimeOption = m_gamemodeMenu->getOptionAt(TIMELIMIT);
								int scoreOption = m_gamemodeMenu->getOptionAt(SCORELIMIT);
								int respawnOption = m_gamemodeMenu->getOptionAt(RESPAWNTIME);
								int gravityOption = m_gamemodeMenu->getOptionAt(GRAVITY);


								
							}
							if (b) {
								setGamemodeSelect(false);
								setMainSelect(true);							

							}
							if (down) {
								m_gamemodeMenu->next();
								m_playerCamController->setTargets(m_gamemodeMenu, m_gamemodeMenu->getTarget(), m_gamemodeMenu->getExtraTarget());
							}
							if (up) {
								m_gamemodeMenu->back();
								m_playerCamController->setTargets(m_gamemodeMenu, m_gamemodeMenu->getTarget(), m_gamemodeMenu->getExtraTarget());
							}

							if (right) {
								m_gamemodeMenu->right();
								m_playerCamController->setTargets(m_gamemodeMenu, m_gamemodeMenu->getTarget(), m_gamemodeMenu->getExtraTarget());
							}
							if (left) {
								m_gamemodeMenu->left();
								m_playerCamController->setTargets(m_gamemodeMenu, m_gamemodeMenu->getTarget(), m_gamemodeMenu->getExtraTarget());
							}





							break;
						case PLAYERSELECT:
							switch (this->players[i]) {
								case OFFLINE:
									if (a) {
										this->players[i] = ONLINE;
										this->setColor(i, m_orangeColor);
										m_playerTeam[i] = 1;
									}
									if (b) {
										for (size_t u = 0; u < 4; u++) {
											this->players[u] = OFFLINE;
											m_playerTeam[u] = 0;
											this->setColor(u, m_offColor);
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
											this->setColor(i, m_offColor);
											}

											if (right || left) {
												m_playerTeam[i] = (m_playerTeam[i] % 2) + 1;
												if (m_playerTeam[i] == 1) {
													this->setColor(i, m_orangeColor);
												}
												else if (m_playerTeam[i] == 2) {
													this->setColor(i, m_blueColor);
												}
												else {
													this->setColor(i, m_offColor);
												}
											}



											break;
										case READY:
											if (b) {
												this->players[i] = OFFLINE;
												this->setColor(i, m_offColor);
												m_playerTeam[i] = 0;
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
										player.model = m_playerModelIndex[u];
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
								this->mapMenu[m_selector]->setLightColor(m_offColor);
								m_selector = 0;
							}
							break;			
					}

					break;

				case PROFILEMENU:
					if (a) {

					}
					if (b) {
						setProfileMenu(false);
						setMainSelect(true);
					}


					break;
				case OPTIONSMENU:
					if (b) {
						this->m_activeMenu= MAINMENU;
						this->optionsMenuList[m_selector]->setLightColor(m_offColor);
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

	m_mainMenu->update(dt);
	m_gamemodeMenu->update(dt);
	m_optionsMenu->update(dt);
	m_profileMenu->update(dt);
	m_playerCamController->update(dt);

	return true;
}
// Renders the state
bool MenuState::render(float dt) {
	// Clear back buffer
	m_app->getDXManager()->clear(DirectX::Colors::Black);

	// Draw the scene
	m_scene.draw(dt, m_cam);

	// Draw HUD
	m_scene.drawHUD();

	return true;
}







// CREATE ALL THE MENUS


void MenuState::initMain() {
	m_mainMenu = new MenuHandler();
	m_scene.addObject(m_mainMenu);

	m_mainMenu->addMenuBox("start");
	m_mainMenu->addMenuBox("profiles");
	m_mainMenu->addMenuBox("options");
	m_mainMenu->addMenuBox("exit");
	m_mainMenu->addMenuBox("potato");
	m_mainMenu->setPosition(Vector3(0, -0.5, 7));
	m_mainMenu->setFacingDirection(Vector3(0, 0, -1));
	m_mainMenu->setOffColor(m_offColor);
	m_mainMenu->setOnColor(m_onColor);
	m_mainMenu->activate();
}

void MenuState::initGamemode() {

	m_gamemodeMenu = new MenuHandler();
	m_scene.addObject(m_gamemodeMenu);

	m_gamemodeMenu->addMenuSelector("gamemode");
	m_gamemodeMenu->addMenuSelectorItem("payload");
	m_gamemodeMenu->addMenuSelectorItem("deathmatch");
	m_gamemodeMenu->addMenuSelectorItem("team deathmatch");

	m_gamemodeMenu->addMenuSelector("time limit");
	m_gamemodeMenu->addMenuSelectorItem("0");
	m_gamemodeMenu->addMenuSelectorItem("01");
	m_gamemodeMenu->addMenuSelectorItem("022");
	m_gamemodeMenu->addMenuSelectorItem("0333");
	m_gamemodeMenu->setStaticSelection(true, 0);

	m_gamemodeMenu->addMenuSelector("score limit");
	m_gamemodeMenu->addMenuSelectorItem("1");
	m_gamemodeMenu->addMenuSelectorItem("11");
	m_gamemodeMenu->addMenuSelectorItem("122");
	m_gamemodeMenu->addMenuSelectorItem("1333");
	m_gamemodeMenu->setStaticSelection(true, 0);

	m_gamemodeMenu->addMenuSelector("respawntime");
	m_gamemodeMenu->addMenuSelectorItem("2");
	m_gamemodeMenu->addMenuSelectorItem("21");
	m_gamemodeMenu->addMenuSelectorItem("222");
	m_gamemodeMenu->addMenuSelectorItem("2333");
	m_gamemodeMenu->setStaticSelection(true, 0);

	m_gamemodeMenu->addMenuSelector("Gravity");
	m_gamemodeMenu->addMenuSelectorItem("none");
	m_gamemodeMenu->addMenuSelectorItem("moon");
	m_gamemodeMenu->addMenuSelectorItem("earth");
	m_gamemodeMenu->addMenuSelectorItem("mars");
	m_gamemodeMenu->setStaticSelection(true, 0);





	
	m_gamemodeMenu->setPosition(Vector3(5, -0.5, 0));
	m_gamemodeMenu->setFacingDirection(Vector3(-1, 0, 0));




}

void MenuState::initCharacter() {

}

void MenuState::initMap() {

}

void MenuState::initProfile() {
	m_profileMenu = new MenuHandler();
	m_scene.addObject(m_profileMenu);

	m_profileMenu->addMenuBox("kjasd test");
	m_profileMenu->addMenuBox("asdsad test");
	m_profileMenu->addMenuBox("team test");
	m_profileMenu->setPosition(Vector3(-5, -0.5, 0));
	m_profileMenu->setFacingDirection(Vector3(1, 0, 0));


}

void MenuState::initOptions() {

}






// CHANGEMENU

void MenuState::setMainSelect(bool active) {
	if (active) {
		m_mainMenu->activate(); 
		m_playerCamController->setTargets(m_mainMenu, m_mainMenu->getTarget());
		m_activeMenu = MAINMENU;

	}
	else {
		m_mainMenu->deActivate();



	}
}

void MenuState::setGamemodeSelect(bool active) {
	if (active) {
		m_gamemodeMenu->activate();
		m_playerCamController->setTargets(m_gamemodeMenu, m_gamemodeMenu->getTarget(), m_gamemodeMenu->getExtraTarget());
		m_activeMenu = STARTMENU;
		m_activeSubMenu = GAMEOPTIONSELECT;
	}
	else {
		m_gamemodeMenu->deActivate();


	}




}

void MenuState::setCharacterSelect(bool active) {
	if (active) {
		this->m_playerCamController->setTargets(this->playerMenu[0], this->playerMenu[1],
			this->playerMenu[2], this->playerMenu[3]);
		this->menuList[m_selector]->setLightColor(m_offColor);

		m_activeMenu = STARTMENU;

	}
	else {



	}

}

void MenuState::setMapSelect(bool active) {

}

void MenuState::setProfileMenu(bool active) {
	if (active) {
		m_profileMenu->activate();
		m_playerCamController->setTargets(m_profileMenu->getTarget());
		m_activeMenu = PROFILEMENU;

	}
	else {
		m_profileMenu->deActivate();


	}
}

void MenuState::setOptionsMenu(bool active) {


}



void MenuState::updateCamera() {

}



void MenuState::changeMenu(int change, int active)
{
	int max = 0;
	if (active == MAINMENU) {
		this->menuList[m_selector]->setLightColor(m_offColor);
		max = (int)this->menuList.size()-1;
	}
	if (active == OPTIONSMENU) {
		this->optionsMenuList[m_selector]->setLightColor(m_offColor);
		max = (int)this->menuList.size() - 1;
	}
	if (active == STARTMENU && m_activeSubMenu == MAPSELECT) {

		this->mapMenu[m_selector]->setLightColor(m_offColor);
		max = (int)this->mapMenu.size() - 1;
	}


	m_selector += change;
	if (m_selector < 0)
		m_selector = max;
	if (m_selector > max)
		m_selector = 0;
	if (active == MAINMENU) {
		this->menuList[m_selector]->setLightColor(m_onColor);
		m_playerCamController->setTargets(
			this->menuList[m_selector],
			nullptr,
			nullptr,
			nullptr
		);
	}
	if (active == OPTIONSMENU) {
		this->optionsMenuList[m_selector]->setLightColor(m_onColor);
		m_playerCamController->setTargets(
			this->optionsMenuList[m_selector],
			nullptr,
			nullptr,
			nullptr
		);
	}
	if (active == STARTMENU && m_activeSubMenu == MAPSELECT) {

		this->mapMenu[m_selector]->setLightColor(m_onColor);
		m_playerCamController->setTargets(
			this->mapMenu[m_selector],
			nullptr,
			nullptr,
			nullptr
		);
	}
	
}

