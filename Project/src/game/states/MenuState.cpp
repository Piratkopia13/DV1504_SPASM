#include "MenuState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MenuState::MenuState(StateStack& stack) 
	: State(stack)
	, m_cam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
	, m_fpsText(&m_font, L"")
	, m_debugCamText(&m_font, L"")
{
	// Get the Application instance
	m_app = Application::getInstance();
	m_info = GameInfo::getInstance();
	m_info->isInMenu = true;
	// Set up camera with controllers
	m_playerCamController = std::make_unique<PlayerCameraController>(&m_cam);

	m_scene = std::make_unique<Scene>(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)));

	/*Clear Scores*/	
	m_info->resetScore();
	m_info->getPlayers().clear();
	m_info->gameSettings.teams.clear();
	m_info->convertGameSettings();

	/*VIB REMOVAL*/
	auto& gamePad = m_app->getInput().getGamePad();
	for (int u = 0; u < 4; u++)
		gamePad.SetVibration(u, 0, 0);

	// Set up the scene
	//m_scene->addSkybox(L"skybox_space_512.dds");
	// Add a directional light
	Vector3 color(0.9f, 0.9f, 0.9f);
	Vector3 direction(0.4f, -0.6f, 1.0f);
	direction.Normalize();
	m_scene->setUpDirectionalLight(Lights::DirectionalLight(color, direction));

	// Set up HUD texts
	m_debugCamText.setPosition(Vector2(0.f, 20.f));
	// Add texts to the scene
	m_scene->addText(&m_fpsText);
#ifdef _DEBUG
	m_scene->addText(&m_debugCamText);
#endif

	auto& resMan = m_app->getResourceManager();

	for(size_t i = 0; i < m_info->botHeadNames.size();i++) 
		m_playerHeadModels.push_back(resMan.getFBXModel("fisk/"+m_info->botHeadNames[i] + "_head").getModel());

	for (size_t i = 0; i < m_info->botBodyNames.size(); i++)
		m_playerBodyModels.push_back(resMan.getFBXModel("fisk/" + m_info->botBodyNames[i] + "_body").getModel());
	for (size_t i = 0; i < m_info->botLegNames.size(); i++)
		m_playerLegModels.push_back(resMan.getFBXModel("fisk/" + m_info->botLegNames[i] + "_legs").getModel());

	for (size_t i = 0; i < m_info->botArmNames.size(); i++) {
		m_playerArmLModels.push_back(resMan.getFBXModel("fisk/" + m_info->botArmNames[i] + "_armL").getModel());
		m_playerArmRModels.push_back(resMan.getFBXModel("fisk/" + m_info->botArmNames[i] + "_armR").getModel());
	}

	m_block = resMan.getFBXModel("block").getModel();
	
	m_targets.push_back(new MenuHandler());
	m_targets[0]->setPosition(Vector3(0, 0, -7));

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
	//m_scene->addObject(background);

	//MAINMENU
	initMain();
	updateCamera();
	//STARTMENU
	m_gamemodeMenu = nullptr;
	initGamemode();
	initCharacterModels();
	for (size_t i = 0; i < 4; i++) {
		initCharacter(i, false);
	}
	
	m_mapMenu = nullptr;
	m_profileCreator = nullptr;
	m_profileViewer = nullptr;
	//CUSTOMISABILITY
	initProfile();
	initProfileCreator();
	initProfileViewer();

	initOptions();
	initGraphics();
	initSound();

	m_playerCamController->setUseExtraZ(false);
	m_playerCamController->setTargets(m_mainMenu);
	m_playerCamController->setOffset(Vector3(0,0,0));
	m_playerCamController->setMoving(false);
	m_playerCamController->setPosition(Vector3(0,0,0));
	m_playerCamController->setFollowSpeed(5);

	// Sound
	m_app->getResourceManager().getSoundManager()->playAmbientSound(SoundManager::Ambient::Theme, true, 0.1f);
	
	m_app->getGameSettings().reset();
}


