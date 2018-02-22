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
	m_info = GameInfo::getInstance();
	// Set up camera with controllers
	m_playerCamController = std::make_unique<PlayerCameraController>(&m_cam);

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



	for(size_t i = 0; i < m_info->botHeadNames.size();i++) 
		m_playerHeadModels.push_back(resMan.getFBXModel("fisk/"+m_info->botHeadNames[i] + "_head").getModel());

	for (size_t i = 0; i < m_info->botBodyNames.size(); i++)
		m_playerBodyModels.push_back(resMan.getFBXModel("fisk/" + m_info->botBodyNames[i] + "_body").getModel());
	for (size_t i = 0; i < m_info->botLegNames.size(); i++)
		m_playerLegModels.push_back(resMan.getFBXModel("fisk/" + m_info->botLegNames[i] + "_body").getModel());

	
	for (size_t i = 0; i < m_info->botArmNames.size(); i++) {
		m_playerArmLModels.push_back(resMan.getFBXModel("fisk/" + m_info->botArmNames[i] + "_armL").getModel());
		m_playerArmRModels.push_back(resMan.getFBXModel("fisk/" + m_info->botArmNames[i] + "_armR").getModel());
	}



	m_block = resMan.getFBXModel("block").getModel();
	


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
	
	
	this->background = new MenuItem(m_backGroundModel, Vector3(0.f, -2.3f, 0.5f));
	this->background->m_useColor = 1;
	this->background->setLightColor(Vector4(0.5f,0.5f,0.5f,0.5f));
	//m_scene.addObject(background);

	//MAINMENU
	initMain();

	//STARTMENU
	initGamemode();
	initCharacterModels();
	m_mapMenu = nullptr;

	//CUSTOMISABILITY
	initProfile();
	initOptions();


	m_playerCamController->setUseExtraZ(false);
	m_playerCamController->setTargets(m_mainMenu);
	m_playerCamController->setOffset(Vector3(0,0,0));
	m_playerCamController->setMoving(false);
	m_playerCamController->setPosition(Vector3(0,0,0));
	m_playerCamController->setFollowSpeed(8);

	// Sound
	m_app->getResourceManager().getSoundManager()->playAmbientSound(SoundManager::Ambient::Theme, true);
	
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
	Memory::safeDelete(m_mapMenu);

	for (size_t i = 0; i < m_playerz.size(); i++) {
		Memory::safeDelete(m_playerz[i]);
	}
	for (size_t i = 0; i < m_playerMenuModelz.size(); i++) {
		m_playerMenuModelz[i].clear();

	}


	delete this->background;
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
				case MAINMENU: {
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
						// Sound
						m_app->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Select);
					}
				}break;	

				case STARTMENU: {
					switch (m_activeSubMenu) {
						case GAMEOPTIONSELECT: {
							if (a) {
								
								
								m_info->gameSettings.gameMode = m_gamemodeMenu->getOptionAt(GAMEMODE);
								m_info->gameSettings.scoreLimit = m_gamemodeMenu->getOptionAt(SCORELIMIT);
								m_info->gameSettings.timelimit = m_gamemodeMenu->getOptionAt(TIMELIMIT);
								m_info->gameSettings.respawnTime = m_gamemodeMenu->getOptionAt(RESPAWNTIME);
								m_info->gameSettings.gravity = m_gamemodeMenu->getOptionAt(GRAVITY);
								m_info->getPlayers().clear();
								m_info->gameSettings.teams.clear();
								

								if (m_info->gameSettings.gameMode == DEATHMATCH) {
									for (size_t u = 0; u < 4; u++)
										m_info->gameSettings.teams.push_back({ u, 0});

								}
								else {
									for (size_t u = 0; u < 2; u++)
										m_info->gameSettings.teams.push_back({ u, 0});
								}

								initMap();

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
							for (size_t u = 0; u < m_playerz.size(); u++) {
								if (m_playerz[u]) {
									if (m_playerz[u]->player.port == i) {
										spot = u;
									} 
								}
							}
							if (a) {
								if (spot == -1) {
									//check for empty space
									for (size_t u = 0; u < m_playerz.size(); u++) {
										if (m_playerz[u] == nullptr) {											
											spot = u;
											break;										
										}
									}
									MenuPlayer* temp = new MenuPlayer;
									if (spot == -1) {
										//if spot not found, create one
										spot = m_playerz.size();
										m_playerz.push_back(temp);

									}
									initCharacter(spot);
									m_characterMenu[spot]->activate();

									temp->player.port = i;
									if (m_info->gameSettings.gameMode == DEATHMATCH) {
										temp->player.team = spot;
										temp->player.color = spot;
										m_characterMenu[spot]->setOptionAt(1, spot);

									}
									else {
										temp->player.team = spot * 0.5;
										temp->player.color = m_info->gameSettings.teams[temp->player.team].color;
										m_characterMenu[spot]->setOptionAt(1, temp->player.team);
										m_characterMenu[spot]->setOptionAt(2, temp->player.color);
									}

									temp->player.hue = 0;
									temp->player.headModel = 0;
									temp->player.bodyModel = 0;
									temp->player.armModel = 0;
									temp->player.legModel = 0;
									temp->player.currentProfile = &m_info->getProfiles()[0];
									temp->ready = false;
									initCharacterModel(spot);
									m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(temp->player.color,temp->player.hue));
									
									m_playerz[spot] = temp;
								}
								else {
									if(!m_playerz[spot]->ready) {
										m_playerz[spot]->ready = true;
										m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(m_playerz[spot]->player.color, m_playerz[spot]->player.hue) * 4);
									}
									else {
										int ready = 0;
										for (size_t p = 0; p < m_playerz.size(); p++) {
											if (m_playerz[p]) {
												if (m_playerz[p]->ready)
													ready++;
											}
											else
												ready++;
										}
										if (ready == m_playerz.size()) {
											setCharacterSelect(false);
											setMapSelect(true);

											updateCamera();
										}
									}
								}
							}

							if (b) {
								if (spot == -1) {
									//controller not online, go back to game settings
									for (size_t u = 0; u < m_playerz.size(); u++) {
										Memory::safeDelete(m_playerz[u]);
										removeCharacter(u);
										m_playerMenuModelz[u].setLight(m_offColor);
									}
									m_playerz.clear();
									setCharacterSelect(false);
									setGamemodeSelect(true);
								}
								else {
									if (m_playerz[spot]->ready) {
										m_playerz[spot]->ready = false;
										m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(m_playerz[spot]->player.color,m_playerz[spot]->player.hue));
									}
									else {
										Memory::safeDelete(m_playerz[spot]);
										m_playerz[spot] = nullptr;
										removeCharacter(spot);
										m_playerMenuModelz[spot].setLight(m_offColor);
									}
								}
							}
							if (down) {
								if (spot > -1) {
									if (!m_playerz[spot]->ready) {
										m_characterMenu[spot]->next();
									}
								}
							}
							if (up) {
								if (spot > -1) {
									if (!m_playerz[spot]->ready) {
										m_characterMenu[spot]->back();
									}
								}
							}
							if (right) {
								if (spot > -1) {
									if (!m_playerz[spot]->ready) {

										m_characterMenu[spot]->right();
										
									}
								}
							}
							if (left) {
								if (spot > -1) {
									if (!m_playerz[spot]->ready) {

										m_characterMenu[spot]->left();
										
									}
								}
							}

							if (left || right) {
								if (spot > -1) {
									if (!m_playerz[spot]->ready) {
										size_t option = m_characterMenu[spot]->getActive();
										if (m_info->gameSettings.gameMode == DEATHMATCH) {

											if (option == 1) {
												m_playerz[spot]->player.color = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(m_playerz[spot]->player.color, m_playerz[spot]->player.hue));
											}
											if (option == 2) {
												m_playerz[spot]->player.hue = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(m_playerz[spot]->player.color, m_playerz[spot]->player.hue));
											}

											if (option == 3) {
												m_playerz[spot]->player.headModel = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].head->setModel(m_playerHeadModels[m_playerz[spot]->player.headModel]);
											}
											if (option == 4) {
												m_playerz[spot]->player.bodyModel = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].body->setModel(m_playerBodyModels[m_playerz[spot]->player.bodyModel]);
											}
											if (option == 5) {
												m_playerz[spot]->player.legModel = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].legs->setModel(m_playerLegModels[m_playerz[spot]->player.legModel]);
											}
											if (option == 6) {
												m_playerz[spot]->player.armModel = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].armL->setModel(m_playerArmLModels[m_playerz[spot]->player.armModel]);
												m_playerMenuModelz[spot].armR->setModel(m_playerArmRModels[m_playerz[spot]->player.armModel]);
											}

										}
										else if (m_info->gameSettings.gameMode != DEATHMATCH) {

											if (option == 1) {
												m_playerz[spot]->player.team = m_characterMenu[spot]->getOptionAt(option);
												m_playerz[spot]->player.color = m_info->gameSettings.teams[m_playerz[spot]->player.team].color;
												m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(m_playerz[spot]->player.color,m_playerz[spot]->player.hue));
												m_characterMenu[spot]->setOptionAt(2, m_info->gameSettings.teams[m_playerz[spot]->player.team].color);
											}

											if (option == 2) {
												size_t team = m_playerz[spot]->player.team;
												size_t color = m_characterMenu[spot]->getOptionAt(option);
												m_info->gameSettings.teams[team].color = color;

												for (size_t p = 0; p < m_playerz.size(); p++) {
													if (m_playerz[p]) {
														if (m_playerz[p]->player.team == team && p != spot) {
															m_characterMenu[p]->setOptionAt(2, color);
															m_playerz[p]->player.color = color;
															
															if (m_playerz[p]->ready)
																m_playerMenuModelz[p].setLight(
																	m_info->getDefaultColor(m_playerz[p]->player.color, m_playerz[p]->player.hue) * 4);
															else
																m_playerMenuModelz[p].setLight(
																	m_info->getDefaultColor(m_playerz[p]->player.color, m_playerz[p]->player.hue));
														}

													}
												}
												m_playerz[spot]->player.color = color;
												m_playerMenuModelz[spot].setLight(
													m_info->getDefaultColor(color, m_playerz[spot]->player.hue));
											}
											if (option == 3) {
												m_playerz[spot]->player.hue = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(m_playerz[spot]->player.color, m_playerz[spot]->player.hue));
											}
											if (option == 4) {
												m_playerz[spot]->player.headModel = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].head->setModel(m_playerHeadModels[m_playerz[spot]->player.headModel]);
											}
											if (option == 5) {
												m_playerz[spot]->player.bodyModel = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].body->setModel(m_playerBodyModels[m_playerz[spot]->player.bodyModel]);
											}
											if (option == 6) {
												m_playerz[spot]->player.legModel = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].legs->setModel(m_playerLegModels[m_playerz[spot]->player.legModel]);
											}
											if (option == 7) {
												m_playerz[spot]->player.armModel = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].armL->setModel(m_playerArmLModels[m_playerz[spot]->player.armModel]);
												m_playerMenuModelz[spot].armR->setModel(m_playerArmRModels[m_playerz[spot]->player.armModel]);
											}





										}
									}

								}


							}





						}break;
						case MAPSELECT: {
							if (right) {
								m_mapMenu->right();
							}
							if (left) {
								m_mapMenu->left();
							}
							updateCamera();
							if (a) {	
								// START GAME
								//set map: gamemode * menu selection
								m_app->getGameSettings().level = m_mapMenu->getOptionAt(0)+m_app->getGameSettings().gamemode*10;
								

								startGame();
							}
							if (b) {
								setMapSelect(false);
								setCharacterSelect(true);
								updateCamera();
							}

						}break;									
					}break;
				}break;

				case PROFILEMENU: {
					if (a) {

					}
					if (b) {
						setProfileMenu(false);
						setMainSelect(true);
					}

				}break;

				case OPTIONSMENU: {
					if (b) {
						

					}
					if (down) {
						
					}
					if (up) {
						
					}

				}break;
				
				default:
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

	m_gamemodeMenu->addMenuSelector("player Health");
	m_gamemodeMenu->addMenuSelectorItem("100");
	m_gamemodeMenu->addMenuSelectorItem("200");
	m_gamemodeMenu->addMenuSelectorItem("10");
	m_gamemodeMenu->addMenuSelectorItem("50");
	m_gamemodeMenu->setStaticSelection(true, 0);


	m_gamemodeMenu->setStep(0.0);

	m_gamemodeMenu->setOnColor(m_onColor);
	
	m_gamemodeMenu->setPosition(Vector3(5, -0.5, 0));
	m_gamemodeMenu->setFacingDirection(Vector3(-1, 0, 0));




}

