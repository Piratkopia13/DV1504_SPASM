#include "Game.h"
#include "states/GameState.h"
#include "states/MenuState.h"
#include "states/PauseState.h"

Game::Game(HINSTANCE hInstance)
	: Application(1280, 720, "SPASM v0.50", hInstance)
	, m_stateStack()
{
	// Register states
	registerStates();
	// Set starting state
#ifdef _DEBUG
	m_stateStack.pushState(States::Game);
#else
	m_stateStack.pushState(States::MainMenu);
#endif

}

Game::~Game() {	}

int Game::run() {
	
	// Start the game loop and return when game exits
	return startGameLoop();
}

void Game::registerStates() {

	// Register all of the different states
	m_stateStack.registerState<GameState>(States::Game);
	m_stateStack.registerState<MenuState>(States::MainMenu);
	m_stateStack.registerState<PauseState>(States::Pause);
}

void Game::resize(int width, int height) {
	m_stateStack.resize(width, height);
}

void Game::processInput(float dt) {
	m_stateStack.processInput(dt);
}

void Game::update(float dt) {
	m_stateStack.update(dt);
}

void Game::render(float dt) {
	m_stateStack.render(dt);
}