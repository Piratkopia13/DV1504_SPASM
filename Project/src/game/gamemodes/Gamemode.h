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
	float getGametime();

	virtual int checkWin() = 0;
	
protected:
	void addScore(const float toAdd, const int team);
	void setGametime(const float& seconds);
	void addGametime(const float& seconds);

private:
	float m_scoreTeamOne, m_scoreTeamTwo;
	float m_gametime;
};