void MenuState::initCharacterModels() {
		Vector3 charMenuMid(7, -2, -9);

	for (size_t i = 0; i < 4; i++) {
		Vector3 charMid = Vector3(0, 0, -1.0f)*((float)i - 1.5f) * 3.5f + charMenuMid;
		PlayerMenuModel temp;

		
		temp.head = new MenuItem(m_playerHeadModels[0], charMid + Vector3(0, 0.8f, 0));
		temp.body = new MenuItem(m_playerBodyModels[0], charMid + Vector3(0, 0.3f, 0));
		temp.legs = new MenuItem(m_playerLegModels[0],  charMid - Vector3(0, 0.9f, 0));
		temp.armL = new MenuItem(m_playerArmLModels[0], charMid + Vector3(0, 0.2f, -0.1f));
		temp.armR = new MenuItem(m_playerArmRModels[0], charMid + Vector3(0, 0.2f, 0.1f));
	

		temp.head->getTransform().setRotations(Vector3(0, -1.55, 0));
		temp.body->getTransform().setRotations(Vector3(0, -1.55, 0));
		temp.legs->getTransform().setRotations(Vector3(0, -1.55, 0));
		temp.armL->getTransform().setRotations(Vector3(0, -1.55, 0));
		temp.armR->getTransform().setRotations(Vector3(0, -1.55, 0));

		temp.head->getTransform().setScale(1.4f);
		temp.body->getTransform().setScale(1.4f);
		temp.legs->getTransform().setScale(1.4f);
		temp.armL->getTransform().setScale(1.4f);
		temp.armR->getTransform().setScale(1.4f);

		m_scene.addObject(temp.head);
		m_scene.addObject(temp.body);
		m_scene.addObject(temp.legs);
		m_scene.addObject(temp.armL);
		m_scene.addObject(temp.armR);
		temp.setLight(m_offColor);
		temp.reset();
		m_playerMenuModelz.push_back(temp);
		
	}


}

