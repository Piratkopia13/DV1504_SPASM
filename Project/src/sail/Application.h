#pragma once

#include <windowsx.h>
#include "utils/Timer.h"
#include "Win32Window.h"
#include "DXManager.h"
#include "resources/ResourceManager.h"
#include "Input.h"

class Application {

public:
	struct Profile {
		std::string name;
		size_t kills;
		size_t deaths;
		size_t wins;
		size_t losses;
		float winRatio;
		bool preOrder;
	};
	
	struct GameSettings {
		struct player {
			Profile* profile = nullptr;
			int port;
			int team;
			DirectX::SimpleMath::Vector4 color;
			int model;
			bool ready;
		};
		int used;
		int level;
		int gamemode;
		int scoreLimit;
		int timeLimit;
		float respawnTime;
		float gravityScale;

		int teamColors[2];
		DirectX::SimpleMath::Vector4 teamOneColor;
		DirectX::SimpleMath::Vector4 teamTwoColor;
		std::vector<player> players;
		void reset() {
			used = 0;
			level = 0;
			gamemode = 0;
			scoreLimit = 0;
			timeLimit = 0;
			respawnTime = 2;
			gravityScale = 1;

			teamColors[0] = 0;
			teamColors[1] = 1;
			players.clear();
		};
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
	std::vector<Profile>& getProfiles();

private:
	static Application* m_instance;
	Win32Window m_window;
	DXManager m_dxManager;
	ResourceManager m_resourceManager;

	Timer m_timer;
	UINT m_fps;

	Input m_input;
	GameSettings m_gameVariables;
	std::vector<Profile> m_profiles;

};