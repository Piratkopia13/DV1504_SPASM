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
	Character* character = nullptr;
	for (unsigned int i = 0; i < numOfPlayers; i++) {
		if (GameInfo::getInstance()->getScore().getPlayerStats(i).kills > m_maxKills)
			m_winnerID = i;
	}


}


void TeamDeathmatch::draw() {

}


int TeamDeathmatch::checkWin() {
	return m_winnerID;
}