void MenuState::initCharacterModel(size_t spot) {
	Vector3 charMenuMid(7, -2, -9);
	if (m_info->gameSettings.gameMode == DEATHMATCH)
		charMenuMid = Vector3(7,-1.3,-9);

	Vector3 charMid = Vector3(0, 0, -1.0f)*((float)spot - 1.5f) * 3.5f + charMenuMid;
	m_playerMenuModelz[spot].head->setPosition( charMid + Vector3(0, 0.8f, 0));
	m_playerMenuModelz[spot].body->setPosition( charMid + Vector3(0, 0.3f, 0));
	m_playerMenuModelz[spot].legs->setPosition (charMid - Vector3(0, 0.9f, 0));
	m_playerMenuModelz[spot].armL->setPosition( charMid + Vector3(0, 0.3f, -0.0f));
	m_playerMenuModelz[spot].armR->setPosition( charMid + Vector3(0, 0.3f, 0.0f));



	m_playerMenuModelz[spot].head->setModel(m_playerHeadModels[0]);
	m_playerMenuModelz[spot].body->setModel(m_playerBodyModels[0]);
	m_playerMenuModelz[spot].legs->setModel(m_playerLegModels[0]);
	m_playerMenuModelz[spot].armL->setModel(m_playerArmLModels[0]);
	m_playerMenuModelz[spot].armR->setModel(m_playerArmRModels[0]);
	m_playerMenuModelz[spot].setLight(m_offColor);

	
}

