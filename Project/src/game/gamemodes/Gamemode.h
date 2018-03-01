#pragma once

#include "../../sail/Sail.h"

class CharacterHandler;
class Gamemode {
public:
	Gamemode();
	virtual ~Gamemode();

	virtual void update(CharacterHandler* charHandler, float delta);
	virtual void draw();

	const std::vector<float> & getScore();
	const float & getScore(const unsigned int team);
	float getGametime();

	virtual int checkWin() = 0;
	
	void addScore(const float toAdd, const unsigned int team);
protected:
	void setScore(const float score, const unsigned int team);
	void setGametime(const float& seconds);
	void addGametime(const float& seconds);

private:
	std::vector<float> m_teamScore;
	float m_gametime;
};