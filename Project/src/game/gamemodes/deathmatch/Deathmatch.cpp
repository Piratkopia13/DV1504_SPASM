#include "Deathmatch.h"
#include "../../CharacterHandler.h"
#include "../../GameInfo/Score.h"
#include "../../GameInfo.h"

Deathmatch::Deathmatch() {
	m_winnerID = -1;
	m_maxKills = GameInfo::getInstance()->convertedGameSettings.scoreLimit;
}


Deathmatch::~Deathmatch() {

}


void Deathmatch::update(CharacterHandler* charHandler, float delta) {
	Gamemode::update(charHandler, delta);
	
	unsigned int numOfPlayers = charHandler->getNrOfPlayers();
	Character* character = nullptr;
	float numOfKills = 0;
	for (unsigned int i = 0; i < numOfPlayers; i++) {
		numOfKills = float(GameInfo::getInstance()->getScore().getPlayerStats(i).kills);
		setScore(numOfKills, i);
		if (numOfKills >= m_maxKills)
			m_winnerID = i;
	}


}


void Deathmatch::draw() {

}


int Deathmatch::checkWin() {
	return m_winnerID;
}