void MenuState::initCharacter(size_t spot) {
	static Vector3 charMid(7, 1, -9);

	if (m_characterMenu.size() == 0) {
		for (size_t i = 0; i < 4; i++) {
			m_characterMenu.push_back(new MenuHandler());
			m_scene.addObject(m_characterMenu[i]);
		}
	}
	

	
	MenuHandler* temp = m_characterMenu[spot];
	temp->reset();
	temp->setPosition(charMid + Vector3(0.0f, 0.0f, -1.0f)*((float)spot - 1.5f) * 3.5f);
	temp->setSize(0.7f);
	temp->setFacingDirection(Vector3(-1.0, 0, 0));
	temp->setStep(0.4f);

	//PROFILE
	temp->addMenuSelector("profile");
	temp->addMenuSelectorItem(m_info->getProfiles()[0].getName());
	for (size_t p = 1; p < m_info->getProfiles().size(); p++) {
		temp->addMenuSelectorItem(m_info->getProfiles()[p].getName());
	}
	temp->setStaticSelection(true, 0);

	//TEAMS & COLORS
	if (m_info->gameSettings.gameMode == DEATHMATCH) {
		temp->addMenuSelector("Color");
		
		for (size_t i = 0; i < m_info->colorNames.size(); i++) {
			temp->addMenuSelectorItem(m_info->colorNames[i]);

		}
		temp->setStaticSelection(true, 0);

	}
	else {
		temp->addMenuSelector("team");
		temp->addMenuSelectorItem("1");
		temp->addMenuSelectorItem("2");
		temp->setStaticSelection(true, 0);

		temp->addMenuSelector("Team Color");
		for (size_t i = 0; i < m_info->colorNames.size(); i++) {
			temp->addMenuSelectorItem(m_info->colorNames[i]);
		}
		temp->setStaticSelection(true, 0);
	}

	// COLOR HUE
	temp->addMenuSelector("hue");
	for (size_t i = 0; i < m_info->colorHues.size(); i++) {
		temp->addMenuSelectorItem(m_info->colorHues[i]);
	}
	temp->setStaticSelection(true, 0);

	temp->addMenuSelector("<         >");
	for (size_t i = 0; i < m_info->botHeadNames.size(); i++) {
		temp->addMenuSelectorItem("            ");
	}
	temp->setStaticSelection(true, 0);

	temp->addMenuSelector("<         >");
	for (size_t i = 0; i < m_info->botBodyNames.size(); i++) {
		temp->addMenuSelectorItem("            ");
	}
	temp->setStaticSelection(true, 0);

	temp->addMenuSelector("<         >");
	for (size_t i = 0; i < m_info->botLegNames.size(); i++) {
		temp->addMenuSelectorItem("            ");
	}
	temp->setStaticSelection(true, 0);

	//temp->addMenuSelector("bot arms");
	//for (size_t i = 0; i < m_info->botArmNames.size(); i++) {
	//	temp->addMenuSelectorItem("i           i");
	//}
	//temp->setStaticSelection(true, 0);


}

