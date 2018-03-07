#include "Deathmatch.h"
#include "../../CharacterHandler.h"
#include "../../GameInfo/Score.h"
#include "../../GameInfo.h"

Deathmatch::Deathmatch() {
	m_winnerID = Gamemode::NONE;
	m_maxKills = GameInfo::getInstance()->convertedGameSettings.scoreLimit;
}


Deathmatch::~Deathmatch() {

}


void Deathmatch::update(CharacterHandler* charHandler, float delta) {
	Gamemode::update(nullptr, delta);
	
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

	if (getGametime() <= 0.f) {
		std::vector<float> playerScore = getScore();
		
		float score = 0;
		for (unsigned int i = 0; i < playerScore.size(); i++) {
			if (playerScore[i] > score) {
				score = playerScore[i];
				m_winnerID = static_cast<int>(i);
			}
		}
	}

	return m_winnerID;
}