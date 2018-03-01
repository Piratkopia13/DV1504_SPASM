#pragma once

#include "../Gamemode.h"

class TeamDeathmatch : public Gamemode {

public:
	TeamDeathmatch();
	~TeamDeathmatch();

	virtual void update(CharacterHandler* charHandler, float delta);
	virtual void draw();

	virtual int checkWin();

private:
	float m_maxKills;
	int m_winnerID;

};