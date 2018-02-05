#pragma once

#include <windowsx.h>
#include "utils/Timer.h"
#include "Win32Window.h"
#include "DXManager.h"
#include "resources/ResourceManager.h"

class Application {

public:
	struct Input {
		friend class Application;
	public:
		enum MouseButton {
			LEFT,
			RIGHT
		};



		std::unique_ptr<DirectX::Keyboard> keyboard;
		std::unique_ptr<DirectX::GamePad> gamepad;
		DirectX::Keyboard::State keyboardState;
		DirectX::GamePad::State gamepadState[4];

		void showCursor(bool show) {
			ShowCursor(show);
		}
		bool isCursorHidden() {
			CURSORINFO pci = { 0 };
			pci.cbSize = sizeof(CURSORINFO);
			GetCursorInfo(&pci);
			return pci.flags == 0;
		}

		static LONG getMouseDX() {
			return m_mouseDXSinceLastFrame;
		}
		static LONG getMouseDY() {
			return m_mouseDYSinceLastFrame;
		}
		static bool isMouseBtnPressed(MouseButton btn) {
			return m_mouseButtons[btn];
		}
		static bool wasJustPressed(MouseButton btn) {
			return m_mouseButtons[btn] && !m_mouseButtonsPressedSinceLastFrame[btn];
		}

		static void processMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
			UINT dwSize;

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == NULL) {
				return;
			}

			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
				OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE) {

				// Accumulate deltas until the next frame
				m_frameDeltaAccumulationMouseDX += raw->data.mouse.lLastX;
				m_frameDeltaAccumulationMouseDY += raw->data.mouse.lLastY;

				// Update mouse button
				if ((RI_MOUSE_LEFT_BUTTON_DOWN & raw->data.mouse.usButtonFlags) == RI_MOUSE_LEFT_BUTTON_DOWN)
					m_mouseButtons[MouseButton::LEFT] = true;
				if ((RI_MOUSE_LEFT_BUTTON_UP & raw->data.mouse.usButtonFlags) == RI_MOUSE_LEFT_BUTTON_UP)
					m_mouseButtons[MouseButton::LEFT] = false;
				if ((RI_MOUSE_RIGHT_BUTTON_DOWN & raw->data.mouse.usButtonFlags) == RI_MOUSE_RIGHT_BUTTON_DOWN)
					m_mouseButtons[MouseButton::RIGHT] = true;
				if ((RI_MOUSE_RIGHT_BUTTON_UP & raw->data.mouse.usButtonFlags) == RI_MOUSE_RIGHT_BUTTON_UP)
					m_mouseButtons[MouseButton::RIGHT] = false;

			}

			delete[] lpb;
		}

	

	private:
		void registerRawDevices(HWND hwnd) {
			// Register mouse for raw input
			RAWINPUTDEVICE Rid[1];

			Rid[0].usUsagePage = 0x01;
			Rid[0].usUsage = 0x02;
			Rid[0].dwFlags = RIDEV_INPUTSINK;   // adds HID mouse and also ignores legacy mouse messages
			Rid[0].hwndTarget = hwnd;

			if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
				//registration failed. Call GetLastError for the cause of the error
				OutputDebugString(L"Could not register mouse for raw input");
			}
		}

		void newFrame() {
			// Set cursor position to center of window if the cursor is hidden
			if (isCursorHidden()) {
				POINT p;
				p.x = Application::getInstance()->getWindow()->getWindowWidth() / 2;
				p.y = Application::getInstance()->getWindow()->getWindowHeight() / 2;
				ClientToScreen(*Application::getInstance()->getWindow()->getHwnd(), &p);
				SetCursorPos(p.x, p.y);
			}

			m_mouseDXSinceLastFrame = m_frameDeltaAccumulationMouseDX;
			m_mouseDYSinceLastFrame = m_frameDeltaAccumulationMouseDY;
			m_frameDeltaAccumulationMouseDX = 0;
			m_frameDeltaAccumulationMouseDY = 0;
		}
		void endOfFrame() {
			m_mouseButtonsPressedSinceLastFrame[0] = m_mouseButtons[0];
			m_mouseButtonsPressedSinceLastFrame[1] = m_mouseButtons[1];
		}

		static bool m_mouseButtons[2];
		static bool m_mouseButtonsPressedSinceLastFrame[2];

		static LONG m_frameDeltaAccumulationMouseDX;
		static LONG m_frameDeltaAccumulationMouseDY;
		static LONG m_mouseDXSinceLastFrame;
		static LONG m_mouseDYSinceLastFrame;



	};
	struct GameSettings {
		int used;
		int players;
		int ports[4];
		int teams[4];
		int model[4];
		DirectX::SimpleMath::Vector4 color[4];
		int level;
		int gamemode;


	};
public:
	Application(int windowWidth, int windowHeight, char* windowTitle, HINSTANCE hInstance);
	virtual ~Application();

	int startGameLoop();

	// Required methods
	virtual int run() = 0;
	virtual void processInput(float dt) = 0;
	virtual void update(float dt) = 0;
	virtual void render(float dt) = 0;

	// Optional methods
	// resize() is called when the window is resized
	virtual void resize(int width, int height) {};

	static Application* getInstance();
	DXManager* const getDXManager();
	Win32Window* const getWindow();
	ResourceManager& getResourceManager();
	const UINT getFPS() const;

	Input& getInput();
	GameSettings& getGameSettings();

private:
	static Application* m_instance;
	Win32Window m_window;
	DXManager m_dxManager;
	ResourceManager m_resourceManager;

	Timer m_timer;
	UINT m_fps;

	Input m_input;
	GameSettings m_gameVariables =
	{
		0,
		0,
		{ -1, -1, -1, -1 },
		{ -1, -1, -1, -1 },
		{ -1, -1, -1, -1 },
		{
			DirectX::SimpleMath::Vector4(1,1,1,1),
			DirectX::SimpleMath::Vector4(1,1,1,1),
			DirectX::SimpleMath::Vector4(1,1,1,1),
			DirectX::SimpleMath::Vector4(1,1,1,1)
		},
		0,
		0
	};
};