MenuState::~MenuState()
{
	Memory::safeDelete(m_mainMenu);
	Memory::safeDelete(m_gamemodeMenu);
	for (size_t i = 0; i < m_characterMenu.size(); i++)
		Memory::safeDelete(m_characterMenu[i]);
	Memory::safeDelete(m_mapMenu);
	Memory::safeDelete(m_profileMenu);
	Memory::safeDelete(m_profileCreator);
	Memory::safeDelete(m_profileViewer);
	Memory::safeDelete(m_profileViewerStats);
	Memory::safeDelete(m_profileViewerLines);
	Memory::safeDelete(m_optionsMenu);
	Memory::safeDelete(m_graphicsMenu);
	Memory::safeDelete(m_soundMenu);
	for (size_t i = 0; i < m_targets.size(); i++) {
		Memory::safeDelete(m_targets[i]);
	}
	for (size_t i = 0; i < m_playerz.size(); i++) {
		Memory::safeDelete(m_playerz[i]);
	}
	for (size_t i = 0; i < m_playerMenuModelz.size(); i++) {
		m_playerMenuModelz[i].clear();

	}
	m_graphicsModel.clear();

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
						updateCamera();
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
								m_info->gameSettings.playerLife = m_gamemodeMenu->getOptionAt(PLAYERLIFE);
								m_info->gameSettings.destructibleBlocks = m_gamemodeMenu->getOptionAt(6);
								m_info->getPlayers().clear();
								m_info->gameSettings.teams.clear();
								

								if (m_info->gameSettings.gameMode == DEATHMATCH) {
									/*for (size_t u = 0; u < 4; u++)
										m_info->gameSettings.teams.push_back({ u, 0});*/

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
								if (m_gamemodeMenu->getActive() == 0)
									updateGameMode();

							}
							if (left) {
								m_gamemodeMenu->left();
								if (m_gamemodeMenu->getActive() == 0)
									updateGameMode();
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
									initCharacter(spot, true);
									m_characterMenu[spot]->activate();

									temp->player.port = i;
									if (m_info->gameSettings.gameMode == DEATHMATCH) {
										temp->player.team = spot;

										m_characterMenu[spot]->next();
										bool cok = false;
										while (!cok) {
											temp->player.color = m_characterMenu[spot]->getOptionAt(1);

											for (size_t u = 0; u < m_playerz.size(); u++) {
												if (m_playerz[u]) {

													if (temp->player.color == m_playerz[u]->player.color && spot != u) {
														m_characterMenu[spot]->right();
														break;
													}
												}
												if (u == m_playerz.size() - 1) {
													cok = true;
													break;
												}
											}
										}
										m_characterMenu[spot]->back();


									}
									else {
										temp->player.team = (size_t)(spot * 0.5f);
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
									m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(temp->player.color, temp->player.hue));
									
									m_playerz[spot] = temp;
									bool ok = false;
									while (!ok) {
										m_playerz[spot]->player.currentProfile = &m_info->getProfiles()[m_characterMenu[spot]->getOptionAt(0)];

										for (size_t u = 0; u < m_playerz.size(); u++) {
											if (m_playerz[u]) {

												if (m_playerz[spot]->player.currentProfile == m_playerz[u]->player.currentProfile && spot != u) {
													m_characterMenu[spot]->right();
													break;
												}
											}
											if (u == m_playerz.size() - 1) {
												ok = true;
												break;
											}
										}
									}

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
									//player not online, go back to game settings
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
										if (option == 0) {
											bool ok = false;
											while (!ok) {
												m_playerz[spot]->player.currentProfile = &m_info->getProfiles()[m_characterMenu[spot]->getOptionAt(option)];

												for (size_t u = 0; u < m_playerz.size(); u++) {
													if (m_playerz[u]) {

														if (m_playerz[spot]->player.currentProfile == m_playerz[u]->player.currentProfile && spot != u) {
															right ? m_characterMenu[spot]->right() : m_characterMenu[spot]->left();
															break;
														}
													}
													if (u == m_playerz.size() - 1) {
														ok = true;
														break;
													}
												}
											}




											if (m_playerz[spot]->player.currentProfile->preOrdered()== 0) {
												int opt = m_info->gameSettings.gameMode == DEATHMATCH ? 3:4;
												
												if (m_characterMenu[spot]->getOptionAt(opt) == 2) {
													m_characterMenu[spot]->setOptionAt(opt, 0);
													m_playerz[spot]->player.headModel = m_characterMenu[spot]->getOptionAt(opt);
													m_playerMenuModelz[spot].head->setModel(m_playerHeadModels[m_playerz[spot]->player.headModel]);
												}

											}

										}
										if (m_info->gameSettings.gameMode == DEATHMATCH) {




											if (option == 1) {
												bool ok = false;
												while (!ok) {
													m_playerz[spot]->player.color = m_characterMenu[spot]->getOptionAt(option);

													for (size_t u = 0; u < m_playerz.size(); u++) {
														if (m_playerz[u]) {
															if (m_playerz[spot]->player.color == m_playerz[u]->player.color && spot != u) {
																right ? m_characterMenu[spot]->right() : m_characterMenu[spot]->left();
																break;
															}

														}
														if (u == m_playerz.size() - 1) {
															ok = true;
															break;
														}
													}
												}





												m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(m_playerz[spot]->player.color, m_playerz[spot]->player.hue));
											}
											if (option == 2) {
											
												m_playerz[spot]->player.hue = m_characterMenu[spot]->getOptionAt(option);
												m_playerMenuModelz[spot].setLight(m_info->getDefaultColor(m_playerz[spot]->player.color, m_playerz[spot]->player.hue));
									
											}

											if (option == 3) {
												if (m_characterMenu[spot]->getOptionAt(option) == 2 && m_playerz[spot]->player.currentProfile->preOrdered() == 0) {
													if (right)
														m_characterMenu[spot]->right();
													if (left)
														m_characterMenu[spot]->left();
												}
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
												size_t color = 0;

												bool ok = false;
												while (!ok) {
													m_info->gameSettings.teams[team].color = m_characterMenu[spot]->getOptionAt(option);

													for (size_t u = 0; u < m_info->gameSettings.teams.size(); u++) {
														if (m_info->gameSettings.teams[team].color == m_info->gameSettings.teams[u].color && team != u) {
															right ? m_characterMenu[spot]->right() : m_characterMenu[spot]->left();
															break;
														}
														if (u == m_info->gameSettings.teams.size() - 1) {
															ok = true;
															break;
														}
													}
												}





												color = m_info->gameSettings.teams[team].color;





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
												if (m_characterMenu[spot]->getOptionAt(option) == 2 && m_playerz[spot]->player.currentProfile->preOrdered() == 0) {
													if (right)
														m_characterMenu[spot]->right();
													if (left)
														m_characterMenu[spot]->left();
												}
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
								m_info->gameSettings.map = m_mapMenu->getOptionAt(0);
								

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
					switch (m_activeSubMenu) {
						case PMAIN: {
							if (a) {
								switch (m_profileMenu->getActive()) {
								case 0:
									setProfileMenu(false);
									setProfileCreator(true);
									break;
								case 1:
									setProfileMenu(false);
									initProfileViewer();
									setProfileViewer(true);
									break;
								default:
									break;
								}
								updateCamera();

							}
							if (b) {
								setProfileMenu(false);
								setMainSelect(true);
								updateCamera();
							}

							if (up) {
								m_profileMenu->back();
							}
							if (down) {
								m_profileMenu->next();
							}
						}break;
						case PCREATE: {
							static std::string name = "";
							if (a) {
								if (m_profileCreator->getActive() == 0) {
									if (m_profileCreator->getEditing()) {
										name = m_profileCreator->getEditorText();
										m_profileCreator->editing(false);
									}
									else {
										m_profileCreator->editing(true);
									}

								}
								if (m_profileCreator->getActive() == 2) {
									if(name.size() > 0) {
										size_t last = name.find_last_not_of(' ');
										std::string temp = name.substr(0, last+1);
										if (temp.size() > 0)
											m_info->addProfile(temp, m_profileCreator->getOptionAt(1));

										setProfileCreator(false);
										setProfileMenu(true);
										updateCamera();
									}
								}

							}
							if (b) {
								if (m_profileCreator->getEditing()) {
									m_profileCreator->editing(false);
								}
								else {
									setProfileCreator(false);
									setProfileMenu(true);
									name = "";
								}
								updateCamera();
								
							}
							if (up) {
								if (m_profileCreator->getActive() == 0 && m_profileCreator->getEditing()) {
									m_profileCreator->up();
								}
								else {
									m_profileCreator->back();
								}

							}
							if (down) {
								if (m_profileCreator->getActive() == 0 && m_profileCreator->getEditing()) {
									m_profileCreator->down();
								}
								else {
									m_profileCreator->next();
								}
								
							}
							if (right) {
								m_profileCreator->right();
								
							}
							if (left) {
								m_profileCreator->left();
								
								
							}

						}break;
						case PVIEW: {
							if (a) {

							}
							if (b) {
								setProfileViewer(false);
								setProfileMenu(true);
								updateCamera();
							}
							if (up) {

							}
							if (down) {

							}
							if (right) {
								m_profileViewer->right();
								updateProfileViewer();
							}
							if (left) {
								m_profileViewer->left();
								updateProfileViewer();
							}

						}break;
						default:
							break;
					}
				}break;

				case OPTIONSMENU: {
					switch (m_activeSubMenu) {
						case MAIN: {
							if (a) {
								switch (m_optionsMenu->getActive()) {
								case 0:
									setOptionsMenu(false);
									setGraphicsMenu(true);
									break;
								case 1:
									setOptionsMenu(false);
									setSoundMenu(true);
									break;
								}
							}
							if (b) {
								setOptionsMenu(false);
								setMainSelect(true);
							}
							if (up) {
								m_optionsMenu->back();
							}
							if (down) {
								m_optionsMenu->next();
							}
							updateCamera();
						} break;
						case GRAPHICS: {
							if (a) {
								setGraphicsMenu(false);
								setOptionsMenu(true);
							}
							if (b) {
								setGraphicsMenu(false);
								setOptionsMenu(true);
							}
							if (up) {
								m_graphicsMenu->back();
							}
							if (down) {
								m_graphicsMenu->next();
							}
							if (right) {
								m_graphicsMenu->right();
								updateGraphics();
							}
							if (left) {
								m_graphicsMenu->left();
								updateGraphics();
							}
							updateCamera();
						} break;
						case SOUND: {
							if (a) {
								setSoundMenu(false);
								setOptionsMenu(true);
							}
							if (b) {
								setSoundMenu(false);
								setOptionsMenu(true);
							}
							if (up) {
								m_soundMenu->back();
							}
							if (down) {
								m_soundMenu->next();
							}
							if (right) {
								m_soundMenu->right();
								updateSound();
								if (m_soundMenu->getActive() == 2) {
									float pitch = Utils::rnd() * 0.3f + 0.8f;
									Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Explosion, 3.0f, pitch);
								}



							}
							if (left) {
								m_soundMenu->left();
								updateSound();
								if (m_soundMenu->getActive() == 2) {
									float pitch = Utils::rnd() * 0.3f + 0.8f;
									Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Explosion, 3.0f, pitch);
								}
							}
							updateCamera();
						} break;
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
	m_scene->resize(width, height);

	return true;
}

// Updates the state
bool MenuState::update(float dt) {

	// Update HUD texts
	if (m_info->graphicsSettings.fpsCounter == 0)
		m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));
	else
		m_fpsText.setText(L"");

	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

	if(m_mainMenu)
		m_mainMenu->update(dt);
	if(m_gamemodeMenu)
		m_gamemodeMenu->update(dt);
	if(m_optionsMenu)
		m_optionsMenu->update(dt);
	if(m_profileMenu)
		m_profileMenu->update(dt);
	for (size_t i = 0; i < m_characterMenu.size(); i++)
		if(m_characterMenu[i])
			m_characterMenu[i]->update(dt);
	if(m_mapMenu)
		m_mapMenu->update(dt);
	if(m_profileMenu)
		m_profileMenu->update(dt);
	if(m_profileCreator)
		m_profileCreator->update(dt);
	if(m_profileViewer)
		m_profileViewer->update(dt);
	if(m_profileViewerStats)
		m_profileViewerStats->update(dt);
	if (m_profileViewerLines)
		m_profileViewerLines->update(dt);
	if(m_optionsMenu)
		m_optionsMenu->update(dt);
	if(m_graphicsMenu)
		m_graphicsMenu->update(dt);
	if(m_soundMenu)
		m_soundMenu->update(dt);





	m_playerCamController->update(dt);










	return true;
}
// Renders the state
bool MenuState::render(float dt) {
	// Clear back buffer
	m_app->getDXManager()->clear(DirectX::Colors::Black);

	// Draw the scene
	m_scene->draw(dt, m_cam);

	// Draw HUD
	m_scene->drawHUD();

	return true;
}


