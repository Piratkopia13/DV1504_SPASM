#pragma once
#include "../../sail/Sail.h"
#include "../PlayerCameraController.h"
#include "../objects/menu/MenuHandler.h"
#include "../GameInfo.h"

class ScoreState : public State {
public:
	ScoreState(StateStack& stack);
	~ScoreState();

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


	// Texts
	SailFont m_font;
	Text m_fpsText;
	Text m_debugCamText;



	
	struct ScoreLine {
		MenuText* profileName;
		MenuText* kills;
		MenuText* deaths;
		MenuText* score;
		MenuText* captures;

		void clear() {
			Memory::safeDelete(profileName);
			Memory::safeDelete(kills);
			Memory::safeDelete(deaths);
			Memory::safeDelete(score);
			Memory::safeDelete(captures);
		}
	};
	

	std::vector<ScoreLine> m_scoreLine;


};