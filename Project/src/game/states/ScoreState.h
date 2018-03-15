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
		std::vector<MenuText*> text;

		void clear() {
			for (size_t i = 0; i < text.size(); i++) {
				Memory::safeDelete(text[i]);
			}
		}
		
	};
	
	ScoreLine m_winner;
	ScoreLine m_topLine;
	std::vector<ScoreLine> m_scoreLine;
	std::vector<bool> m_ready;


private:
	void addScore();
	void setPositions();
	void exitScoreBoard();

};