// CREATE ALL THE MENUS


void MenuState::initMain() {
	m_mainMenu = new MenuHandler();
	m_scene->addObject(m_mainMenu);

	m_mainMenu->addMenuBox("start");
	m_mainMenu->addMenuBox("profiles");
	m_mainMenu->addMenuBox("options");
	m_mainMenu->addMenuBox("exit");
	m_mainMenu->setPosition(Vector3(0, -0.5, 7));
	m_mainMenu->setFacingDirection(Vector3(0, 0, -1));
	m_mainMenu->setSize(1.3f);
	m_mainMenu->setOffColor(m_offColor);
	//m_mainMenu->setOnColor(m_onColor);
	m_mainMenu->activate();
	Logger::log("main menu loaded");
}

void MenuState::initGamemode() {

	if (!m_gamemodeMenu) {
		m_gamemodeMenu = new MenuHandler();
		m_scene->addObject(m_gamemodeMenu);

	}

	m_gamemodeMenu->addMenuSelector("gamemode");
	for (size_t i = 0; i < m_info->gameModes.size(); i++) {
		m_gamemodeMenu->addMenuSelectorItem(m_info->gameModes[i].name);

	}
	//m_gamemodeMenu->setStaticSelection(true, 0);
	m_gamemodeMenu->addMenuSelector("time limit");
	for (size_t i = 0; i < m_info->timeLimit.size(); i++) {
		m_gamemodeMenu->addMenuSelectorItem(m_info->timeLimit[i].name);

	}
	m_gamemodeMenu->setStaticSelection(true, 0);
	m_gamemodeMenu->addMenuSelector("score limit");
	for (size_t i = 0; i < m_info->scoreLimit.size(); i++) {
		m_gamemodeMenu->addMenuSelectorItem(m_info->scoreLimit[i].name);

	}
	m_gamemodeMenu->setStaticSelection(true, 0);
	m_gamemodeMenu->addMenuSelector("respawn time");
	for (size_t i = 0; i < m_info->respawnTime.size(); i++) {
		m_gamemodeMenu->addMenuSelectorItem(m_info->respawnTime[i].name);

	}
	m_gamemodeMenu->setStaticSelection(true, 0);
	m_gamemodeMenu->addMenuSelector("gravity");
	for (size_t i = 0; i < m_info->gravity.size(); i++) {
		m_gamemodeMenu->addMenuSelectorItem(m_info->gravity[i].name);

	}
	m_gamemodeMenu->setStaticSelection(true, 0);
	m_gamemodeMenu->addMenuSelector("player Health");
	for (size_t i = 0; i < m_info->playerHealth.size(); i++) {
		m_gamemodeMenu->addMenuSelectorItem(m_info->playerHealth[i].name);

	}
	m_gamemodeMenu->setStaticSelection(true, 0);

	m_gamemodeMenu->addMenuSelector("Destructible map");
	for (size_t i = 0; i < m_info->destructibleBlocks.size(); i++) {
		m_gamemodeMenu->addMenuSelectorItem(m_info->destructibleBlocks[i].name);
	}
	m_gamemodeMenu->setStaticSelection(true, 0);

	m_gamemodeMenu->setStep(0.0f);
	m_gamemodeMenu->setOnColor(m_onColor);
	
	m_gamemodeMenu->setPosition(Vector3(5, -0.5, 0));
	m_gamemodeMenu->setFacingDirection(Vector3(-1, 0, 0));




	Logger::log("gamesettings menu loaded");
}

