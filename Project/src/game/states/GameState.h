#pragma once

#include "../../sail/Sail.h"
#include "../PlayerCameraController.h"
#include "../CharacterHandler.h"
#include "../level/Level.h"
#include "../ProjectileHandler.h"
#include "../objects/Hook.h"
#include "../collision/CollisionHandler.h"

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

	// Camera
	PerspectiveCamera m_cam;
	FlyingCameraController m_camController;
	std::unique_ptr<PlayerCameraController> m_playerCamController;
	bool m_flyCam;

	// Scene
	Scene m_scene;

	// Models
	//std::unique_ptr<FbxModel> m_sphere;

	// Texts
	SailFont m_font;
	Text m_fpsText;
	Text m_debugCamText;

	// Handlers
	std::unique_ptr<Level> m_level;
	std::unique_ptr<ProjectileHandler> m_projHandler;
	std::unique_ptr<CollisionHandler> m_collisionHandler;
	std::unique_ptr<CharacterHandler> m_characterHandler;




};