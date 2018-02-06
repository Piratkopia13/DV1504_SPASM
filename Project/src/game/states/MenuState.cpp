#include "MenuState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

MenuState::MenuState(StateStack& stack) 
	: State(stack)
	, m_cam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
	, m_hudCam(1280.f, 720.f, -1.f, 1.f)
	, m_fpsText(&m_font, L"")
	, m_debugText(&m_font, L"")
	, m_debugCamText(&m_font, L"")
	, m_debugParticleText(&m_font, L"")
	, m_playerCamController(&m_cam)
	, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
{
	m_app = Application::getInstance();

	
	//Update the hud shader
	m_hudShader.updateCamera(m_hudCam);

	m_cam.setPosition(Vector3(0.f, 5.f, -7.0f));

	m_timer.startTimer();

	// Add skybox to the scene
	m_scene.addSkybox(L"skybox_space_512.dds");
	auto& l = m_scene.getLights();
	auto dl = l.getDL();
	dl.color = Vector3(0.9f, 0.9f, 0.9f);
	dl.direction = Vector3(0.4f, -0.6f, 1.0f);
	//dl.direction = Vector3(0.f, -1.f, 0.f);
	dl.direction.Normalize();
	l.setDirectionalLight(dl);

	m_scene.setShadowLight();

	m_matShader.updateLights(m_scene.getLights());


	
	m_debugCamText.setPosition(Vector2(0.f, 20.f));
	m_debugText.setPosition(Vector2(0.f, 40.f));

	// Add texts to the scene
	m_scene.addText(&m_fpsText);
	m_scene.addText(&m_debugText);
	m_scene.addText(&m_debugCamText);
	m_scene.addText(&m_debugParticleText);

	m_menuBlock = std::make_unique<FbxModel>("block.fbx");
	m_menuBlock->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	m_menuStart = std::make_unique<FbxModel>("startButton.fbx");
	m_menuStart->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	m_menuOptions = std::make_unique<FbxModel>("optionsButton.fbx");
	m_menuOptions->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	m_menuExit = std::make_unique<FbxModel>("exitButton.fbx");
	m_menuExit->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	m_player = std::make_unique<FbxModel>("fisk.fbx");
	m_player->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	m_backGround = std::make_unique<FbxModel>("menu_screen.fbx");
	m_backGround->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());
	

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

	m_playerCamController.setUseExtraZ(false);
	m_playerCamController.setTargets(this->menuList[0]);
	m_playerCamController.setOffset(Vector3(0,0,0));
	m_playerCamController.setMoving(false);
	m_playerCamController.setPosition(Vector3(0,0,0));
	m_playerCamController.setFollowSpeed(8);

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
								this->m_playerCamController.setTargets(this->playerMenu[0], this->playerMenu[1], 
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
											this->m_playerCamController.setTargets(this->menuList[0]);

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
													this->m_playerCamController.setTargets(this->mapMenu[0]);				
													break;
												}
											}
											if (b) {
											this->players[i] = OFFLINE;
											this->setColor(i, this->offColor);

											}

											if (right) {
												this->setColor(i, this->getRandomColor());
											}
											if (left) {
												this->setColor(i, this->getRandomColor());
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
								this->m_playerCamController.setTargets(this->playerMenu[0], this->playerMenu[1],
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


	static float counter = 0.f;
	counter += dt;

	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));

	auto& cPos = m_cam.getPosition();

	std::wstring flying(L"Flying (shift for boost, rmouse to toggle cursor)");
	std::wstring Walking(L"Walking (rmouse to toggle cursor)");

	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L"  NumNodes: " + std::to_wstring(Quadtree::numNodes));

	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

		m_playerCamController.update(dt);

	return true;
}
// Renders the state
bool MenuState::render(float dt) {
	// Clear the buffer where the deferred light pass will render to
	m_app->getDXManager()->clear(DirectX::Colors::Black);
	// Clear back buffer

	// Draw the scene
	// before rendering the final output to the back buffer
	m_scene.draw(dt, m_cam, nullptr, nullptr);

	//m_app->getDXManager()->enableAlphaBlending();
	//m_colorShader.updateCamera(m_cam);
	

	Application::getInstance()->getDXManager()->getDeviceContext()->GSSetShader(nullptr, 0, 0);

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
		m_playerCamController.setTargets(
			this->menuList[m_selector],
			nullptr,
			nullptr,
			nullptr
		);
	}
	if (active == OPTIONSMENU) {
		this->optionsMenuList[m_selector]->setLightColor(this->onColor);
		m_playerCamController.setTargets(
			this->optionsMenuList[m_selector],
			nullptr,
			nullptr,
			nullptr
		);
	}
	if (active == STARTMENU && m_activeSubMenu == MAPSELECT) {

		this->mapMenu[m_selector]->setLightColor(this->onColor);
		m_playerCamController.setTargets(
			this->mapMenu[m_selector],
			nullptr,
			nullptr,
			nullptr
		);
	}
	
}