void MenuState::updateGameMode() {
	if (m_gamemodeMenu) {
		int options[] = { 0,0,0,0,0,0 ,0};

		for (size_t i = 0; i < 6; i++) {
			options[i] = m_gamemodeMenu->getOptionAt(i);

		}
		m_gamemodeMenu->reset();


		m_gamemodeMenu->addMenuSelector("gamemode");
		for (size_t i = 0; i < m_info->gameModes.size(); i++) {
			m_gamemodeMenu->addMenuSelectorItem(m_info->gameModes[i].name);

		}
		m_gamemodeMenu->setOptionAt(0, options[0]);

		//m_gamemodeMenu->setStaticSelection(true, 0);
		m_gamemodeMenu->addMenuSelector("time limit");
		for (size_t i = 0; i < m_info->timeLimit.size(); i++) {
			m_gamemodeMenu->addMenuSelectorItem(m_info->timeLimit[i].name);

		}
		m_gamemodeMenu->setStaticSelection(true, 0);
		m_gamemodeMenu->setOptionAt(1, options[1]);
		m_gamemodeMenu->addMenuSelector("score limit");
		for (size_t i = 0; i < m_info->scoreLimit.size(); i++) {
			if(options[0] == 0)
				m_gamemodeMenu->addMenuSelectorItem(m_info->scoreLimit[i].name);
			if (options[0] == 1)
				m_gamemodeMenu->addMenuSelectorItem(m_info->scoreLimitDM[i].name);
			if (options[0] == 2)
				m_gamemodeMenu->addMenuSelectorItem(m_info->scoreLimitTDM[i].name);


		}
		m_gamemodeMenu->setStaticSelection(true, 0);
		m_gamemodeMenu->setOptionAt(2, options[2]);
		m_gamemodeMenu->addMenuSelector("respawn time");
		for (size_t i = 0; i < m_info->respawnTime.size(); i++) {
			m_gamemodeMenu->addMenuSelectorItem(m_info->respawnTime[i].name);

		}
		m_gamemodeMenu->setStaticSelection(true, 0);
		m_gamemodeMenu->setOptionAt(3, options[3]);
		m_gamemodeMenu->addMenuSelector("gravity");
		for (size_t i = 0; i < m_info->gravity.size(); i++) {
			m_gamemodeMenu->addMenuSelectorItem(m_info->gravity[i].name);

		}
		m_gamemodeMenu->setStaticSelection(true, 0);
		m_gamemodeMenu->setOptionAt(4, options[4]);
		m_gamemodeMenu->addMenuSelector("player Health");
		for (size_t i = 0; i < m_info->playerHealth.size(); i++) {
			m_gamemodeMenu->addMenuSelectorItem(m_info->playerHealth[i].name);

		}
		m_gamemodeMenu->setStaticSelection(true, 0);
		m_gamemodeMenu->setOptionAt(5, options[5]);

		m_gamemodeMenu->addMenuSelector("Destructible map");
		for (size_t i = 0; i < m_info->destructibleBlocks.size(); i++) {
			m_gamemodeMenu->addMenuSelectorItem(m_info->destructibleBlocks[i].name);
		}
		m_gamemodeMenu->setStaticSelection(true, 0);
		m_gamemodeMenu->setOptionAt(6, options[6]);
	}

}

