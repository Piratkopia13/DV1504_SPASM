#pragma once
#include "../../sail/Sail.h"
#include "../PlayerCameraController.h"
#include "../objects/menu/MenuHandler.h"
#include "../GameInfo.h"

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
	GameInfo* m_info;

	// Camera
	PerspectiveCamera m_cam;
	std::unique_ptr<PlayerCameraController> m_playerCamController;
	bool m_flyCam;

	// Scene
	Scene m_scene;

	// Models

	std::vector<Model*> m_playerHeadModels;
	std::vector<Model*> m_playerBodyModels;
	std::vector<Model*> m_playerLegModels;
	std::vector<Model*> m_playerArmLModels;
	std::vector<Model*> m_playerArmRModels;


	Model* m_backGroundModel;

	Model* m_block;



	// Texts
	SailFont m_font;
	Text m_fpsText;
	Text m_debugCamText;

	DirectX::SimpleMath::Vector4 m_onColor;
	DirectX::SimpleMath::Vector4 m_offColor;

	std::vector<DirectX::SimpleMath::Vector4> m_teamColors;


	DirectX::SimpleMath::Vector4 m_orangeColor;
	
	// MENU 0

	enum m_activeMenu {
		MAINMENU,
		STARTMENU,
		PROFILEMENU,
		OPTIONSMENU
	};

	enum MainMenuSelections {
		START,
		PROFILE,
		OPTIONS,
		EXIT
	};

	enum StartMenuPart {
		GAMEOPTIONSELECT,
		PLAYERSELECT,
		MAPSELECT
	};
	
	enum GameOptionsOptions {
		GAMEMODE,
		TIMELIMIT,
		SCORELIMIT,
		RESPAWNTIME,
		GRAVITY,
		PLAYERLIFE

	};

	enum GameModeSelection {
		PAYLOAD,
		DEATHMATCH,
		TEAMDEATHMATCH
	};


	enum ProfileMenu {
		PMAIN,
		PCREATE,
		PVIEW
	};

	enum OptionsMenu {
		MAIN,
		GRAPHICS,
		SOUND
	};

	enum GraphicsMenu {
		PARTICLES,
		BLOOM,
		ANTIALIASING,
		BACKGROUND,
		FPSCOUNTER,
		VSYNC,
		WTFG
	};
	enum SoundMenu {
		MASTERVOLUME,
		BACKGROUNDVOLUME,
		EFFECTVOLUME,
		WTFS
	};


	enum PlayerSelect {
		OFFLINE,
		ONLINE,
		NOTREADY = 0,
		READY
	};



	int m_activeMenu;
	int m_activeSubMenu;


	MenuItem* background;
	

	//MAIN MENU
	MenuHandler* m_mainMenu;

	//GAMEMODE MENU
	MenuHandler* m_gamemodeMenu;

	std::vector<MenuHandler*> m_characterMenu;

	struct MenuPlayer {
		GameInfo::Player player;
		bool ready;

	};
	std::vector<MenuPlayer*> m_playerz;
	//std::vector<MenuItem*> m_playerMenuModels;

	struct PlayerMenuModel {
		MenuItem* head;
		MenuItem* body;
		MenuItem* legs;
		MenuItem* armL;
		MenuItem* armR;

		void setLight(const DirectX::SimpleMath::Vector4& color) {
			if (head) 
				head->setLightColor(color);
			if (body)
				body->setLightColor(color);
			if (legs)
				legs->setLightColor(color);
			if (armL)
				armL->setLightColor(color);
			if (armR)
				armR->setLightColor(color);
		}
		void clear() {
			Memory::safeDelete(head);
			Memory::safeDelete(body);
			Memory::safeDelete(legs);
			Memory::safeDelete(armL);
			Memory::safeDelete(armR);
		}
		void reset() {
			if(head)
				head->setModel(nullptr);
			if(body)
				body->setModel(nullptr);
			if(legs)
				legs->setModel(nullptr);
			if(armL)
				armL->setModel(nullptr);
			if(armR)
				armR->setModel(nullptr);
		}

	};
	std::vector<PlayerMenuModel> m_playerMenuModelz;
	PlayerMenuModel m_graphicsModel;

	MenuHandler* m_mapMenu;

	//OPTIONS N SHIT
	MenuHandler* m_profileMenu;
	MenuHandler* m_profileCreator;
	MenuHandler* m_profileViewer;
	MenuHandler* m_profileViewerStats;
	

	MenuHandler* m_optionsMenu;
	MenuHandler* m_graphicsMenu;
	MenuHandler* m_soundMenu;
private:
	void initMain();
	void initGamemode();
	void initCharacterModels();
	void initCharacterModel(size_t spot);
	void initCharacter(size_t spot, bool online);
	void removeCharacter(size_t spot);
	void initMap();

	void initProfile();
	void initProfileCreator();
	void initProfileViewer();

	void initOptions();
	void initGraphics();
	void initSound();



	void setMainSelect(bool active);
	void setGamemodeSelect(bool active);
	void setCharacterSelect(bool active);
	void setMapSelect(bool active);

	void setProfileMenu(bool active);
	void setProfileCreator(bool active);
	void setProfileViewer(bool active);

	void setOptionsMenu(bool active);
	void setGraphicsMenu(bool active);
	void setSoundMenu(bool active);

	void updateCamera();
	
	void startGame();
	void updateGraphics();
	void updateSound();

	
};

