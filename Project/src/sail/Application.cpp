#include "Application.h"

using namespace DirectX;

Application* Application::m_instance = nullptr;

LONG Application::Input::m_frameDeltaAccumulationMouseDX = 0;
LONG Application::Input::m_frameDeltaAccumulationMouseDY = 0;
LONG Application::Input::m_mouseDXSinceLastFrame = 0;
LONG Application::Input::m_mouseDYSinceLastFrame = 0;
bool Application::Input::m_mouseButtons[2] = { false, false };
bool Application::Input::m_mouseButtonsPressedSinceLastFrame[2] = {false, false};

Application::Application(int windowWidth, int windowHeight, char* windowTitle, HINSTANCE hInstance)
: m_window(hInstance, windowWidth, windowHeight, windowTitle)
{
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

	// Init dxtk keyboard, mouse and gamepad input
	m_input.keyboard = std::make_unique<Keyboard>();
	m_input.gamepad = std::make_unique<GamePad>();

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
}

Application::~Application() {	}

int Application::startGameLoop() {

	// Start delta timer
	m_timer.startTimer();

	MSG msg = {0};

	m_fps = 0;

	float secCounter = 0.f;
	UINT frameCounter = 0;

	float updateTimer = 0.f;
	float timeBetweenUpdates = 1.f / 30.f;

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
			//delta = min(delta, 0.0008f);

			// Update fps counter
			secCounter += delta;
			frameCounter++;
			if (secCounter >= 1) {
				m_fps = frameCounter;
				frameCounter = 0;
				secCounter = 0.f;
				//std::cout << "FPS: " << m_fps << std::endl;
			}

			// Update input states
			m_input.gamepadState = GamePad::Get().GetState(0);
			m_input.keyboardState = Keyboard::Get().GetState();


			// Update mouse deltas
			m_input.newFrame();

			// Quit on escape or alt-f4
			if (m_input.keyboardState.Escape || m_input.keyboardState.LeftAlt && m_input.keyboardState.F4)
				PostQuitMessage(0);

			processInput(delta);

			// Update
			if(delta < 1.f)
				updateTimer += delta;

			while (updateTimer >= timeBetweenUpdates) {
				update(timeBetweenUpdates);
				updateTimer -= timeBetweenUpdates;
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

Application::Input& Application::getInput() {
	return m_input;
}