void MenuState::removeCharacter(size_t spot) {
	m_characterMenu[spot]->reset();
	m_playerMenuModelz[spot].reset();
	
}

void MenuState::initMap() {

	if (!m_mapMenu) {
		m_mapMenu = new MenuHandler();
		m_scene.addObject(m_mapMenu);

	}
	else
		m_mapMenu->reset();


	m_mapMenu->addMenuSelector("Map");

	if (m_info->gameSettings.gameMode == PAYLOAD) {
		m_mapMenu->addMenuSelectorItem("Domination 1");
		m_mapMenu->addMenuSelectorItem("Domination 2");
		m_mapMenu->addMenuSelectorItem("Domination 3");
		m_mapMenu->addMenuSelectorItem("Domination 4");
	}
	if (m_info->gameSettings.gameMode == DEATHMATCH) {
		m_mapMenu->addMenuSelectorItem("Deathmatch 1");
		m_mapMenu->addMenuSelectorItem("Deathmatch 2");
		m_mapMenu->addMenuSelectorItem("Deathmatch 3");
		m_mapMenu->addMenuSelectorItem("Deathmatch 4");
	}
	if (m_info->gameSettings.gameMode == TEAMDEATHMATCH) {
		m_mapMenu->addMenuSelectorItem("teamDeath 1");
		m_mapMenu->addMenuSelectorItem("teamDeath 2");
		m_mapMenu->addMenuSelectorItem("teamDeath 3");
		m_mapMenu->addMenuSelectorItem("teamDeath 4");
	}
	m_mapMenu->setStaticSelection(true, 0);

	m_mapMenu->setPosition(Vector3(0,0,-12));
	m_mapMenu->setFacingDirection(Vector3(0,0,1));
	m_mapMenu->setStep(0.1);
	

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

		

	}
	else {
		for (size_t i = 0; i < m_characterMenu.size(); i++) {
		
		}
		m_playerCamController->setPosition(Vector3(0, 0, 0));
	}
}

void MenuState::setMapSelect(bool active) {
	if (active) {
		m_activeMenu = STARTMENU;
		m_activeSubMenu = MAPSELECT;

	}
	else {
		for (size_t i = 0; i < m_playerz.size(); i++) {
			m_playerz[i]->ready = false;
		}
	
	}
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

			m_playerCamController->setPosition(Vector3(0, 0, -9));
			m_playerCamController->setTargets(nullptr);
			m_playerCamController->setTarget(Vector3(5, 0, -9));
			//m_playerCamController->setTargets(m_characterMenu[0], m_characterMenu[1], m_characterMenu[2], m_characterMenu[3]);
		}
		if (m_activeSubMenu == MAPSELECT) {

			m_playerCamController->setPosition(Vector3(0, 0, -9));
			m_playerCamController->setTargets(m_mapMenu, m_mapMenu->getTarget(), m_mapMenu->getExtraTarget());

		}

	}
	if (m_activeMenu == PROFILEMENU) {

	}
	if (m_activeMenu == OPTIONSMENU) {

	}









}

void MenuState::startGame() {
	Application::GameSettings& settings = Application::getInstance()->getGameSettings();
	for (size_t i = 0; i < m_playerz.size(); i++) {
		if(m_playerz[i])
			m_info->addPlayer(m_playerz[i]->player);
	}

	// Sound
	m_app->getResourceManager().getSoundManager()->stopAmbientSound(SoundManager::Ambient::Theme);
	requestStackPop();
	requestStackPush(States::Game);
}

