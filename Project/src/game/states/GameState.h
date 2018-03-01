#pragma once

#include "../../sail/Sail.h"
#include "../PlayerCameraController.h"
#include "../CharacterHandler.h"
#include "../level/Level.h"
#include "../ProjectileHandler.h"
#include "../objects/Hook.h"
#include "../collision/CollisionHandler.h"
#include "../UpgradeHandler.h"
#include "../gamemodes/Gamemode.h"
#include "../ParticleHandler.h"
#include "../background/ScoreVisualization.h"

#include "../objects/Block.h"

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
	std::unique_ptr<Model> m_infinityPlane;
	std::unique_ptr<Block> m_infBottom;
	std::unique_ptr<Block> m_infTop;
	std::unique_ptr<Block> m_infLeft;
	std::unique_ptr<Block> m_infRight;

	// Texts
	SailFont m_font;
	Text m_fpsText;
	Text m_debugCamText;

	// Handlers
	std::unique_ptr<Level> m_level;
	std::unique_ptr<ProjectileHandler> m_projHandler;
	std::unique_ptr<CollisionHandler> m_collisionHandler;
	std::unique_ptr<CharacterHandler> m_characterHandler;
	std::unique_ptr<UpgradeHandler> m_upgradeHandler;
	std::unique_ptr<ParticleHandler> m_particleHandler;

	// The current gamemode
	std::unique_ptr<Gamemode> m_gamemode;

	std::unique_ptr<ScoreVisualization> m_scoreVisualization;
};