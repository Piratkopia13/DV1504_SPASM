#pragma once
#include <iostream>
#include <fstream>
#include "GameInfo/Profile.h"
#include "GameInfo/Score.h"
#include "../sail/Sail.h"
#include "SimpleMath.h"

class GameInfo
{
public:
	struct Player;
	struct Team;
	struct Setting;
	struct GameSettings;
	struct GraphicsSettings;
	struct SoundSettings;

	GameInfo();
	~GameInfo();
	static GameInfo* getInstance();


	void resetScore();


	std::vector<Profile>& getProfiles();
	Score& getScore();
	std::vector<Player>& getPlayers();
	const DirectX::SimpleMath::Vector4& getDefaultColor(size_t color, size_t hue);
	//profile port team color hue
	void addPlayer(Player player); 
	void addProfile(std::string name, size_t preOrdered);

	struct Player {
		Profile* currentProfile;
		size_t port;
		size_t team;
		size_t color;
		size_t hue;
		size_t headModel;
		size_t bodyModel;
		size_t legModel;
		size_t armModel;

	};

	struct GraphicsSettings {
		size_t particles = 0;
		size_t bloom = 1;
		size_t AA = 0;
		size_t backGround = 0;
		size_t fpsCounter = 1;
		bool vSync = 0;
		bool depthOfField = false;
	};
	struct ConvertedGraphics {
		float particles = 3;
		float bloom = 1;
		float AA = 1;
		float background = 1;
		float fpsCounter = 1;
		float vSync = 0;
		float depthOfField = 0;
	};

	struct GameSettings {
		struct Team {
			size_t color;
			size_t preOrder;
		};
		size_t map = 0;
		size_t gameMode = 0;
		size_t scoreLimit = 0;
		size_t timelimit = 0;
		size_t respawnTime = 0;
		size_t playerLife = 0;
		size_t gravity = 0;
		size_t destructibleBlocks = 1;
		std::vector<Team> teams;
	};
	struct ConvertedGameSettings {
		struct Team {
			DirectX::SimpleMath::Vector4 color;
			size_t preOrder;
			bool winner;
		};
		std::string map = "Domination/carrot.level";
		size_t gamemode = 0;
		float scoreLimit = 50;
		float timeLimit = 0;
		float respawnTime = 2;
		float playerLife = 50;
		float gravity = 1;
		bool destructability = 1;
		std::vector<Team> teams;
	};


	struct Setting {
		std::string name = "";
		float value = 0;
		float value2 = 0;
	};
	
	struct SoundSettings {
		size_t master = 0;
		size_t background = 0;
		size_t effect = 0;
	};
	struct ConvertedSoundSettings {
		float masterVolume = 0.5;
		float backGroundSoundVolume = 0.5;
		float effectSoundVolume = 0.5;
	};

	enum Modes {
		DOMINATION,
		DEATHMATCH,
		TEAMDEATHMATCH
	};

	GameSettings gameSettings;
	ConvertedGameSettings convertedGameSettings;
	GraphicsSettings graphicsSettings;
	ConvertedGraphics convertedGraphics;
	SoundSettings soundSettings;
	ConvertedSoundSettings convertedSound;

	std::vector<DirectX::SimpleMath::Vector4> defaultColors;
	std::vector<std::string> colorNames;
	std::vector<std::string> colorHues;
	std::vector<std::string> botHeadNames;
	std::vector<std::string> botBodyNames;
	std::vector<std::string> botLegNames;
	std::vector<std::string> botArmNames;

	std::vector<std::vector<std::string>> maps;
	std::vector<Setting> gameModes;
	std::vector<Setting> timeLimit;
	std::vector<Setting> scoreLimit;
	std::vector<Setting> scoreLimitDM;
	std::vector<Setting> scoreLimitTDM;
	std::vector<Setting> respawnTime;
	std::vector<Setting> gravity;
	std::vector<Setting> playerHealth;
	std::vector<Setting> destructibleBlocks;

	std::vector<Setting> particles;
	std::vector<Setting> bloom;
	std::vector<Setting> antiAliasing;
	std::vector<Setting> background;
	std::vector<Setting> fpsCounter;
	std::vector<Setting> frameRateLock;
	std::vector<Setting> vSync;
	std::vector<Setting> depthOfField;


	std::vector<Setting> masterVolume;
	std::vector<Setting> backgroundVolume;
	std::vector<Setting> effectVolume;

	void convertGameSettings();
	void convertGraphics();
	void convertSound();

	void saveGraphics();
	void saveSound();

private:
	static GameInfo* m_infoInstance;

	Score m_gameScore;
	std::vector<Profile> m_profiles;
	std::vector<Player> m_currentPlayers;





	void loadProfiles();
	void saveProfiles();

	void loadGraphics();
	void loadSound();

};