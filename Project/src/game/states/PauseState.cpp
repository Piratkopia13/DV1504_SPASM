#include "PauseState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PauseState::PauseState(StateStack& stack)
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

	// Set up the scene
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

	m_menuOnModel = resMan.getFBXModel("block").getModel();
	m_menuOffModel = resMan.getFBXModel("block2").getModel();

	float windowWidth = (float)m_app->getWindow()->getWindowWidth();
	float windowHeight = (float)m_app->getWindow()->getWindowHeight();
	float texPlaneHeight = windowHeight / 2.5f;
	Vector2 texPlaneHalfSize(texPlaneHeight / 2.f * (windowWidth / windowHeight), texPlaneHeight / 2.f);
	m_background = ModelFactory::PlaneModel::Create(texPlaneHalfSize);
	m_background->buildBufferForShader(&m_colorShader);
	m_background->getMaterial()->setColor(Vector4(0, 0, 0, 0.8f)); 
	m_background->getTransform().setTranslation(Vector3(0, 0, 4.f));
	m_background->getTransform().setRotations(Vector3(-1.62f, 0, 0));


	m_onColor = Vector4(1.f, 1.f, 1.f, 1.f);
	m_offColor = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

	this->m_selector = 0;
	this->startTimer = 0;
	

	m_pauseMenu = std::make_unique<MenuHandler>();
	m_scene.addObject(m_pauseMenu.get());

	m_pauseMenu->addMenuBox("resume");
	m_pauseMenu->addMenuBox("quit");
	m_pauseMenu->setPosition(Vector3(0, -0.5, 7));
	m_pauseMenu->setFacingDirection(Vector3(0, 0, -1));
	m_pauseMenu->setOffColor(m_offColor);
	m_pauseMenu->setOnColor(m_onColor);
	m_pauseMenu->activate();
	Logger::log("pause menu loaded");


	/*MenuItem* start = new MenuItem(m_menuOnModel, Vector3(1, 3, 3));
	MenuItem* something = new MenuItem(m_menuOffModel, Vector3(1, 2, 3));
	MenuItem* exit = new MenuItem(m_menuOffModel, Vector3(1, 1, 3));
	

	this->m_menuList.push_back(start);
	this->m_menuList.push_back(something);
	this->m_menuList.push_back(exit);


	for (size_t i = 0; i < this->m_menuList.size(); i++) {
		m_scene.addObject(m_menuList[i]);
	}*/


	m_playerCamController->setTargets(
		m_pauseMenu->getTarget(),
		nullptr,
		nullptr,
		nullptr
	);
	m_playerCamController->setMoving(false);
	/*m_playerCamController->setPosition(this->m_menuList[1]->getTransform().getTranslation());
	m_playerCamController->setTarget(this->m_menuList[1]->getTransform().getTranslation());*/

	m_playerCamController->setPosition(m_pauseMenu->getTarget()->getTransform().getTranslation());
	m_playerCamController->setTarget(m_pauseMenu->getTarget()->getTransform().getTranslation());
	m_playerCamController->setOffset(Vector3(0,0,4));
}


PauseState::~PauseState()
{
	for (size_t i = 0; i < this->m_menuList.size(); i++) {
		delete this->m_menuList[i];
	}
}

// Process input for the state
bool PauseState::processInput(float dt) {

	if (this->startTimer > 0)
		return false;

	m_app->getInput().processAllGamepads([&](auto& state, auto& tracker) {
		// ON BUTTON CLICK
		/*if (tracker.a == GamePad::ButtonStateTracker::PRESSED) {
			switch (this->m_selector) {
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

		}*/
		/* UPDATED */
		if (tracker.a == GamePad::ButtonStateTracker::PRESSED) {
			//switch (this->m_selector) {
			switch (m_pauseMenu->getActive()) {
			case 0:

				requestStackPop();
				//this->beginStartTimer();
				break;
			
			case 1:

				requestStackClear();
				requestStackPush(States::MainMenu);

			default:
				break;
			}

		}
		/* UPDATED */

		if (tracker.b == GamePad::ButtonStateTracker::PRESSED) {


		}

		if (tracker.dpadDown == GamePad::ButtonStateTracker::PRESSED) {
			this->changeMenu(1);
		}
		if (tracker.dpadUp == GamePad::ButtonStateTracker::PRESSED) {
			this->changeMenu(-1);
		}


		if (tracker.menu == 3) {
		}
		if (tracker.back == 3) {

			// show scoreboard ? 
		}
	});

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

	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));

	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

	m_posZTEMP -= dt * 0.1f;
	//m_background->getTransform().setTranslation(Vector3(0, 0, m_posZTEMP));

	if (this->startTimer > 0) {


		if (startTimer > 2) {

		}


	}




	m_playerCamController->update(dt);

	return false;
}

// Renders the state
bool PauseState::render(float dt) {

	// Draw the scene
	m_background->draw();
	m_scene.draw(dt, m_cam);

	//m_pauseMenu->draw();

	m_colorShader.updateCamera(m_cam);

	// Draw HUD
	m_scene.drawHUD();

	return true;
}

void PauseState::beginStartTimer()
{
	this->startTimer += 0.01f;
}

void PauseState::changeMenu(int change) {
	/*this->m_menuList[this->m_selector]->setModel(m_menuOffModel);
	this->m_selector += change;
	if (this->m_selector < 0)
		this->m_selector = 2;
	if (this->m_selector > 2)
		this->m_selector = 0;

	this->m_menuList[this->m_selector]->setModel(m_menuOnModel);
	m_playerCamController->setTargets(
		this->m_menuList[this->m_selector],
		nullptr,
		nullptr,
		nullptr
	);*/

	switch (change) {
	case 1:
		m_pauseMenu->next();
		break;
	case -1:
		m_pauseMenu->back();
		break;
	default:
		break;
	}

	/*m_playerCamController->setTargets(
		m_pauseMenu->getTarget(),
		nullptr,
		nullptr,
		nullptr
	);*/

}
