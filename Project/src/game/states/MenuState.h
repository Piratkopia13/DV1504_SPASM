#pragma once
#include "../../sail/Sail.h"
#include "../PlayerCameraController.h"
#include "../objects/menu/MenuItem.h"

class MenuState : public State {
public:
	MenuState(StateStack& stack);
	~MenuState();

	// Process input for the state
	virtual bool processInput(float dt);
	// Process window resizing for the state
	virtual bool resize(int width, int height);
	// Updates the state
	virtual bool update(float dt);
	// Renders the state
	virtual bool render(float dt);

private:
	Application * m_app;

	PerspectiveCamera m_cam;
	OrthographicCamera m_hudCam;
	PlayerCameraController m_playerCamController;
	Scene m_scene;


	SimpleColorShader m_colorShader;
	SimpleTextureShader m_texShader;
	SimpleTextureShader m_hudShader;
	DirectionalLightShader m_dirLightShader;
	MaterialShader m_matShader;
	SailFont m_font;

	Timer m_timer;

	Text m_fpsText;
	Text m_debugText;
	Text m_debugCamText;
	Text m_debugParticleText;


	std::unique_ptr<FbxModel> m_player;
	std::unique_ptr<FbxModel> m_menuStart;

	std::unique_ptr<FbxModel> m_menuOptions;
	std::unique_ptr<FbxModel> m_menuExit;

	std::unique_ptr<FbxModel> m_menuBlock;
	std::unique_ptr<FbxModel> m_backGround;


	DirectX::SimpleMath::Vector4 onColor;
	DirectX::SimpleMath::Vector4 offColor;

	DirectX::SimpleMath::Vector4 playerColor[4];


	// MENU 0

	enum ActiveMenu {
		MAINMENU,
		STARTMENU,
		OPTIONSMENU
	};

	enum MainMenu {
		NOTHING = -1,
		START,
		OPTIONS,
		EXIT
	};

	enum StartMenu {
		PLAYERSELECT,
		MAPSELECT
	};

	enum OptionsMenu {
		CHOICE0,
		CHOICE1,
		CHOICE2
	};

	enum PlayerSelect {
		OFFLINE,
		ONLINE,
		NOTREADY = 0,
		READY
	};


	int activeMenu;
	int activeSubMenu;
	int selector;
	int menu;
	int startMenu;
	int maxChoices;

	int players[4];
	int playersReady[4];

	MenuItem* background;
	std::vector<MenuItem*> menuList;
	std::vector<MenuItem*> playerMenu;
	std::vector<MenuItem*> mapMenu;

	std::vector<MenuItem*> optionsMenuList;


	void changeMenu(int change, int active);
	void setColor(int player, DirectX::SimpleMath::Vector4 color) {
		this->playerColor[player] = color;
		this->playerMenu[player]->setLightColor(this->playerColor[player]);
	}
	DirectX::SimpleMath::Vector4 getRandomColor() {
		return DirectX::SimpleMath::Vector4(Utils::rnd(), Utils::rnd(), Utils::rnd(), 1);
	}
};

