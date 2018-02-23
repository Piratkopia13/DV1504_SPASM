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
		size_t particles = 3;
		size_t bloom = 1;
		size_t AA = 0;
		size_t backGround = 0;
		bool fpsCounter = 1;
		bool vSync = 0;
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
		std::vector<Team> teams;
	};
	struct Setting {
		std::string name = "";
		float value = 0;
		float value2 = 0;
	};
	
	struct SoundSettings {
		float masterVolume = 0.5;
		float backGroundSoundVolume = 0.5;
		float effectSoundVolume = 0.5;
		bool wtf = false;
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

	std::vector<Setting> gameModes;
	std::vector<Setting> timeLimit;
	std::vector<Setting> scoreLimit;
	std::vector<Setting> respawnTime;
	std::vector<Setting> gravity;
	std::vector<Setting> playerHealth;

	std::vector<Setting> particles;
	std::vector<Setting> bloom;
	std::vector<Setting> antiAliasing;
	std::vector<Setting> background;
	std::vector<Setting> fpsCounter;
	std::vector<Setting> vSync;


	std::vector<Setting> masterVolume;
	std::vector<Setting> backgroundVolume;
	std::vector<Setting> effectVolume;
	std::vector<Setting> wtfVolume;




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