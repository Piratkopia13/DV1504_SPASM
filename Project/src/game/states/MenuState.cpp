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
	m_scene.addSkybox(L"skybox2_512.dds");
	auto& l = m_scene.getLights();
	auto dl = l.getDL();
	dl.color = Vector3(0.9f, 0.9f, 0.9f);
	dl.direction = Vector3(0.4f, -0.6f, 1.0f);
	//dl.direction = Vector3(0.f, -1.f, 0.f);
	dl.direction.Normalize();
	l.setDirectionalLight(dl);

	m_scene.setShadowLight();

	Lights::PointLight pl;
	pl.setColor(Vector3(0.1f, 0.9f, 0.1f));
	pl.setPosition(Vector3(0.f, 4.f, -2.f));
	pl.setAttenuation(1.f, 1.f, 1.f);
	l.addPointLight(pl);


	m_matShader.updateLights(m_scene.getLights());


	
	m_debugCamText.setPosition(Vector2(0.f, 20.f));
	m_debugText.setPosition(Vector2(0.f, 40.f));

	// Add texts to the scene
	m_scene.addText(&m_fpsText);
	m_scene.addText(&m_debugText);
	m_scene.addText(&m_debugCamText);
	m_scene.addText(&m_debugParticleText);

	m_menuOn = std::make_unique<FbxModel>("block.fbx");
	m_menuOn->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	m_menuOff = std::make_unique<FbxModel>("block2.fbx");
	m_menuOff->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());

	//m_WeaponModel1 = std::make_unique<FbxModel>("weapon.fbx");
	//m_WeaponModel1->getModel()->buildBufferForShader(&m_scene.getDeferredRenderer().getGeometryShader());
	
	
	this->selector = 0;
	this->menu = -1;
	for(int i = 0; i < 4; i++)
		this->players[i] = 0;

	MenuItem* start = new MenuItem(m_menuOn->getModel(), Vector3(0, 1, 7));
	MenuItem* something = new MenuItem(m_menuOff->getModel(), Vector3(0, 0, 7));
	MenuItem* exit = new MenuItem(m_menuOff->getModel(), Vector3(0, -1, 7));

	MenuItem* player1 = new MenuItem(m_menuOff->getModel(), Vector3(7, 1, -2.25));
	MenuItem* player2 = new MenuItem(m_menuOff->getModel(), Vector3(7, 1, -0.75));
	MenuItem* player3 = new MenuItem(m_menuOff->getModel(), Vector3(7, 1, 0.75));
	MenuItem* player4 = new MenuItem(m_menuOff->getModel(), Vector3(7, 1, 2.25));

	//MenuItem* start = new MenuItem(m_menuOff->getModel(), Vector3(1, 3, 1));
	//MenuItem* something = new MenuItem(m_menuOff->getModel(), Vector3(1, 2, 1));
	//MenuItem* exit = new MenuItem(m_menuOff->getModel(), Vector3(1, 1, 1));

	


	this->menuList.push_back(start);
	this->menuList.push_back(something);
	this->menuList.push_back(exit);

	this->startMenuList1.push_back(player1);
	this->startMenuList1.push_back(player2);
	this->startMenuList1.push_back(player3);
	this->startMenuList1.push_back(player4);

	for (size_t i = 0; i < this->menuList.size(); i++) {
		m_scene.addObject(menuList[i]);
	}
	for (size_t i = 0; i < this->startMenuList1.size(); i++) {
		m_scene.addObject(startMenuList1[i]);
	}


	m_playerCamController.setTargets(
		this->menuList[0],
		nullptr,
		nullptr,
		nullptr
	);
	m_playerCamController.setOffset(Vector3(0,0,0));
	m_playerCamController.setMoving(false);
	m_playerCamController.setPosition(Vector3(0,0,0));
	m_playerCamController.setFollowSpeed(8);
}


MenuState::~MenuState()
{
	for (size_t i = 0; i < this->menuList.size(); i++) {
		delete this->menuList[i];
	}
	for (size_t i = 0; i < this->startMenuList1.size(); i++) {
		delete this->startMenuList1[i];
	}
	for (size_t i = 0; i < this->startMenuList2.size(); i++) {
		delete this->startMenuList2[i];
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
	//Keyboard::KeyboardStateTracker& keyTracker;



	for (int i = 0; i < 4; i++) {

		DirectX::GamePad::State& padState = m_app->getInput().gamepadState[i];
		GamePad::ButtonStateTracker& padTracker = gpTracker[i];
		if (padState.IsConnected()) {

			// ON BUTTON CLICK
			if (padTracker.a == GamePad::ButtonStateTracker::PRESSED) {
				switch (this->selector) {
				case START: {
					switch (this->menu) {
					case NOTHING: {
						this->menu = START;
						m_playerCamController.setTargets(
							this->startMenuList1[0],
							this->startMenuList1[1],
							this->startMenuList1[2],
							this->startMenuList1[3]
						);

					}break;


					case START: {
				
						switch (this->players[i]) {
						case 0: {
							this->players[i] = 1;
							this->startMenuList1[i]->setModel(m_menuOn->getModel());
						}break;
						case 1: {




							requestStackPop();
							requestStackPush(States::Game);
						}



						}




					}break;






					}
				}break;
				case OPTIONS: {
					
					



				}break;
				case EXIT: {
					PostQuitMessage(0);
				}break;
				default:
					break;
				}
				
			}
			if (padTracker.b == GamePad::ButtonStateTracker::PRESSED) {
				switch (this->menu) {
				case NOTHING: {
					
				}break;


				case START: {
					switch (this->players[i]) {
					case 0: {
						for (int u = 0; u < 4; u++) {
							if (this->players[u] == 1)
							{
								this->players[u] = 0;
								this->startMenuList1[u]->setModel(m_menuOff->getModel());

							}
							m_playerCamController.setTargets(
								this->menuList[0],
								nullptr,
								nullptr,
								nullptr
							);
							this->menu = NOTHING;
						}
					}break;
					case 1: {
						this->players[i] = 0;
						this->startMenuList1[i]->setModel(m_menuOff->getModel());
					}



					}




				}break;






				}

			}
			
			if (padTracker.dpadDown == GamePad::ButtonStateTracker::PRESSED) {
				switch (this->menu) {
				case NOTHING: {
					this->changeMenu(1);

				}break;
					
				}

			}
			if (padTracker.dpadUp == GamePad::ButtonStateTracker::PRESSED) {
				switch (this->menu) {
				case NOTHING: {
					this->changeMenu(-1);
				}break;
				default:
					break;
				}
			}


			if (padTracker.menu == 3) {
			}
			if (padTracker.back == 3) {

				// show scoreboard ? 
			}
		}



	}





	if (kbTracker.pressed.Q) {
		requestStackPop();
		requestStackPush(States::Game);
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
	m_app->getDXManager()->clear(DirectX::Colors::Teal);
	// Clear back buffer

	// Draw the scene
	// before rendering the final output to the back buffer
	m_scene.draw(dt, m_cam, nullptr, nullptr);

	//m_app->getDXManager()->enableAlphaBlending();
	m_colorShader.updateCamera(m_cam);
	

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

void MenuState::changeMenu(int change)
{
	this->menuList[this->selector]->setModel(m_menuOff->getModel());
	this->selector += change;
	if (this->selector < 0)
		this->selector = 2;
	if (this->selector > 2)
		this->selector = 0;

	this->menuList[this->selector]->setModel(m_menuOn->getModel());
	m_playerCamController.setTargets(
		this->menuList[this->selector],
		nullptr,
		nullptr,
		nullptr
	);
}
