#pragma once

#include "../../sail/Sail.h"
#include "../objects/Character.h"
#include "../objects/Block.h"

class Gamemode {
public:
	Gamemode();
	~Gamemode();

	virtual void update(Character* player, float delta);
	virtual void draw();

	int getScore(const int team);
	
protected:
	void addScore(const int toAdd, const int team);

private:
	int m_scoreTeamOne, m_scoreTeamTwo;
};