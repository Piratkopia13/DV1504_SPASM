#include "TeamDeathmatch.h"
#include "../../CharacterHandler.h"
#include "../../GameInfo/Score.h"
#include "../../GameInfo.h"

TeamDeathmatch::TeamDeathmatch() {
	m_winnerID = -1;
}


TeamDeathmatch::~TeamDeathmatch() {

}


void TeamDeathmatch::update(CharacterHandler* charHandler, float delta) {
	Gamemode::update(charHandler, delta);

	unsigned int numOfPlayers = charHandler->getNrOfPlayers();
	int teamOne = 0;
	int teamTwo = 0;
	Character* character = nullptr;
	for (unsigned int i = 0; i < numOfPlayers; i++) {
		switch (charHandler->getCharacter(i)->getTeam()) {
		case 0:
			teamOne = GameInfo::getInstance()->getScore().getPlayerStats(i).kills;
			break;
		case 1:
			teamTwo = GameInfo::getInstance()->getScore().getPlayerStats(i).kills;
			break;
		default:
			Logger::Warning("The number of teams are too many");
			return;
		}
	}


	if (teamOne >= m_maxKills)
		m_winnerID = 0;


	if (teamTwo >= m_maxKills)
		m_winnerID = 1;

}


void TeamDeathmatch::draw() {

}


int TeamDeathmatch::checkWin() {
	return m_winnerID;
}