void MenuState::initCharacterModels() {
		Vector3 charMenuMid(0, -2, -7);

	for (size_t i = 0; i < 4; i++) {
		Vector3 charMid = Vector3(0, 0, -1.0f)*((float)i - 1.5f) * 3.5f + charMenuMid;
		Vector3 direction(0,0,1);
		PlayerMenuModel temp;
	
		temp.head = new MenuItem(m_playerHeadModels[0], charMid + Vector3(0, 0.8f, 0));
		temp.body = new MenuItem(m_playerBodyModels[0], charMid + Vector3(0, 0.3f, 0));
		temp.legs = new MenuItem(m_playerLegModels[0],  charMid - Vector3(0, 0.9f, 0));
		temp.armL = new MenuItem(m_playerArmLModels[0], charMid + Vector3(0, 0.2f, -0.1f));
		temp.armR = new MenuItem(m_playerArmRModels[0], charMid + Vector3(0, 0.2f, 0.1f));
	
		temp.head->getTransform().setRotations(Vector3(0, 0.0f, 0));
		temp.body->getTransform().setRotations(Vector3(0, 0.0f, 0));
		temp.legs->getTransform().setRotations(Vector3(0, 0.0f, 0));
		temp.armL->getTransform().setRotations(Vector3(0, 0.0f, 0));
		temp.armR->getTransform().setRotations(Vector3(0, 0.0f, 0));

		temp.head->getTransform().setScale(1.4f);
		temp.body->getTransform().setScale(1.4f);
		temp.legs->getTransform().setScale(1.4f);
		temp.armL->getTransform().setScale(1.4f);
		temp.armR->getTransform().setScale(1.4f);

		m_scene->addObject(temp.head);
		m_scene->addObject(temp.body);
		m_scene->addObject(temp.legs);
		m_scene->addObject(temp.armL);
		m_scene->addObject(temp.armR);
		temp.setLight(m_offColor);
		temp.reset();
		m_playerMenuModelz.push_back(temp);
	}
	Vector3 charMid(-5, 9, 3);
	m_graphicsModel.head = new MenuItem(m_playerHeadModels[0], charMid + Vector3(0, 0.8f, 0));
	m_graphicsModel.body = new MenuItem(m_playerBodyModels[0], charMid + Vector3(0, 0.3f, 0));
	m_graphicsModel.legs = new MenuItem(m_playerLegModels[0], charMid - Vector3(0, 0.2f, 0));
	m_graphicsModel.armL = new MenuItem(m_playerArmLModels[0], charMid + Vector3(0, 0.2f, 0.1f));
	m_graphicsModel.armR = new MenuItem(m_playerArmRModels[0], charMid + Vector3(0, 0.2f, -0.1f));

	m_graphicsModel.setLight(m_info->getDefaultColor(1, 0));

	m_graphicsModel.head->getTransform().setRotations(Vector3(0, 0.0f, 0));
	m_graphicsModel.body->getTransform().setRotations(Vector3(0, 0.0f, 0));
	m_graphicsModel.legs->getTransform().setRotations(Vector3(0, 0.0f, 0));
	m_graphicsModel.armL->getTransform().setRotations(Vector3(0, 0.0f, 0));
	m_graphicsModel.armR->getTransform().setRotations(Vector3(0, 0.0f, 0));

	m_graphicsModel.head->getTransform().setScale(1.4f);
	m_graphicsModel.body->getTransform().setScale(1.4f);
	m_graphicsModel.legs->getTransform().setScale(1.4f);
	m_graphicsModel.armL->getTransform().setScale(1.4f);
	m_graphicsModel.armR->getTransform().setScale(1.4f);

	//m_scene->addObject(m_graphicsModel.head);
	//m_scene->addObject(m_graphicsModel.body);
	//m_scene->addObject(m_graphicsModel.legs);
	//m_scene->addObject(m_graphicsModel.armL);
	//m_scene->addObject(m_graphicsModel.armR);

	

	Logger::log("player models loaded");
}

void MenuState::initCharacterModel(size_t spot) {
	Vector3 charMenuMid(0, -2, -7);
	if (m_info->gameSettings.gameMode == DEATHMATCH)
		charMenuMid = Vector3(0,-1.3f,-7);

	Vector3 charMid = Vector3(-1, 0, 0.0f)*((float)spot - 1.5f) * 3.5f + charMenuMid;
	m_playerMenuModelz[spot].head->setPosition( charMid + Vector3(0, 0.8f, 0));
	m_playerMenuModelz[spot].body->setPosition( charMid + Vector3(0, 0.3f, 0));
	m_playerMenuModelz[spot].legs->setPosition (charMid - Vector3(0, 0.2f, 0));
	m_playerMenuModelz[spot].armL->setPosition( charMid + Vector3(0, 0.3f, -0.0f));
	m_playerMenuModelz[spot].armR->setPosition( charMid + Vector3(0, 0.3f, 0.0f));

	m_playerMenuModelz[spot].head->setModel(m_playerHeadModels[0]);
	m_playerMenuModelz[spot].body->setModel(m_playerBodyModels[0]);
	m_playerMenuModelz[spot].legs->setModel(m_playerLegModels[0]);
	m_playerMenuModelz[spot].armL->setModel(m_playerArmLModels[0]);
	m_playerMenuModelz[spot].armR->setModel(m_playerArmRModels[0]);
	m_playerMenuModelz[spot].setLight(m_offColor);
	
}

void MenuState::initCharacter(size_t spot, bool online) {
	static Vector3 charMid(0, 1, -7);
	if (m_characterMenu.size() == 0) {
		for (size_t i = 0; i < 4; i++) {
			m_characterMenu.push_back(new MenuHandler());
			m_scene->addObject(m_characterMenu[i]);
			m_characterMenu[i]->addMenuSelector("Offline");

		}
	}
	
	

	
	MenuHandler* temp = m_characterMenu[spot];
	temp->reset();
	temp->setPosition(charMid + Vector3(-1.0f, 0.0f, 0.0f)*((float)spot - 1.5f) * 3.5f);
	temp->setSize(0.7f);
	temp->setFacingDirection(Vector3(0.0f, 0, 1.0f));
	temp->setStep(0.4f);

	if (online) {
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

	}
	else {
		//temp->addMenuSelector("press");
		//temp->addMenuSelector("jump");
		//temp->addMenuSelector("to");
		//temp->addMenuSelector("join");

		temp->addMenuSelector("player " + std::to_string(spot+1));
		temp->addMenuSelector("offline");

	}

	//temp->addMenuSelector("bot arms");
	//for (size_t i = 0; i < m_info->botArmNames.size(); i++) {
	//	temp->addMenuSelectorItem("i           i");
	//}
	//temp->setStaticSelection(true, 0);


}

