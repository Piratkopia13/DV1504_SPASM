#pragma once

#include "../Gamemode.h"

class Deathmatch : public Gamemode {

public:
	Deathmatch();
	~Deathmatch();

	virtual void update(CharacterHandler* charHandler, float delta);
	virtual void draw();

	virtual int checkWin();

private:
	size_t m_maxKills;
	int m_winnerID;

};