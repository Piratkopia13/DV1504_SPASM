#pragma once

#include "../../sail/Sail.h"

class CharacterHandler;
class Gamemode {
public:
	Gamemode();
	virtual ~Gamemode();

	virtual void update(CharacterHandler* charHandler, float delta);
	virtual void draw();

	int getScore(const int team);
	
protected:
	void addScore(const int toAdd, const int team);

private:
	int m_scoreTeamOne, m_scoreTeamTwo;
};