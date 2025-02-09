#pragma once
#include "objects/Character.h"
#include "GameInfo.h"

class PlayerSpawn;
class CharacterHandler {
public:
	CharacterHandler(ParticleHandler* particleHandler, ProjectileHandler* projHandler = nullptr);
	~CharacterHandler();

	void addSpawnPoint(unsigned int team, const DirectX::SimpleMath::Vector3& position);
	void killPlayer(unsigned int index);
	void respawnPlayer(unsigned int id);
	void setRespawnTime(float time);

	unsigned int getNrOfPlayers();
	Character* getCharacter(unsigned int id);
	bool useableTarget(unsigned int index);

	void update(float dt);
	void processInput();
	size_t getNumberOfSpawns();
	PlayerSpawn * getSpawn(size_t index);


private:

	void addPlayer(Character* player);
	DirectX::SimpleMath::Vector3 getRandomSpawnPoint(UINT team) const;

	ParticleHandler* m_particleHandler;
	std::vector<Character*> m_characters;
	std::vector<float> m_respawnTimers;
	std::vector<DirectX::SimpleMath::Vector3> m_spawns[4];
	std::unique_ptr<Model> m_laserModel;
	std::vector<std::unique_ptr<PlayerSpawn>> m_drawSpawns;

	GameInfo* m_info;

};


class PlayerSpawn : public Object {

public:
	PlayerSpawn(const DirectX::SimpleMath::Vector3 & position, const DirectX::SimpleMath::Vector4 & color);
	~PlayerSpawn();

	void draw();

private:


};