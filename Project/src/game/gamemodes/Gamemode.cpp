#include "Gamemode.h"

Gamemode::Gamemode() {
	m_scoreTeamOne = 0;
	m_scoreTeamTwo = 0;

	m_gametime = 60.f;
}

Gamemode::~Gamemode() {

}

void Gamemode::update(CharacterHandler* charHandler, float delta) {
	m_gametime -= delta;
}

void Gamemode::draw() {

}

float Gamemode::getScore(const int team) {
	switch (team) {
	case 1:
		return m_scoreTeamOne;
	case 2:
		return m_scoreTeamTwo;
	default:
		return -1;
	}
}

float Gamemode::getGametime() {
	return m_gametime;
}

void Gamemode::addScore(const float toAdd, const int team) {
	switch (team) {
	case 1:
		m_scoreTeamOne += toAdd;
		break;
	case 2:
		m_scoreTeamTwo += toAdd;
		break;
	default:
		break;
	}
}

void Gamemode::setGametime(const float& seconds) {
	m_gametime = seconds;
}

void Gamemode::addGametime(const float& seconds) {
	m_gametime += seconds;
}