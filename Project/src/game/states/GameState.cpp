#include "GameState.h"

GameState::GameState(StateStack& stack)
: State(stack)
{

}

// Process input for the state
bool GameState::processInput(float dt) {

	return true;
}
// Updates the state
bool GameState::update(float dt) {

	return true;
}
// Renders the state
bool GameState::render(float dt) {

	return true;
}