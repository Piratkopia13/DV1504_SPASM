#pragma once

#include "../../sail/Sail.h"
#include "../PlayerCameraController.h"
#include "../objects/Character.h"
#include "../level/Level.h"
#include "../ProjectileHandler.h"

class GameState : public State {
public:
	GameState(StateStack& stack);
	~GameState();

	// Process input for the state
	virtual bool processInput(float dt);
	// Process window resizing for the state
	virtual bool resize(int width, int height);
	// Updates the state
	virtual bool update(float dt);
	// Renders the state
	virtual bool render(float dt);

private:
	Application* m_app;

	PerspectiveCamera m_cam;
	OrthographicCamera m_hudCam;
	FlyingCameraController m_camController;
	Scene m_scene;
	std::unique_ptr<FbxModel> m_sphere;
	std::unique_ptr<Model> m_plane;
	std::unique_ptr<Model> m_texturePlane;
	std::unique_ptr<Model> m_texturePlane2;


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

	PlayerCameraController m_playerCamController;
	bool m_flyCam;

	std::vector<Model*> m_modelCopies;

	std::unique_ptr<FbxModel> m_fbxModel;
	std::unique_ptr<FbxModel> m_characterModel;
	std::unique_ptr<FbxModel> m_WeaponModel1;

	// TEST REMOVE THIS
	std::vector<std::unique_ptr<Model>> models;

	// Currently used level
	std::unique_ptr<Level> m_currLevel;

	Weapon* weapons[4];
	Character* player[4];
	ProjectileHandler* projHandler;

};