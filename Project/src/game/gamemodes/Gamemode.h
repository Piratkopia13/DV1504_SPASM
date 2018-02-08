#pragma once

#include "../../sail/Sail.h"

class CharacterHandler;
class Gamemode {
public:
	Gamemode();
	virtual ~Gamemode();

	virtual void update(CharacterHandler* charHandler, float delta);
	virtual void draw();

	float getScore(const int team);
	
protected:
	void addScore(const float toAdd, const int team);

private:
	float m_scoreTeamOne, m_scoreTeamTwo;
};