void MenuState::removeCharacter(size_t spot) {
	m_characterMenu[spot]->reset();
	m_playerMenuModelz[spot].reset();
	initCharacter(spot, false);
	
}

void MenuState::initMap() {

	if (!m_mapMenu) {
		m_mapMenu = new MenuHandler();
		m_scene->addObject(m_mapMenu);

	}
	else
		m_mapMenu->reset();


	m_mapMenu->addMenuSelector("Map");

	for (size_t i = 0; i < m_info->maps[m_info->gameSettings.gameMode].size(); i++) {
		m_mapMenu->addMenuSelectorItem(m_info->maps[m_info->gameSettings.gameMode][i]);

	}


	m_mapMenu->setStaticSelection(true, 0);

	m_mapMenu->setPosition(Vector3(-5,0,0));
	m_mapMenu->setFacingDirection(Vector3(1,0,0));
	m_mapMenu->setStep(0.1f);
	

	Logger::log("maps loaded");
}

void MenuState::initProfile() {
	m_profileMenu = new MenuHandler();
	m_scene->addObject(m_profileMenu);

	m_profileMenu->addMenuBox("create profile");
	m_profileMenu->addMenuBox("view profiles");
	m_profileMenu->setPosition(Vector3(-5.0f, 0.0f, 0));
	m_profileMenu->setFacingDirection(Vector3(1, 0, 0));
	Logger::log("profile main menu loaded");
}

void MenuState::initProfileCreator() {
	if (!m_profileCreator) {
		m_profileCreator = new MenuHandler();
		m_scene->addObject(m_profileCreator);
	}
	else {
		m_profileCreator->reset();

	}

	m_profileCreator->addTextItem("name", "        ");
	Logger::log("textItem loaded");
	m_profileCreator->addMenuSelector("preorder");
	m_profileCreator->addMenuSelectorItem("no");
	m_profileCreator->addMenuSelectorItem("yus");
	m_profileCreator->setStaticSelection(true, 0);
	m_profileCreator->addMenuBox("done");

	m_profileCreator->setPosition(Vector3(0,0,-5));
	m_profileCreator->setFacingDirection(Vector3(0,0,1));
	Logger::log("profile creator menu loaded");
}

void MenuState::initProfileViewer() {
	Vector3 pos(0, 0, 5);
	if (!m_profileViewer) {
		m_profileViewer = new MenuHandler();
		m_profileViewerStats = new MenuHandler();
		m_profileViewerLines = new MenuHandler();
		m_scene->addObject(m_profileViewer);
		m_scene->addObject(m_profileViewerLines);
		m_scene->addObject(m_profileViewerStats);
	}
	else {
		m_profileViewer->reset();
		m_profileViewerLines->reset();
		m_profileViewerStats->reset();

	}

	auto& profiles = m_info->getProfiles();
	m_profileViewer->addMenuSelector("name");
	for (size_t i = 4; i < profiles.size(); i++) {
		m_profileViewer->addMenuSelectorItem(profiles[i].getName());
	}
	m_profileViewer->setStaticSelection(true, 0);

	m_profileViewerLines->addMenuBox("kills");
	m_profileViewerLines->addMenuBox("Deaths");
	m_profileViewerLines->addMenuBox("KD");
	m_profileViewerLines->addMenuBox("wins");
	m_profileViewerLines->addMenuBox("losses");
	m_profileViewerLines->addMenuBox("winratio");

	updateProfileViewer();
	

	m_profileViewerStats->setStep(0);
	m_profileViewerLines->setStep(0);
	m_profileViewerStats->setOnColor( Vector4(0.6f, 0.6f, 0.6f, 1));
	m_profileViewerLines->setOnColor( Vector4(0.6f, 0.6f, 0.6f, 1));
	m_profileViewerStats->setOffColor(Vector4(0.6f, 0.6f, 0.6f, 1));
	m_profileViewerLines->setOffColor(Vector4(0.6f, 0.6f, 0.6f, 1));




	m_profileViewer->setPosition(pos+Vector3(0,2,0));
	m_profileViewer->setFacingDirection(Vector3(0,0,-1));

	m_profileViewerLines->setPosition(pos+Vector3(-3,0,0));
	m_profileViewerLines->setFacingDirection(Vector3(0, 0, -1));
	m_profileViewerStats->setPosition(pos+Vector3(3,0,0));
	m_profileViewerStats->setFacingDirection(Vector3(0, 0, -1));
	m_profileViewerStats->setSize(0.5f);
	m_profileViewerLines->setSize(0.5f);


	Logger::log("profile viewer menu loaded");
}

void MenuState::updateProfileViewer() {


	auto& profiles = m_info->getProfiles();
	m_profileViewerStats->reset();
	m_profileViewerStats->addMenuBox(std::to_string(profiles[m_profileViewer->getOptionAt(0)+4].getStats().kills));
	m_profileViewerStats->addMenuBox(std::to_string(profiles[m_profileViewer->getOptionAt(0)+4].getStats().deaths));
	std::string kdString = std::to_string(profiles[m_profileViewer->getOptionAt(0)+4].getKD());
	m_profileViewerStats->addMenuBox(kdString.substr(0, kdString.find_first_of(".") + 3));
	m_profileViewerStats->addMenuBox(std::to_string(profiles[m_profileViewer->getOptionAt(0) + 4].getStats().wins));
	m_profileViewerStats->addMenuBox(std::to_string(profiles[m_profileViewer->getOptionAt(0) + 4].getStats().losses));
	std::string wrString = std::to_string(profiles[m_profileViewer->getOptionAt(0)+4].getWinRatio());
	m_profileViewerStats->addMenuBox(wrString.substr(0, wrString.find_first_of(".") + 3));


}



