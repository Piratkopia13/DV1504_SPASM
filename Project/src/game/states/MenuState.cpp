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

	m_block = resMan.getFBXModel("block").getModel();
	m_menuBlockModel = resMan.getFBXModel("block").getModel();
	m_menuStartModel = resMan.getFBXModel("startButton").getModel();
	m_menuOptionsModel = resMan.getFBXModel("optionsButton").getModel();
	m_menuExitModel = resMan.getFBXModel("exitButton").getModel();
	m_playerModel = resMan.getFBXModel("fisk").getModel();
	m_backGroundModel = resMan.getFBXModel("menu_screen").getModel();
	


	m_onColor = Vector4(1.f, 1.f, 1.f, 1.f);
	m_offColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
	m_orangeColor = Vector4(1.0f, 1.0f, 0.0f, 1.0f);


	m_teamColors.push_back(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	m_teamColors.push_back(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	m_teamColors.push_back(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	m_teamColors.push_back(Vector4(0.0f, 1.0f, 1.0f, 1.0f));
	m_teamColors.push_back(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
	m_teamColors.push_back(Vector4(1.0f, 1.0f, 0.0f, 1.0f));

	m_app->getGameSettings().teamOneColor = m_teamColors[0];
	m_app->getGameSettings().teamTwoColor = m_teamColors[1];


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
	initCharacterModels();
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
	for (size_t i = 0; i < m_characterMenu.size(); i++)
		Memory::safeDelete(m_characterMenu[i]);
	Memory::safeDelete(m_profileMenu);
	Memory::safeDelete(m_optionsMenu);

	for (size_t i = 0; i < m_players.size(); i++) {
		Memory::safeDelete(m_players[i]);
	}

	for (size_t i = 0; i < m_playerMenuModels.size(); i++) {
		Memory::safeDelete(m_playerMenuModels[i]);

	}
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




	const Keyboard::KeyboardStateTracker& kbTracker = m_app->getInput().getKbStateTracker();
	static int keyboardPort = 0;
	if (kbTracker.pressed.D1) {
		keyboardPort = 0;
	}
	if (kbTracker.pressed.D2) {
		keyboardPort = 1;
	}
	if (kbTracker.pressed.D3) {
		keyboardPort = 2;
	}
	if (kbTracker.pressed.D4) {
		keyboardPort = 3;
	}


	for (int i = 0; i < 4; i++) {
		const GamePad::State& padState = m_app->getInput().getGamePadState(i);
		const GamePad::ButtonStateTracker& padTracker = m_app->getInput().getGpStateTracker(i);
		
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


		


		if (i == keyboardPort) {



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
			if (kbTracker.pressed.F) {
				static int camP = 0;
				camP++;
				if (camP > 3)
					camP = 0;
				if (camP == 0)
					m_playerCamController->setPosition(Vector3(0, 0, 0));
				if (camP == 1)
					m_playerCamController->setPosition(Vector3(0, 15, 0));
				if (camP == 2)
					m_playerCamController->setPosition(Vector3(10, 15, 10));
				if (camP == 3)
					m_playerCamController->setPosition(Vector3(-10, 15, -10));

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
					if (down) {
						m_mainMenu->next();
						updateCamera();
					}
					if (up) {
						m_mainMenu->back();
						updateCamera();
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
				case STARTMENU: {
					switch (m_activeSubMenu) {
						case GAMEOPTIONSELECT: {
							if (a) {
								Application::getInstance()->getGameSettings().gamemode = m_gamemodeMenu->getOptionAt(GAMEMODE);
								Application::getInstance()->getGameSettings().scoreLimit = m_gamemodeMenu->getOptionAt(SCORELIMIT);
								Application::getInstance()->getGameSettings().timeLimit = m_gamemodeMenu->getOptionAt(TIMELIMIT);
								Application::getInstance()->getGameSettings().respawnTime = m_gamemodeMenu->getOptionAt(RESPAWNTIME);
								Application::getInstance()->getGameSettings().gravityScale = m_gamemodeMenu->getOptionAt(GRAVITY);

								


								setGamemodeSelect(false);
								setCharacterSelect(true);
							}
							if (b) {
								setGamemodeSelect(false);
								setMainSelect(true);							

							}
							if (down) {
								m_gamemodeMenu->next();
							}
							if (up) {
								m_gamemodeMenu->back();
							}

							if (right) {
								m_gamemodeMenu->right();
							}
							if (left) {
								m_gamemodeMenu->left();
							}

							updateCamera();



						}	break;
						case PLAYERSELECT: {

							// controller not online by default
							int spot = -1;

							//check if controller is online
							for (size_t u = 0; u < m_players.size(); u++) {
								if (m_players[u]) {
									if (m_players[u]->port == i) {
										spot = u;
									} 
								}
							}
							Logger::Warning("contrllerID: " + std::to_string(i));
							Logger::log("online: "+std::to_string(spot));

							if (a) {
								if (spot == -1) {
									//check for empty space
									for (size_t u = 0; u < m_players.size(); u++) {
										if (m_players[u] == nullptr) {											
											spot = u;
											break;
											
										}
									}
									Logger::log("check for empty: " + std::to_string(spot));
									if (spot != -1) {
										int asd = 0;
									}
									Application::GameSettings::player* temp = new Application::GameSettings::player;
									if (spot == -1) {
										//if spot not found, create one
										spot = m_players.size();
										Logger::log("creating Spot at: "+std::to_string(spot));
										m_players.push_back(temp);

									}
									initCharacter(spot);
									m_characterMenu[spot]->activate();

									temp->port = i;
									if (Application::getInstance()->getGameSettings().gamemode == DEATHMATCH) {
										temp->team = spot;
										temp->color = m_teamColors[spot];
										m_characterMenu[spot]->setOptionAt(1, spot);
										m_playerMenuModels[spot]->setLightColor(m_teamColors[spot]);

									}
									else {
										temp->team = spot % 2;
										if (temp->team == 0) {
											temp->color = m_app->getGameSettings().teamOneColor;
											
										}
										if (temp->team == 1) {
											temp->color = m_app->getGameSettings().teamTwoColor;

										}
										temp->color = m_teamColors[spot % 2];
										m_characterMenu[spot]->setOptionAt(1, spot);
										m_playerMenuModels[spot]->setLightColor(m_teamColors[temp->team]);


									}

									temp->model = 0;
									temp->profile = &Application::getInstance()->getProfiles()[0];
									temp->ready = false;
									
									m_players[spot] = temp;
									Logger::log("adding to spot: " + std::to_string(spot) +" with port: " + std::to_string(i));

								}
								else {
									if(!m_players[spot]->ready) {
										m_players[spot]->ready = true;
										m_playerMenuModels[spot]->setLightColor(m_teamColors[m_players[spot]->team] * 3);
									}
									else {
										startGame();
										break;

									}
								}
							}

							if (b) {
								if (spot == -1) {

									//controller not online, go back to game settings
									for (size_t u = 0; u < m_players.size(); u++) {
										Memory::safeDelete(m_players[u]);
										removeCharacter(u);
										m_playerMenuModels[u]->setLightColor(m_offColor);
									}
									m_players.clear();
									setCharacterSelect(false);
									setGamemodeSelect(true);

								}
								else {
									if (m_players[spot]->ready) {
										m_players[spot]->ready = false;
										m_playerMenuModels[spot]->setLightColor(m_teamColors[m_players[spot]->team]);
									}
									else {

										Memory::safeDelete(m_players[spot]);
										m_players[spot] = nullptr;
										removeCharacter(spot);
										m_playerMenuModels[spot]->setLightColor(m_offColor);
									}

								}


							}


							if (down) {
								if (spot > -1) {
									if (!m_players[spot]->ready) {
										m_characterMenu[spot]->next();
									}
								}


							}

							if (up) {
								if (spot > -1) {
									if (!m_players[spot]->ready) {
										m_characterMenu[spot]->back();
									}


								}




							}
							if (right) {
								if (spot > -1) {
									if (!m_players[spot]->ready) {

										m_characterMenu[spot]->right();
										
									}
								}

							}
							if (left) {
								if (spot > -1) {
									if (!m_players[spot]->ready) {

										m_characterMenu[spot]->left();
										
									}
								}

							}

							if (left || right) {
								if (spot > -1) {
									if (!m_players[spot]->ready) {
										size_t option = m_characterMenu[spot]->getActive();
										if (Application::getInstance()->getGameSettings().gamemode == DEATHMATCH) {


											if (option == 1) {
												size_t color = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModels[spot]->setLightColor(m_teamColors[color]);
												m_players[spot]->color = m_teamColors[color];
											}





											if (option == 2) {
												m_players[spot]->model = m_characterMenu[spot]->getOptionAt(2);
												if (m_players[spot]->model == 0) {
													Logger::log("model: 0");
													m_playerMenuModels[spot]->setModel(m_playerModel);

												}
												if (m_players[spot]->model == 1) {
													Logger::log("model: 1");
													m_playerMenuModels[spot]->setModel(m_block);
												}



											}

										}
										else if (Application::getInstance()->getGameSettings().gamemode != DEATHMATCH) {

											if (option == 1) {
												m_players[spot]->team = m_characterMenu[spot]->getOptionAt(option);
												if (m_players[spot]->team == 0) {
													m_players[spot]->color = m_app->getGameSettings().teamOneColor;
												}
												if (m_players[spot]->team == 1) {
													m_players[spot]->color = m_app->getGameSettings().teamTwoColor;
												}
												m_playerMenuModels[spot]->setLightColor(m_players[spot]->color);
											}

											if (option == 2) {
												size_t team = m_players[spot]->team;
												size_t color = m_characterMenu[spot]->getOptionAt(option);
												if (team == 0) {
													m_app->getGameSettings().teamOneColor = m_teamColors[color];
												}
												if (team == 1) {
													m_app->getGameSettings().teamTwoColor = m_teamColors[color];
												}

												for (size_t p = 0; p < m_players.size(); p++) {
													if (m_players[p]->team == team && p != spot) {
														m_characterMenu[p]->setOptionAt(2, color);
														if (team == 0) {
															m_players[p]->color = m_app->getGameSettings().teamOneColor;
														}
														if (team == 1) {
															m_players[p]->color = m_app->getGameSettings().teamTwoColor;
														}
														if (m_players[p]->ready)
															m_playerMenuModels[p]->setLightColor(m_players[p]->color * 3);
														else
															m_playerMenuModels[p]->setLightColor(m_players[p]->color);
													}
												}
												m_playerMenuModels[spot]->setLightColor(m_teamColors[color]);
												m_players[spot]->color = m_teamColors[color];
											}

											if (option == 3) {
												m_players[spot]->model = m_characterMenu[spot]->getOptionAt(3);

												if (m_players[spot]->model == 0) {
													m_playerMenuModels[spot]->setModel(m_playerModel);

												}
												if (m_players[spot]->model == 1) {
													m_playerMenuModels[spot]->setModel(m_block);
												}



											}

										}
									}

								}


							}





						}break;
							

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

				}	break;

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
	m_gamemodeMenu->addMenuSelectorItem("no limit");
	m_gamemodeMenu->addMenuSelectorItem("1 min");
	m_gamemodeMenu->addMenuSelectorItem("2 min");
	m_gamemodeMenu->addMenuSelectorItem("3 min");
	m_gamemodeMenu->addMenuSelectorItem("5 min");
	m_gamemodeMenu->addMenuSelectorItem("10 min");
	m_gamemodeMenu->setStaticSelection(true, 0);

	m_gamemodeMenu->addMenuSelector("score limit");
	m_gamemodeMenu->addMenuSelectorItem("50");
	m_gamemodeMenu->addMenuSelectorItem("75");
	m_gamemodeMenu->addMenuSelectorItem("100");
	m_gamemodeMenu->addMenuSelectorItem("150");
	m_gamemodeMenu->addMenuSelectorItem("250");
	m_gamemodeMenu->setStaticSelection(true, 0);

	m_gamemodeMenu->addMenuSelector("respawntime");
	m_gamemodeMenu->addMenuSelectorItem("2 s");
	m_gamemodeMenu->addMenuSelectorItem("4 s");
	m_gamemodeMenu->addMenuSelectorItem("8 s");
	m_gamemodeMenu->addMenuSelectorItem("instant");
	m_gamemodeMenu->setStaticSelection(true, 0);

	m_gamemodeMenu->addMenuSelector("Gravity");
	m_gamemodeMenu->addMenuSelectorItem("earth");
	m_gamemodeMenu->addMenuSelectorItem("moon");
	m_gamemodeMenu->addMenuSelectorItem("mars");
	m_gamemodeMenu->addMenuSelectorItem("none");
	m_gamemodeMenu->setStaticSelection(true, 0);


	m_gamemodeMenu->setStep(0.1);


	
	m_gamemodeMenu->setPosition(Vector3(5, 0, 0));
	m_gamemodeMenu->setFacingDirection(Vector3(-1, 0, 0));




}

void MenuState::initCharacterModels() {
	Vector3 charMid(7, 2, -7);

	if (m_playerMenuModels.size() > 0) {
		for (size_t i = 0; i < m_playerMenuModels.size(); i++)
			Memory::safeDelete(m_playerMenuModels[i]);
		m_playerMenuModels.clear();
	}
	

	for (size_t i = 0; i < 4; i++) {

		MenuItem* temp = new MenuItem(m_playerModel,charMid + Vector3(0,0,-1.0f)*((float)i-2.0f) * 1.4f);
		temp->getTransform().setRotations(Vector3(0,-1.55,0));
		temp->setLightColor(m_offColor);
		m_scene.addObject(temp);
		m_playerMenuModels.push_back(temp);
	}
}

void MenuState::initCharacter(size_t spot) {
	static Vector3 charMid(5, -2, -7);

	if (m_characterMenu.size() == 0) {
		for (size_t i = 0; i < 4; i++) {
			m_characterMenu.push_back(new MenuHandler());
			m_scene.addObject(m_characterMenu[i]);
		}
	}
	

	
	MenuHandler* temp = m_characterMenu[spot];
	temp->reset();
	temp->setPosition(charMid + Vector3(0.0f, 0.0f, -1.0f)*((float)spot - 2.0f) * 2.0f);
	
	temp->setFacingDirection(Vector3(-1.0, 0, 0));
	temp->addMenuSelector("profile");
	temp->addMenuSelectorItem(Application::getInstance()->getProfiles()[0].name);
	for (size_t p = 1; p < Application::getInstance()->getProfiles().size(); p++) {
		temp->addMenuSelectorItem(Application::getInstance()->getProfiles()[p].name);
	}
	temp->setStaticSelection(true, 0);
	if (Application::getInstance()->getGameSettings().gamemode == DEATHMATCH) {
		temp->addMenuSelector("Color");
		temp->addMenuSelectorItem("red");
		temp->addMenuSelectorItem("green");
		temp->addMenuSelectorItem("blue");
		temp->addMenuSelectorItem("light Blue");
		temp->addMenuSelectorItem("purple");
		temp->addMenuSelectorItem("yellow");
		temp->setStaticSelection(true, 0);

	}
	else {
		temp->addMenuSelector("team");
		temp->addMenuSelectorItem("team one");
		temp->addMenuSelectorItem("team two");
		temp->setStaticSelection(true, 0);

		temp->addMenuSelector("Team Color");
		temp->addMenuSelectorItem("red");
		temp->addMenuSelectorItem("green");
		temp->addMenuSelectorItem("blue");
		temp->addMenuSelectorItem("light blue");
		temp->addMenuSelectorItem("purple");
		temp->addMenuSelectorItem("yellow");
		temp->setStaticSelection(true, 0);
	}

	temp->addMenuSelector("Bot");
	temp->addMenuSelectorItem("Fisk");
	temp->addMenuSelectorItem("trashbot");
	temp->setStaticSelection(true, 0);
}

void MenuState::removeCharacter(size_t spot) {
	m_characterMenu[spot]->reset();

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
		m_activeMenu = STARTMENU;
		m_activeSubMenu = GAMEOPTIONSELECT;
		updateCamera();
	}
	else {
		m_gamemodeMenu->deActivate();


	}




}

void MenuState::setCharacterSelect(bool active) {
	if (active) {

		for (size_t i = 0; i < 4; i++) {
			
		}
		
		m_activeMenu = STARTMENU;
		m_activeSubMenu = PLAYERSELECT;

		m_playerCamController->setPosition(Vector3(0, 0, -7));
		m_playerCamController->setTargets(nullptr);
		m_playerCamController->setTarget(Vector3(5,0,-7));

	}
	else {
		for (size_t i = 0; i < m_characterMenu.size(); i++) {
			//if(m_characterMenu[i])
				//m_characterMenu[i]->deActivate();
		}
		m_playerCamController->setPosition(Vector3(0,0,0));

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
	if (m_activeMenu == MAINMENU) {
		m_playerCamController->setTargets(m_mainMenu, m_mainMenu->getTarget());
	}
	if (m_activeMenu == STARTMENU) {
		if (m_activeSubMenu == GAMEOPTIONSELECT) {
			m_playerCamController->setTargets(m_gamemodeMenu, m_gamemodeMenu->getTarget(), m_gamemodeMenu->getExtraTarget());
		}
		if (m_activeSubMenu == PLAYERSELECT) {

			//m_playerCamController->setTargets(m_characterMenu[0], m_characterMenu[1], m_characterMenu[2], m_characterMenu[3]);
		}
		if (m_activeSubMenu == MAPSELECT) {

		}

	}
	if (m_activeMenu == PROFILEMENU) {

	}
	if (m_activeMenu == OPTIONSMENU) {

	}









}

void MenuState::startGame() {
	Application::GameSettings& settings = Application::getInstance()->getGameSettings();



	requestStackPop();
	requestStackPush(States::Game);

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











/*

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
			m_playerCamController->setPosition(Vector3(0,0,0));

			setCharacterSelect(false);
			setGamemodeSelect(true);
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



*/