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
	Model* m_playerModel;
	Model* m_menuStartModel;

	Model* m_menuOptionsModel;
	Model* m_menuExitModel;

	Model* m_menuBlockModel;
	Model* m_backGroundModel;

	// Texts
	SailFont m_font;
	Text m_fpsText;
	Text m_debugCamText;

	DirectX::SimpleMath::Vector4 m_onColor;
	DirectX::SimpleMath::Vector4 m_offColor;
	DirectX::SimpleMath::Vector4 m_orangeColor;
	DirectX::SimpleMath::Vector4 m_blueColor;



	// MENU 0

	enum m_activeMenu {
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


	int m_activeMenu;
	int m_activeSubMenu;
	int m_selector;
	int m_menu;
	int m_startMenu;
	int m_maxChoices;


	int m_gamemode;
	int m_map;
	int players[4];
	int playersReady[4];
	int m_playerModelIndex[4];
	int m_playerTeam[4];

	DirectX::SimpleMath::Vector4 playerColor[4];

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
