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

	particles.push_back({ "standard",3,0 });
	particles.push_back({ "plenty",6,0 });
	particles.push_back({ "too much",10,0 });
	particles.push_back({ "none",0,0 });

	bloom.push_back({ "standard",1,0 });
	bloom.push_back({ "high",2,0 });
	bloom.push_back({ "none",0,0 });

	antiAliasing.push_back({ "FXAA",1,0 });
	antiAliasing.push_back({ "none",0,0 });

	background.push_back({ "standard",1,0 });
	background.push_back({ "something",2,0 });
	background.push_back({ "none",0,0 });

	fpsCounter.push_back({ "top left",1,0 });
	fpsCounter.push_back({ "none",0,0 });

	frameRateLock.push_back({ "none", 0, 0 });
	frameRateLock.push_back({ "60", 1.0f / 60.0f, 0 });
	frameRateLock.push_back({ "120", 1.0f / 120.0f, 0 });
	frameRateLock.push_back({ "144", 1.0f / 144.0f, 0 });

	vSync.push_back({ "none",0,0 });
	vSync.push_back({ "vsync",1,0 });


	wtfGraphics.push_back({ "no", 0 ,0 });
	wtfGraphics.push_back({ "pogchamp", 0 ,0 });

	// SOUND SETTINGS
	
	masterVolume.push_back({ "50",0.5f,0 });
	masterVolume.push_back({ "60",0.6f,0 });
	masterVolume.push_back({ "70",0.7f,0 });
	masterVolume.push_back({ "80",0.8f,0 });
	masterVolume.push_back({ "90",0.9f,0 });
	masterVolume.push_back({ "100",1.0f,0 });
	masterVolume.push_back({ "0",0.0f,0 });
	masterVolume.push_back({ "10",0.1f,0 });
	masterVolume.push_back({ "20",0.2f,0 });
	masterVolume.push_back({ "30",0.3f,0 });
	masterVolume.push_back({ "40",0.4f,0 });

	backgroundVolume.push_back({ "50",0.5f,0 });
	backgroundVolume.push_back({ "60",0.6f,0 });
	backgroundVolume.push_back({ "70",0.7f,0 });
	backgroundVolume.push_back({ "80",0.8f,0 });
	backgroundVolume.push_back({ "90",0.9f,0 });
	backgroundVolume.push_back({ "100",1.0f,0 });
	backgroundVolume.push_back({ "0",0.0f,0 });
	backgroundVolume.push_back({ "10",0.1f,0 });
	backgroundVolume.push_back({ "20",0.2f,0 });
	backgroundVolume.push_back({ "30",0.3f,0 });
	backgroundVolume.push_back({ "40",0.4f,0 });

	effectVolume.push_back({ "50",0.5f,0 });
	effectVolume.push_back({ "60",0.6f,0 });
	effectVolume.push_back({ "70",0.7f,0 });
	effectVolume.push_back({ "80",0.8f,0 });
	effectVolume.push_back({ "90",0.9f,0 });
	effectVolume.push_back({ "100",1.0f,0 });
	effectVolume.push_back({ "0",0.0f,0 });
	effectVolume.push_back({ "10",0.1f,0 });
	effectVolume.push_back({ "20",0.2f,0 });
	effectVolume.push_back({ "30",0.3f,0 });
	effectVolume.push_back({ "40",0.4f,0 });

	wtfVolume.push_back({ "no", 0, 0 });
	wtfVolume.push_back({ "wtf", 0, 0 });

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

void GameInfo::convertGraphics() {



}

void GameInfo::saveGraphics() {

}

void GameInfo::saveSound() {

}

void GameInfo::loadProfiles() {
	// read from file
}

void GameInfo::saveProfiles() {
	// write to file
}

void GameInfo::loadGraphics()
{
}

void GameInfo::loadSound()
{
}



