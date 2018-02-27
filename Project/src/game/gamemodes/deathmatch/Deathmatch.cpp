#include "Deathmatch.h"
#include "../../CharacterHandler.h"
#include "../../GameInfo/Score.h"
#include "../../GameInfo.h"

Deathmatch::Deathmatch() {
	m_winnerID = -1;
}


Deathmatch::~Deathmatch() {

}


void Deathmatch::update(CharacterHandler* charHandler, float delta) {
	Gamemode::update(charHandler, delta);
	
	unsigned int numOfPlayers = charHandler->getNrOfPlayers();
	Character* character = nullptr;
	for (unsigned int i = 0; i < numOfPlayers; i++) {
		if (GameInfo::getInstance()->getScore().getPlayerStats(i).kills > m_maxKills)
			m_winnerID = i;
	}


}


void Deathmatch::draw() {

}


int Deathmatch::checkWin() {
	return m_winnerID;
}