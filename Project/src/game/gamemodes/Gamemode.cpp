#include "Gamemode.h"

Gamemode::Gamemode() {
	m_scoreTeamOne = 0;
	m_scoreTeamTwo = 0;
}

Gamemode::~Gamemode() {

}

void Gamemode::update(Character* player, float delta) {

}

void Gamemode::draw() {

}

int Gamemode::getScore(const int team) {
	switch (team) {
	case 1:
		return m_scoreTeamOne;
	case 2:
		return m_scoreTeamTwo;
	default:
		return -1;
	}
}

void Gamemode::addScore(const int toAdd, const int team) {
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