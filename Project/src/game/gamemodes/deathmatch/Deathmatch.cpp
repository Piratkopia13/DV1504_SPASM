#include "Deathmatch.h"
#include "../../CharacterHandler.h"


Deathmatch::Deathmatch() {

}


Deathmatch::~Deathmatch() {

}


void Deathmatch::update(CharacterHandler* charHandler, float delta) {
	Gamemode::update(charHandler, delta);
	
	unsigned int numOfPlayers = charHandler->getNrOfPlayers();
	for (unsigned int i = 0; i < numOfPlayers; i++) {
		Character* character = charHandler->getCharacter(i);
	}


}


void Deathmatch::draw() {

}


int Deathmatch::checkWin() {
	// Get kills
	// if kills exceeds x
	// win

	return 0;
}