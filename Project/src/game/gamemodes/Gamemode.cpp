#include "Gamemode.h"
#include "../GameInfo.h"

Gamemode::Gamemode() {
	m_teamScore.resize(GameInfo::getInstance()->getPlayers().size());
	for (unsigned int i = 0; i < m_teamScore.size(); i++) {
		m_teamScore[i] = 0;
	}

	m_gametime = GameInfo::getInstance()->convertedGameSettings.timeLimit;
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

const float & Gamemode::getScore(const unsigned int team) {
	if (team < 0 || team > m_teamScore.size()) {
		Logger::Warning("Tried to fetch an out of bounds teamscore");
		return 0.f;
	}

	return m_teamScore[team];
}

float Gamemode::getGametime() {
	return m_gametime;
}

void Gamemode::addScore(const float toAdd, const unsigned int team) {
	if (team < 0 || team > m_teamScore.size())
		return;

	m_teamScore[team] += toAdd;
}  

void Gamemode::setScore(const float score, const unsigned int team) {
	if (team < 0 || team > m_teamScore.size())
		return;

	m_teamScore[team] = score;
}

void Gamemode::setGametime(const float& seconds) {
	m_gametime = seconds;
}

void Gamemode::addGametime(const float& seconds) {
	m_gametime += seconds;
}