void MenuState::initOptions() {
	m_optionsMenu = new MenuHandler();
	m_scene->addObject(m_optionsMenu);

	m_optionsMenu->addMenuBox("graphics");
	m_optionsMenu->addMenuBox("sound");

	m_optionsMenu->setPosition(Vector3(-5, -0.5f, 0));
	m_optionsMenu->setFacingDirection(Vector3(1, 0, 0));


	Logger::log("options menu loaded");
}

void MenuState::initGraphics() {
	m_graphicsMenu = new MenuHandler();
	m_scene->addObject(m_graphicsMenu);
	size_t ite = 0;

	m_graphicsMenu->addMenuSelector("particles");
	for (size_t i = 0; i < m_info->particles.size(); i++) {
		m_graphicsMenu->addMenuSelectorItem(m_info->particles[i].name);
	}
	m_graphicsMenu->setStaticSelection(true, 0);
	m_graphicsMenu->setOptionAt(ite++, m_info->graphicsSettings.particles);

	m_graphicsMenu->addMenuSelector("bloom");
	for (size_t i = 0; i < m_info->bloom.size(); i++) {
		m_graphicsMenu->addMenuSelectorItem(m_info->bloom[i].name);
	}
	m_graphicsMenu->setStaticSelection(true, 0);
	m_graphicsMenu->setOptionAt(ite++, m_info->graphicsSettings.bloom);

	m_graphicsMenu->addMenuSelector("antiAliasing");
	for (size_t i = 0; i < m_info->antiAliasing.size(); i++) {
		m_graphicsMenu->addMenuSelectorItem(m_info->antiAliasing[i].name);

	}
	m_graphicsMenu->setStaticSelection(true, 0);
	m_graphicsMenu->setOptionAt(ite++, m_info->graphicsSettings.AA);

	m_graphicsMenu->addMenuSelector("background");
	for (size_t i = 0; i < m_info->background.size(); i++) {
		m_graphicsMenu->addMenuSelectorItem(m_info->background[i].name);

	}
	m_graphicsMenu->setStaticSelection(true, 0);
	m_graphicsMenu->setOptionAt(ite++, m_info->graphicsSettings.backGround);

	m_graphicsMenu->addMenuSelector("fpscounter");
	for (size_t i = 0; i < m_info->fpsCounter.size(); i++) {
		m_graphicsMenu->addMenuSelectorItem(m_info->fpsCounter[i].name);

	}
	m_graphicsMenu->setStaticSelection(true, 0);
	m_graphicsMenu->setOptionAt(ite++, m_info->graphicsSettings.fpsCounter);

	m_graphicsMenu->addMenuSelector("vsync");
	for (size_t i = 0; i < m_info->vSync.size(); i++) {
		m_graphicsMenu->addMenuSelectorItem(m_info->vSync[i].name);

	}
	m_graphicsMenu->setStaticSelection(true, 0);
	m_graphicsMenu->setOptionAt(ite++, m_info->graphicsSettings.vSync);

	m_graphicsMenu->addMenuSelector("depth of field");
	for (size_t i = 0; i < m_info->depthOfField.size(); i++) {
		m_graphicsMenu->addMenuSelectorItem(m_info->depthOfField[i].name);

	}
	m_graphicsMenu->setStaticSelection(true, 0);
	m_graphicsMenu->setOptionAt(ite++, m_info->graphicsSettings.depthOfField);

	m_graphicsMenu->setPosition(Vector3(0,-1,-5));
	m_graphicsMenu->setFacingDirection(Vector3(0, 0, 1));


	Logger::log("graphics menu loaded");
}

void MenuState::initSound() {
	m_soundMenu = new MenuHandler();
	m_scene->addObject(m_soundMenu);
	size_t ite = 0;

	m_soundMenu->addMenuSelector("master volume");
	for (size_t i = 0; i < m_info->masterVolume.size(); i++) {
		m_soundMenu->addMenuSelectorItem(m_info->masterVolume[i].name);
	}
	m_soundMenu->setStaticSelection(true, 0);
	m_soundMenu->setOptionAt(ite++, m_info->soundSettings.master);

	m_soundMenu->addMenuSelector("background volume");
	for (size_t i = 0; i < m_info->backgroundVolume.size(); i++) {
		m_soundMenu->addMenuSelectorItem(m_info->backgroundVolume[i].name);
	}
	m_soundMenu->setStaticSelection(true, 0);
	m_soundMenu->setOptionAt(ite++, m_info->soundSettings.background);

	m_soundMenu->addMenuSelector("effect volume");
	for (size_t i = 0; i < m_info->effectVolume.size(); i++) {
		m_soundMenu->addMenuSelectorItem(m_info->effectVolume[i].name);
	}
	m_soundMenu->setStaticSelection(true, 0);
	m_soundMenu->setOptionAt(ite++, m_info->soundSettings.effect);


	m_soundMenu->setPosition(Vector3(0,0,5));
	m_soundMenu->setFacingDirection(Vector3(0,0,-1));


	Logger::log("sound menu loaded");
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
			m_characterMenu[i]->activate();
		}
		
		m_activeMenu = STARTMENU;
		m_activeSubMenu = PLAYERSELECT;

		

	}
	else {
		for (size_t i = 0; i < m_characterMenu.size(); i++) {
			m_characterMenu[i]->deActivate();
		
		}
	}
}

void MenuState::setMapSelect(bool active) {
	if (active) {
		m_activeMenu = STARTMENU;
		m_activeSubMenu = MAPSELECT;
		m_mapMenu->activate();

	}
	else {
		m_mapMenu->deActivate();
		for (size_t i = 0; i < m_playerz.size(); i++) {
			m_playerz[i]->ready = false;
		}

	}
}

