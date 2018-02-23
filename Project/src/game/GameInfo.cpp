#include "GameInfo.h"

using namespace DirectX::SimpleMath;

GameInfo* GameInfo::m_infoInstance = nullptr;

GameInfo::GameInfo() 
	:m_gameScore(0) 
{

	m_profiles.push_back(Profile("Guest 1", 0));
	m_profiles.push_back(Profile("Guest 2", 0));
	loadProfiles();
	
	float base = 1.0f;
	float hue1 = 0.2f;
	float hue2 = 0.4f;

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

	//YELLOW
	defaultColors.push_back(Vector4(base, base, 0.0f, 1.0f));
	defaultColors.push_back(Vector4(base - hue1, base - hue1, hue1, 1.0f));
	defaultColors.push_back(Vector4(base - hue2, base - hue2, hue2, 1.0f));
	
	//PURPLE
	defaultColors.push_back(Vector4(base, 0.0f, base, 1.0f));
	defaultColors.push_back(Vector4(base - hue1, hue1, base - hue1, 1.0f));
	defaultColors.push_back(Vector4(base - hue2, hue2, base - hue2, 1.0f));

	colorNames.push_back("red");
	colorNames.push_back("greeen");
	colorNames.push_back("blue");
	colorNames.push_back("yellow");
	colorNames.push_back("purple");

	colorHues.push_back("dark");
	colorHues.push_back("normal");
	colorHues.push_back("light");


	// PLAYER MODEL PARTS

	botHeadNames.push_back("fisk");
	botHeadNames.push_back("hooded");
	botHeadNames.push_back("bulb");

	botBodyNames.push_back("fisk");
	botBodyNames.push_back("stick");
	botBodyNames.push_back("trash");

	
	botLegNames.push_back("fisk");
	botLegNames.push_back("uni");
	botLegNames.push_back("trash");

	botArmNames.push_back("fisk");


	// GAME SETTINGS

	gameModes.push_back({ "domination", 0, 0});
	gameModes.push_back({ "deathMatch", 0, 0 });
	gameModes.push_back({ "team Deathmatch", 0, 0 });

	timeLimit.push_back({ "no Limit", 0.0f, 0 });
	timeLimit.push_back({ "1 min", 60.0f, 0 });
	timeLimit.push_back({ "2 min", 120.0f, 0 });
	timeLimit.push_back({ "3 min", 180.0f, 0 });
	timeLimit.push_back({ "5 min", 300.0f, 0 });
	timeLimit.push_back({ "10 min", 600.0f, 0 });

	scoreLimit.push_back({ "50 points", 50.0f, 0 });
	scoreLimit.push_back({ "100 points", 100.0f, 0 });
	scoreLimit.push_back({ "150 points", 150.0f, 0 });
	scoreLimit.push_back({ "250 points", 250.0f, 0 });
	scoreLimit.push_back({ "no limit", 0, 0 });

	respawnTime.push_back({ "2 seconds", 2.0f, 0});
	respawnTime.push_back({ "4 seconds", 4.0f, 0 });
	respawnTime.push_back({ "8 seconds", 8.0f, 0 });
	respawnTime.push_back({ "instant", 2.0f, 0 });

	gravity.push_back({ "earth",1,0 });
	gravity.push_back({ "moon",0.16f,0 });
	gravity.push_back({ "mars",0.38f,0 });
	gravity.push_back({ "none",0,0 });

	playerHealth.push_back({"100",0,0});
	playerHealth.push_back({ "200",0,0 });
	playerHealth.push_back({ "10",0,0 });
	playerHealth.push_back({ "50",0,0 });
	
	// GRAPHICS SETTINGS




	// SOUND SETTINGS
	






	if (m_infoInstance) {
		Logger::Warning("wtf you doing");
		return;
	}
	m_infoInstance = this;
}


GameInfo::~GameInfo() {
	saveProfiles();
}

GameInfo * GameInfo::getInstance()
{
	return m_infoInstance;
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

std::vector<GameInfo::Player>& GameInfo::getPlayers()
{
	return m_currentPlayers;
}

const DirectX::SimpleMath::Vector4 & GameInfo::getDefaultColor(size_t color, size_t hue) {
	return defaultColors[color*colorHues.size()+hue];
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



