#pragma once
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
		size_t particles;
		size_t bloom;
		size_t AA;
		size_t backGround;
		bool fpsCounter;
		bool vSync;
	};


	struct GameSettings {
		struct Team {
			size_t color;
			size_t preOrder;
		};
		size_t map;
		size_t gameMode;
		size_t scoreLimit;
		size_t timelimit;
		size_t respawnTime;
		size_t playerLife;
		size_t gravity;
		std::vector<Team> teams;
	};
	struct Setting {
		std::string name;
		float value;
		float value2;
	};
	
	struct SoundSettings {
		float masterVolume;
		float backGroundSoundVolume;
		float effectSoundVolume;
		bool wtf;
	};


	GameSettings gameSettings;
	GraphicsSettings graphicsSettings;
	SoundSettings soundSettings;

	std::vector<DirectX::SimpleMath::Vector4> defaultColors;
	std::vector<std::string> colorNames;
	std::vector<std::string> colorHues;
	std::vector<std::string> botHeadNames;
	std::vector<std::string> botBodyNames;
	std::vector<std::string> botLegNames;
	std::vector<std::string> botArmNames;

	//std::vector<Setting> gameModes;




private:
	static GameInfo* m_infoInstance;

	Score m_gameScore;
	std::vector<Profile> m_profiles;
	std::vector<Player> m_currentPlayers;

	void loadProfiles();
	void saveProfiles();

	void loadGraphics();
	void saveGraphics();

	void loadSound();
	void saveSound();

};