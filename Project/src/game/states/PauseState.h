#pragma once
#include "../../sail/Sail.h"
#include "../PlayerCameraController.h"
#include "../objects/menu/MenuItem.h"
#include "Variables.h"

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

	Text m_startText;

	std::unique_ptr<FbxModel> m_fbxModel;
	std::unique_ptr<FbxModel> m_menuOn;
	std::unique_ptr<FbxModel> m_menuOff;


	std::unique_ptr<Model> m_background;

	int selector;
	std::vector<MenuItem*> menuList;
	float startTimer;
	void beginStartTimer();

	void changeMenu(int change);




};

