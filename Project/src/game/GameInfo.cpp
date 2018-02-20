#include "GameInfo.h"

using namespace DirectX::SimpleMath;

GameInfo::GameInfo() 
	:m_gameScore(0) 
{
	m_profiles.push_back(Profile("Guest 1", 0));
	m_profiles.push_back(Profile("Guest 2", 0));
	loadProfiles();

	
	float base = 1.0;
	float hue1 = 0.1;
	float hue2 = 0.2;

	//RED
	defaultColors.push_back(Vector4(base, 0.0f, 0.0f, 1.0f));
	defaultColors.push_back(Vector4(base - hue1, hue1, hue1, 1.0f));
	defaultColors.push_back(Vector4(base - hue2, hue2, hue2, 1.0f));

	//GREEN
	defaultColors.push_back(Vector4(0.0f, base, 0.0f, 1.0f));
	defaultColors.push_back(Vector4(hue1, base - hue1, hue1, 1.0f));
	defaultColors.push_back(Vector4(hue2, base - hue2, hue2, 1.0f));

	//BLUE
	defaultColors.push_back(Vector4(0.0f, 0.0f, base, 1.0f));
	defaultColors.push_back(Vector4(hue1, hue1, base - hue1, 1.0f));
	defaultColors.push_back(Vector4(hue2, hue2, base - hue2, 1.0f));

	////YELLOW
	//defaultColors.push_back(Vector4(base, base, 0.0f, 1.0f));
	//defaultColors.push_back(Vector4(base - hue1, base - hue1, hue1, 1.0f));
	//defaultColors.push_back(Vector4(base - hue2, base - hue2, hue2, 1.0f));
	//
	////PURPLE
	//defaultColors.push_back(Vector4(base, 0.0f, base, 1.0f));
	//defaultColors.push_back(Vector4(base - hue1, hue1, base - hue1, 1.0f));
	//defaultColors.push_back(Vector4(base - hue2, hue2, base - hue2, 1.0f));




}


GameInfo::~GameInfo() {
	saveProfiles();
}

void GameInfo::resetScore() {
	m_gameScore = Score(0);
}

std::vector<Profile>& GameInfo::getProfiles() {
	return m_profiles;
}

Score & GameInfo::getScore() {
	return m_gameScore;
}

void GameInfo::addPlayer(Player player) {
	m_currentPlayers.push_back(player);
	m_gameScore.addPlayer();
}

void GameInfo::addProfile(std::string name, size_t preOrdered) {
	m_profiles.push_back(Profile(name, preOrdered));
	saveProfiles();
}

void GameInfo::loadProfiles() {
	// read from file
}

void GameInfo::saveProfiles() {
	// write to file
}



