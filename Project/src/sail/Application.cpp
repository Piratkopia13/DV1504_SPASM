#include "Application.h"

using namespace DirectX;

Application* Application::m_instance = nullptr;

Application::Application(int windowWidth, int windowHeight, char* windowTitle, HINSTANCE hInstance)
: m_window(hInstance, windowWidth, windowHeight, windowTitle)
{
	// Set default team colors
	m_gameVariables.teamOneColor = DirectX::SimpleMath::Vector4(1.0f, 0.5f, 0.0f, 1.0f);
	m_gameVariables.teamTwoColor = DirectX::SimpleMath::Vector4(0.0f, 0.9f, 1.0f, 1.0f);

	// Initalize the window
	if (!m_window.initialize()) {
		OutputDebugString(L"\nFailed to initialize Win32Window\n");
		Logger::Error("Failed to initialize Win32Window!");
		return;
	}

	// Initialize Direct3D
	if (!m_dxManager.initDirect3D(m_window.getHwnd(), m_window.getWindowWidth(), m_window.getWindowHeight(), 1)) {
		OutputDebugString(L"\nFailed to initialize D3D\n");
		Logger::Error("Failed to initialize Direct3D!");
		return;
	}

	// Register devices to use raw input from hardware
	m_input.registerRawDevices(*m_window.getHwnd());

	// Set up instance if not set
	if (m_instance) {
		Logger::Error("Only one application can exist!");
		return;
	}
	m_instance = this;

	// Load the missing texture texture
	m_resourceManager.LoadDXTexture("missing.tga");

	Profile temp1 = { "testA",0,0,0,0,0.f};
	Profile temp2 = { "testB",0,0,0,0,0.f };
	Profile temp3 = { "testC",0,0,0,0,0.f };
	Profile temp4 = { "testD",0,0,0,0,0.f };
	m_profiles.push_back(temp1);
	m_profiles.push_back(temp2);
	m_profiles.push_back(temp3);
	m_profiles.push_back(temp4);
}

Application::~Application() {	}

int Application::startGameLoop() {

	// Start delta timer
	m_timer.startTimer();
	
	MSG msg = {0};

	m_fps = 0;

	float secCounter = 0.f;
	float elapsedTime = 0.f;
	UINT frameCounter = 0;

	float updateTimer = 0.f;
	float timeBetweenUpdates = 1.f / 60.f;

	static_cast<float>(m_timer.getFrameTime());

	// Main message loop
	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {

			// Handle window resizing
			if (m_window.hasBeenResized()) {
				int newWidth = m_window.getWindowWidth();
				int newHeight = m_window.getWindowHeight();
				m_dxManager.resize(newWidth, newHeight);
				resize(newWidth, newHeight);
			}
			
			// Get delta time from last frame
			float delta = static_cast<float>(m_timer.getFrameTime());
			delta = min(delta, 0.04f);

			// Update fps counter
			secCounter += delta;
			frameCounter++;

			if (secCounter >= 1) {
				m_fps = frameCounter;
				frameCounter = 0;
				secCounter = 0.f;
			}

			// Update input states
			m_input.updateStates();

			// Update mouse deltas
			m_input.newFrame();

			// Quit on escape or alt-f4
			if (m_input.getKeyboardState().LeftAlt && m_input.getKeyboardState().F4)
				PostQuitMessage(0);

			processInput(delta);

			// Update
#ifdef _DEBUG
			if (m_input.getKeyboardState().Escape)
				PostQuitMessage(0);


			//if(delta > 0.0166)
			//	Logger::Warning(std::to_string(elapsedTime) + " delta over 0.0166: " + std::to_string(delta));
#endif
			updateTimer += delta;

			int maxCounter = 0;
		

			while (updateTimer >= timeBetweenUpdates) {
				if (maxCounter >= 4)
					break;
				update(timeBetweenUpdates);
				updateTimer -= timeBetweenUpdates;
				maxCounter++;
			}

			// Render
			render(delta);

			// Update wasJustPressed bools
			m_input.endOfFrame();
		}

	}

	return (int)msg.wParam;

}

Application* Application::getInstance() {
	if (!m_instance)
		Logger::Error("Application instance not set, you need to initialize the class which inherits from Application before calling getInstance().");
	return m_instance;
}

DXManager* const Application::getDXManager() {
	return &m_dxManager;
}
Win32Window* const Application::getWindow() {
	return &m_window;
}
ResourceManager& Application::getResourceManager() {
	return m_resourceManager;
}
const UINT Application::getFPS() const {
	return m_fps;
}

Input& Application::getInput() {
	return m_input;
}

Application::GameSettings & Application::getGameSettings()
{
	return m_gameVariables;
}

std::vector<Application::Profile>& Application::getProfiles()
{
	return m_profiles;
}

