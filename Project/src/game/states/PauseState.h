#pragma once
#include "../../sail/Sail.h"
#include "../PlayerCameraController.h"
#include "../objects/menu/MenuItem.h"

class PauseState :
	public State
{
public:
	PauseState(StateStack& stack);
	~PauseState();

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
	//Text m_startText;

	// Shaders
	SimpleColorShader m_colorShader;

	// Models
	std::unique_ptr<FbxModel> m_fbxModel;
	std::unique_ptr<FbxModel> m_menuOn;
	std::unique_ptr<FbxModel> m_menuOff;
	std::unique_ptr<Model> m_background;

	int m_selector;
	std::vector<MenuItem*> m_menuList;

private:
	float startTimer;
	void beginStartTimer();
	void changeMenu(int change);

};

