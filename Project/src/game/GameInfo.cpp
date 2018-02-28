#include "GameInfo.h"

#include <string>
#include <iostream>
#include <filesystem>

using namespace DirectX::SimpleMath;
using namespace std::experimental::filesystem;

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
	gameModes.push_back({ "deathMatch", 1, 0 });
	gameModes.push_back({ "team Deathmatch", 2, 0 });

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

	playerHealth.push_back({"50",50,0});
	playerHealth.push_back({ "100",100,0 });
	playerHealth.push_back({ "10",10,0 });
	playerHealth.push_back({ "30",30,0 });
	
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


	// MAPS

	std::vector<std::string> domination;
	std::vector<std::string> deathmatch;
	std::vector<std::string> teamdeathmatch;

	std::string s;
	std::stringstream ss;
	std::string path = "res/levels/Domination";
	for (auto & p : directory_iterator(path)) {
		ss << p;
		s = ss.str(); 
		int first = s.find_last_of('\\') + 1;
		int last = s.find_last_of('.');
		if (s.substr(last,6) == ".level") {
			std::string item = s.substr(first, last - first);
			Logger::log("map " + item + " Loaded for domination");
			domination.push_back(item);	
		}
		ss.str("");
	}
	
	path = "res/levels/DM";
	for (auto & p : directory_iterator(path)) {
		ss << p;
		s = ss.str();
		int first = s.find_last_of('\\') + 1;
		int last = s.find_last_of('.');

		std::string item = s.substr(first, last-first);
		deathmatch.push_back(item);
		Logger::log("map " + item + " Loaded for DM");
		ss.str("");
	}
	path = "res/levels/teamDM";
	for (auto & p : directory_iterator(path)) {
		ss << p;
		s = ss.str();
		int first = s.find_last_of('\\') + 1;
		int last = s.find_last_of('.');

		std::string item = s.substr(first, last - first);
		Logger::log("map " + item + " Loaded for TDM");
		teamdeathmatch.push_back(item);
		ss.str("");
	}
	

	maps.push_back(domination);
	maps.push_back(deathmatch);
	maps.push_back(teamdeathmatch);


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

void GameInfo::convertGameSettings() {
	convertedGameSettings = ConvertedGameSettings();
	std::string preText = "";
	if (gameSettings.gameMode == DOMINATION)
		preText = "domination/";
	if (gameSettings.gameMode == DEATHMATCH)
		preText = "DM/";
	if (gameSettings.gameMode == TEAMDEATHMATCH)
		preText = "TDM/";
	convertedGameSettings.map = preText + maps[gameSettings.gameMode][gameSettings.map]+".level";
	convertedGameSettings.gamemode = gameModes[gameSettings.gameMode].value;
	convertedGameSettings.scoreLimit = scoreLimit[gameSettings.scoreLimit].value;
	convertedGameSettings.timeLimit = timeLimit[gameSettings.timelimit].value;
	convertedGameSettings.respawnTime = respawnTime[gameSettings.respawnTime].value;
	convertedGameSettings.playerLife = playerHealth[gameSettings.playerLife].value;
	convertedGameSettings.gravity = gravity[gameSettings.gravity].value;

	for (size_t i = 0; i < gameSettings.teams.size(); i++) {
		convertedGameSettings.teams.push_back({
			getDefaultColor(gameSettings.teams[i].color,0),
			gameSettings.teams[i].preOrder
			});
	}

}

void GameInfo::convertGraphics() {
	convertedGraphics = ConvertedGraphics();
	convertedGraphics.particles = particles[graphicsSettings.particles].value;
	convertedGraphics.bloom = bloom[graphicsSettings.bloom].value;
	convertedGraphics.AA = antiAliasing[graphicsSettings.AA].value;
	convertedGraphics.background = background[graphicsSettings.backGround].value;
	convertedGraphics.particles = particles[graphicsSettings.particles].value;
	convertedGraphics.fpsCounter = fpsCounter[graphicsSettings.fpsCounter].value;
	convertedGraphics.vSync = fpsCounter[graphicsSettings.fpsCounter].value;
	convertedGraphics.wtf = wtfGraphics[graphicsSettings.wtf].value;
}

void GameInfo::saveGraphics() {

}

void GameInfo::saveSound() {

}

void GameInfo::loadProfiles() {
	// read from file
	std::string filePath = "res/data/data.profiles";
	std::ifstream profilesFile(filePath);
	std::string line;
	if (profilesFile.is_open()) {
		while (getline(profilesFile, line)) {
			std::istringstream l(line);
			std::string tempString;
			std::vector<std::string> tempList;
			while (getline(l, tempString, ':')) {
				tempList.push_back(tempString);
			}
			m_profiles.push_back(Profile(tempList[0], std::stoul(tempList[1]), {std::stoul(tempList[2]), std::stoul(tempList[3]), std::stoul(tempList[4]), std::stoul(tempList[5])}));
		}
	}
}

void GameInfo::saveProfiles() {
	// write to file
	std::string filePath = "res/data/data.profiles";
	std::ofstream profilesFile(filePath);
	if (profilesFile.is_open()) {
		for (unsigned int i = 2; i < m_profiles.size(); i++) {
			profilesFile << m_profiles[i].getAsString() << "\n";
		}
		profilesFile.close();
	}
	else {
		Logger::Error("Could not open profiles file - no profiles loaded");
	}
}

void GameInfo::loadGraphics()
{
}

void GameInfo::loadSound()
{
}



