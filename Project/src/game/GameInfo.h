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
	struct GameSettings;
	struct GraphicsSettings;
	struct SoundSettings;

	GameInfo();
	~GameInfo();

	void resetScore();


	std::vector<Profile>& getProfiles();
	Score& getScore();


	void addPlayer(Player player);
	void addProfile(std::string name, size_t preOrdered);

	struct Player {
		Profile* currentProfile;
		size_t team;
		size_t color;
		size_t headModel;
		size_t bodyModel;
		size_t legs;
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


private:


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