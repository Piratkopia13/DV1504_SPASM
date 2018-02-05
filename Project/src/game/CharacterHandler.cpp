#include "CharacterHandler.h"

using namespace DirectX::SimpleMath;

CharacterHandler::CharacterHandler(): m_respawnTime(1){

}


CharacterHandler::~CharacterHandler() {
	for (size_t i = 0; i < m_characters.size(); i++) {
		if (m_characters[i]) {
			delete m_characters[i];
			m_characters.pop_back();
		}
	}
}

void CharacterHandler::addPlayer(Character * player) {
	m_characters.push_back(player);
	m_respawnTimers.push_back(0);
}

void CharacterHandler::addSpawnPoint(unsigned int team, DirectX::SimpleMath::Vector3 position) {
	m_spawns[team].push_back(position);
}

void CharacterHandler::killPlayer(unsigned int index) {
	if (index < m_characters.size()) {
		m_characters[index]->dead();
		m_characters[index]->setPosition(Vector3(0, 0, -100));
		m_respawnTimers[index] = 0.01;
	}


}

void CharacterHandler::respawnPlayer(unsigned int id) {
	if (m_characters.size() > id) {
		m_respawnTimers[id] = 0;
		Vector3 respawnPos(0, 0, 0);
		unsigned int team = m_characters[id]->getTeam();
		unsigned int spawn = unsigned int(Utils::rnd()*m_spawns[team].size());
		respawnPos = m_spawns[team][spawn];
		m_characters[id]->setPosition(respawnPos);
		m_characters[id]->living();
	}
}

void CharacterHandler::setRespawnTime(float time)
{
	m_respawnTime = time < 0 ? 0 : time;
}

void CharacterHandler::update(float dt) {
	
	for (size_t i = 0; i < m_characters.size(); i++) {
		if (m_respawnTimers[i] > 0) {
			if (m_respawnTimers[i] >= m_respawnTime) {
				respawnPlayer(i);
				continue;
			}
			m_respawnTimers[i] += dt;
		}
		else {
			m_characters[i]->update(dt);
			if (m_characters[i]->getHealth() <= 0) {
				killPlayer(i);
			}
		}
	}
}

unsigned int CharacterHandler::getNrOfPlayers()
{
	return m_characters.size();
}

Character* CharacterHandler::getCharacter(unsigned int id) {
	if (id < m_characters.size()) {
		return m_characters[id];
	}
	else 
		return nullptr;
}

bool CharacterHandler::useableTarget(unsigned int index)
{
	if (index < m_characters.size()) {
		if (m_characters[index]) {
			if (m_characters[index]->isAlive())
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}
