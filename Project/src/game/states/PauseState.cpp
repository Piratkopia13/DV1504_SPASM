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

	m_menuOn = std::make_unique<FbxModel>("block.fbx");
	m_menuOn->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());

	m_menuOff = std::make_unique<FbxModel>("block2.fbx");
	m_menuOff->getModel()->buildBufferForShader(&Application::getInstance()->getResourceManager().getShaderSet<DeferredGeometryShader>());

	float windowWidth = (float)m_app->getWindow()->getWindowWidth();
	float windowHeight = (float)m_app->getWindow()->getWindowHeight();
	float texPlaneHeight = windowHeight / 2.5f;
	Vector2 texPlaneHalfSize(texPlaneHeight / 2.f * (windowWidth / windowHeight), texPlaneHeight / 2.f);
	m_background = ModelFactory::PlaneModel::Create(texPlaneHalfSize);
	m_background->buildBufferForShader(&m_colorShader);
	m_background->getMaterial()->setColor(Vector4(0, 0, 0, 0.8f));
	m_background->getTransform().setTranslation(Vector3(0, 0, 2));
	m_background->getTransform().setRotations(Vector3(-1.62f, 0, 0));

	this->m_selector = 0;
	this->startTimer = 0;
	
	

	MenuItem* start = new MenuItem(m_menuOn->getModel(), Vector3(1, 3, 3));
	MenuItem* something = new MenuItem(m_menuOff->getModel(), Vector3(1, 2, 3));
	MenuItem* exit = new MenuItem(m_menuOff->getModel(), Vector3(1, 1, 3));
	

	this->m_menuList.push_back(start);
	this->m_menuList.push_back(something);
	this->m_menuList.push_back(exit);


	for (size_t i = 0; i < this->m_menuList.size(); i++) {
		m_scene.addObject(m_menuList[i]);
	}


	m_playerCamController->setTargets(
		this->m_menuList[0],
		nullptr,
		nullptr,
		nullptr
	);
	m_playerCamController->setMoving(false);
	m_playerCamController->setPosition(this->m_menuList[1]->getTransform().getTranslation());
	m_playerCamController->setTarget(this->m_menuList[1]->getTransform().getTranslation());
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

	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));

	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));


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
	m_scene.draw(dt, m_cam, nullptr, nullptr, nullptr);

	m_colorShader.updateCamera(m_cam);

	// Draw HUD
	m_scene.drawHUD();

	return true;
}

void PauseState::beginStartTimer()
{
	this->startTimer += 0.01f;
}

void PauseState::changeMenu(int change)
{
	this->m_menuList[this->m_selector]->setModel(m_menuOff->getModel());
	this->m_selector += change;
	if (this->m_selector < 0)
		this->m_selector = 2;
	if (this->m_selector > 2)
		this->m_selector = 0;

	this->m_menuList[this->m_selector]->setModel(m_menuOn->getModel());
	m_playerCamController->setTargets(
		this->m_menuList[this->m_selector],
		nullptr,
		nullptr,
		nullptr
	);
}
