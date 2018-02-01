#include "PauseState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PauseState::PauseState(StateStack& stack)
	: State(stack)
	, m_cam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
	, m_hudCam(1280.f, 720.f, -1.f, 1.f)
	, m_fpsText(&m_font, L"")
	, m_debugText(&m_font, L"")
	, m_debugCamText(&m_font, L"")
	, m_debugParticleText(&m_font, L"")
	, m_startText(&m_font, L"")
	, m_playerCamController(&m_cam)
	, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
{
	m_app = Application::getInstance();


	//Update the hud shader
	m_hudShader.updateCamera(m_hudCam);

	m_cam.setPosition(Vector3(0.f, 5.f, -7.0f));

	m_timer.startTimer();

	// Add skybox to the scene
	//m_scene.addSkybox(L"skybox2_512.dds");
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

	float windowWidth = (float)m_app->getWindow()->getWindowWidth();
	float windowHeight = (float)m_app->getWindow()->getWindowHeight();
	float texPlaneHeight = windowHeight / 2.5f;
	Vector2 texPlaneHalfSize(texPlaneHeight / 2.f * (windowWidth / windowHeight), texPlaneHeight / 2.f);
	m_background = ModelFactory::PlaneModel::Create(texPlaneHalfSize);
	m_background->buildBufferForShader(&m_colorShader);
	m_background->getMaterial()->setColor(Vector4(0, 0, 0, 0.8f));
	m_background->getTransform().setTranslation(Vector3(0, 0, 2));
	m_background->getTransform().setRotations(Vector3(-1.62f, 0, 0));

	this->selector = 0;
	this->startTimer = 0;
	
	

	MenuItem* start = new MenuItem(m_menuOn->getModel(), Vector3(1, 3, 3));
	MenuItem* something = new MenuItem(m_menuOff->getModel(), Vector3(1, 2, 3));
	MenuItem* exit = new MenuItem(m_menuOff->getModel(), Vector3(1, 1, 3));
	

	this->menuList.push_back(start);
	this->menuList.push_back(something);
	this->menuList.push_back(exit);


	for (size_t i = 0; i < this->menuList.size(); i++) {
		m_scene.addObject(menuList[i]);
	}


	m_playerCamController.setTargets(
		this->menuList[0],
		nullptr,
		nullptr,
		nullptr
	);
	m_playerCamController.setMoving(false);
	m_playerCamController.setPosition(this->menuList[1]->getTransform().getTranslation());
	m_playerCamController.setTarget(this->menuList[1]->getTransform().getTranslation());
	m_playerCamController.setOffset(Vector3(0,0,4));
}


PauseState::~PauseState()
{
	for (size_t i = 0; i < this->menuList.size(); i++) {
		delete this->menuList[i];
	}
}



// Process input for the state
bool PauseState::processInput(float dt) {

	static Keyboard::KeyboardStateTracker kbTracker;
	static GamePad::ButtonStateTracker gpTracker[4];
	for (int i = 0; i < 4; i++)
		gpTracker[i].Update(m_app->getInput().gamepadState[i]);
	kbTracker.Update(m_app->getInput().keyboardState);

	//DirectX::Keyboard::State& keyState;
	//Keyboard::KeyboardStateTracker& keyTracker;
	if (this->startTimer > 0)
		return false;


	for (int i = 0; i < 4; i++) {

		DirectX::GamePad::State& padState = m_app->getInput().gamepadState[i];
		GamePad::ButtonStateTracker& padTracker = gpTracker[i];
		if (padState.IsConnected()) {

			// ON BUTTON CLICK
			if (padTracker.a == GamePad::ButtonStateTracker::PRESSED) {
				switch (this->selector) {
				case 0:

					requestStackPop();
					//this->beginStartTimer();
					break;

				case 1:


					break;
				case 2:

					requestStackClear();
					requestStackPush(States::MainMenu);

				default:
					break;
				}

			}
			if (padTracker.b == GamePad::ButtonStateTracker::PRESSED) {


			}

			if (padTracker.dpadDown == GamePad::ButtonStateTracker::PRESSED) {
				this->changeMenu(1);
			}
			if (padTracker.dpadUp == GamePad::ButtonStateTracker::PRESSED) {
				this->changeMenu(-1);
			}


			if (padTracker.menu == 3) {
			}
			if (padTracker.back == 3) {

				// show scoreboard ? 
			}
		}



	}

	return false;
}

// Process window resizing for the state
bool PauseState::resize(int width, int height) {
	m_cam.resize(width, height);
	m_scene.resize(width, height);

	return true;
}


// Updates the state
bool PauseState::update(float dt) {


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


	if (this->startTimer > 0) {


		if (startTimer > 2) {

		}


	}




	m_playerCamController.update(dt);

	return false;
}
// Renders the state
bool PauseState::render(float dt) {
	// Clear the buffer where the deferred light pass will render to
	//m_app->getDXManager()->clear(DirectX::Colors::Teal);
	// Clear back buffer

	// Draw the scene
	// before rendering the final output to the back buffer
	
	m_background->draw();
	
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

void PauseState::beginStartTimer()
{
	this->startTimer += 0.01f;
}

void PauseState::changeMenu(int change)
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
