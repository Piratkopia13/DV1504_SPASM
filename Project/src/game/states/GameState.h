#pragma once

#include "../../sail/states/StateStack.h"

class GameState : public State {
public:
	GameState(StateStack& stack);

	// Process input for the state
	virtual bool processInput(float dt);
	// Updates the state
	virtual bool update(float dt);
	// Renders the state
	virtual bool render(float dt);

private:
	
};