void MenuState::setProfileMenu(bool active) {
	if (active) {
		m_profileMenu->activate();
		m_activeMenu = PROFILEMENU;
		m_activeSubMenu = PMAIN;
	}
	else {
		m_profileMenu->deActivate();


	}
}

void MenuState::setProfileCreator(bool active) {
	if (active) {
		m_profileCreator->activate();
		m_activeMenu = PROFILEMENU;
		m_activeSubMenu = PCREATE;
	} 
	else {
		m_profileCreator->deActivate();
		m_activeMenu = PROFILEMENU;
		m_activeSubMenu = PMAIN;
	}
}

void MenuState::setProfileViewer(bool active) {
	if (active) {
		m_profileViewer->activate();
		m_profileViewerLines->activate();
		m_profileViewerStats->activate();
		m_activeMenu = PROFILEMENU;
		m_activeSubMenu = PVIEW;
	}
	else {
		m_profileViewer->deActivate();
		m_profileViewerLines->deActivate();
		m_profileViewerStats->deActivate();
		m_activeMenu = PROFILEMENU;
		m_activeSubMenu = PMAIN;
	}
}


void MenuState::setOptionsMenu(bool active) {
	if (active) {
		m_optionsMenu->activate();
		m_playerCamController->setTargets(m_optionsMenu->getTarget());
		m_activeMenu = OPTIONSMENU;
		m_activeSubMenu = MAIN;
		m_playerCamController->setPosition(Vector3(0, 0, 0));
	}
	else {
		m_optionsMenu->deActivate();

	}
}

void MenuState::setGraphicsMenu(bool active) {
	if (active) {
		m_graphicsMenu->activate();
		m_playerCamController->setTargets(m_graphicsMenu, m_graphicsMenu->getTarget(), m_graphicsMenu->getExtraTarget());
		m_activeMenu = OPTIONSMENU;
		m_activeSubMenu = GRAPHICS;

	}
	else {
		m_graphicsMenu->deActivate();
	}
}

void MenuState::setSoundMenu(bool active) {
	if (active) {
		m_soundMenu->activate();
		m_playerCamController->setTargets(m_soundMenu, m_soundMenu->getTarget());
		m_activeMenu = OPTIONSMENU;
		m_activeSubMenu = SOUND;

	}
	else {
		m_soundMenu->deActivate();
	}
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
			m_playerCamController->setTargets(m_targets[0]);
		}
		if (m_activeSubMenu == MAPSELECT) {

			m_playerCamController->setTargets(m_mapMenu, m_mapMenu->getTarget(), m_mapMenu->getExtraTarget());

		}
	}
	if (m_activeMenu == PROFILEMENU) {
		if (m_activeSubMenu == MAIN) {
			m_playerCamController->setTargets(m_profileMenu, m_profileMenu->getTarget());
		}
		if (m_activeSubMenu == PCREATE) {

			m_playerCamController->setTargets(m_profileCreator, m_profileCreator->getTarget());
		}
		if (m_activeSubMenu == PVIEW) {

			m_playerCamController->setTargets(m_profileViewer, m_profileViewerStats, m_profileViewerLines);
		}



	}
	if (m_activeMenu == OPTIONSMENU) {
		if (m_activeSubMenu == MAIN) {
			m_playerCamController->setTargets(m_optionsMenu, m_optionsMenu->getTarget());
		}
		if (m_activeSubMenu == GRAPHICS) {
			m_playerCamController->setTargets(m_graphicsMenu, m_graphicsMenu->getTarget(), m_graphicsMenu->getExtraTarget());

		}
		if (m_activeSubMenu == SOUND) {
			m_playerCamController->setTargets(m_soundMenu, m_soundMenu->getTarget(), m_soundMenu->getExtraTarget());
		}
	}
}

void MenuState::startGame() {
	Application::GameSettings& settings = Application::getInstance()->getGameSettings();
	for (size_t i = 0; i < m_playerz.size(); i++) {
		if (m_playerz[i]) {
			m_info->addPlayer(m_playerz[i]->player);
			if (m_info->gameSettings.gameMode == DEATHMATCH) {
				m_info->gameSettings.teams.push_back({ m_playerz[i]->player.color, m_playerz[i]->player.currentProfile->preOrdered() });
			}
		}
	}
	m_info->convertGameSettings();

	// Sound
	m_app->getResourceManager().getSoundManager()->stopAmbientSound(SoundManager::Ambient::Theme);
	requestStackPop();
	requestStackPush(States::Game);
}

void MenuState::updateGraphics() {
	m_info->graphicsSettings.particles = m_graphicsMenu->getOptionAt(0);
	m_info->graphicsSettings.bloom = m_graphicsMenu->getOptionAt(1);
	m_info->graphicsSettings.AA = m_graphicsMenu->getOptionAt(2);
	m_info->graphicsSettings.backGround= m_graphicsMenu->getOptionAt(3);
	m_info->graphicsSettings.fpsCounter = m_graphicsMenu->getOptionAt(4);
	m_info->graphicsSettings.vSync = m_graphicsMenu->getOptionAt(5);
	m_info->graphicsSettings.depthOfField = m_graphicsMenu->getOptionAt(6);

	m_info->convertGraphics();
}

void MenuState::updateSound() {

	m_info->soundSettings.master = m_soundMenu->getOptionAt(0);
	m_info->soundSettings.background = m_soundMenu->getOptionAt(1);
	m_info->soundSettings.effect = m_soundMenu->getOptionAt(2);

	m_info->convertSound();

	m_app->getResourceManager().getSoundManager()->setMasterVolume(m_info->convertedSound.masterVolume);
	m_app->getResourceManager().getSoundManager()->setAmbientVolume(m_info->convertedSound.backGroundSoundVolume);
	m_app->getResourceManager().getSoundManager()->setEffectsVolume(m_info->convertedSound.effectSoundVolume);
}

