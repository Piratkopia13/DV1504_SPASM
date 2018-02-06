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

	// Camera
	PerspectiveCamera m_cam;
	std::unique_ptr<PlayerCameraController> m_playerCamController;
	bool m_flyCam;

	// Scene
	Scene m_scene;

	// Models
	std::unique_ptr<FbxModel> m_sphere;
	std::unique_ptr<Model> m_plane;
	std::unique_ptr<Model> m_texturePlane;
	std::unique_ptr<Model> m_texturePlane2;

	// Texts
	SailFont m_font;
	Text m_fpsText;
	Text m_debugCamText;


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
	
};

