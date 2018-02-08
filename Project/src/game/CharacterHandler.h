#pragma once
#include "objects/Character.h"

class CharacterHandler
{
public:
	CharacterHandler(ProjectileHandler* projHandler = nullptr);
	~CharacterHandler();

	void addSpawnPoint(unsigned int team, const DirectX::SimpleMath::Vector3& position);
	void killPlayer(unsigned int index);
	void respawnPlayer(unsigned int id);
	void setRespawnTime(float time);

	unsigned int getNrOfPlayers();
	Character* getCharacter(unsigned int id);
	bool useableTarget(unsigned int index);

	void update(float dt);
private:
	void addPlayer(Character* player);

	float m_respawnTime;

	std::vector<Character*> m_characters;
	std::vector<float> m_respawnTimers;
	std::vector<DirectX::SimpleMath::Vector3> m_spawns[4];



};

