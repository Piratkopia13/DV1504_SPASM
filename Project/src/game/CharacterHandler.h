#pragma once
#include "objects\Character.h"

class CharacterHandler
{
public:
	CharacterHandler();
	~CharacterHandler();

	void addPlayer(Character* player);
	void addSpawnPoint(unsigned int team, DirectX::SimpleMath::Vector3 position);
	void killPlayer(unsigned int index);
	void respawnPlayer(unsigned int id);
	void setRespawnTime(float time);

	unsigned int getNrOfPlayers();
	Character* getCharacter(unsigned int id);
	bool useableTarget(unsigned int index);

	void update(float dt);
private:

	float m_respawnTime;

	std::vector<Character*> m_characters;
	std::vector<float> m_respawnTimers;
	std::vector<DirectX::SimpleMath::Vector3> m_spawns[4];



};

