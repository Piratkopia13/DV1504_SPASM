#include "TeamDeathmatch.h"
#include "../../CharacterHandler.h"
#include "../../GameInfo/Score.h"
#include "../../GameInfo.h"

TeamDeathmatch::TeamDeathmatch() {
	m_winnerID = Gamemode::NONE;
	m_maxKills = GameInfo::getInstance()->convertedGameSettings.scoreLimit;
}


TeamDeathmatch::~TeamDeathmatch() {

}


void TeamDeathmatch::update(CharacterHandler* charHandler, float delta) {
	Gamemode::update(nullptr, delta);

	unsigned int numOfPlayers = charHandler->getNrOfPlayers();
	float teamOne = 0;
	float teamTwo = 0;
	for (unsigned int i = 0; i < numOfPlayers; i++) {
		switch (charHandler->getCharacter(i)->getTeam()) {
		case 0:
			teamOne += float(GameInfo::getInstance()->getScore().getPlayerStats(i).kills);
			break;
		case 1:
			teamTwo += float(GameInfo::getInstance()->getScore().getPlayerStats(i).kills);
			break;
		default:
			Logger::Warning("The number of teams are too many");
			return;
		}
	}

	setScore(teamOne, 0);
	setScore(teamTwo, 1);

	if (teamOne >= m_maxKills)
		m_winnerID = 0;


	if (teamTwo >= m_maxKills)
		m_winnerID = 1;

}


void TeamDeathmatch::draw() {

}


int TeamDeathmatch::checkWin() {
	if (getGametime() <= 0.f) {
		float teamOne = getScore(0);
		float teamTwo = getScore(1);
		m_winnerID = (teamOne > teamTwo) ? 0 : (teamTwo > teamOne) ? 1 : Gamemode::DRAW;
	}

	return m_winnerID;
}