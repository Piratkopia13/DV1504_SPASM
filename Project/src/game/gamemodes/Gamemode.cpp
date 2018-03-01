#include "Gamemode.h"

Gamemode::Gamemode() {
	m_teamScore.resize(4);
	for (int i = 0; i < 4; i++) {
		m_teamScore[i] = 0;
	}

	m_gametime = 60.f;
}

Gamemode::~Gamemode() {

}

void Gamemode::update(CharacterHandler* charHandler, float delta) {
	m_gametime -= delta;
}

void Gamemode::draw() {

}

const std::vector<float> & Gamemode::getScore() {
	return m_teamScore;
}

const float & Gamemode::getScore(const int team) {
	if (team < 0 || team > 4) {
		Logger::Warning("Tried to fetch an out of bounds teamscore");
		return 0.f;
	}

	return m_teamScore[team];
}

float Gamemode::getGametime() {
	return m_gametime;
}

void Gamemode::addScore(const float toAdd, const int team) {
	if (team < 0 || team > 4)
		return;

	m_teamScore[team] += toAdd;
}

void Gamemode::setGametime(const float& seconds) {
	m_gametime = seconds;
}

void Gamemode::addGametime(const float& seconds) {
	m_gametime += seconds;
}