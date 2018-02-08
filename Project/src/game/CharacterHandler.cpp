#include "CharacterHandler.h"

using namespace DirectX::SimpleMath;

CharacterHandler::CharacterHandler(ProjectileHandler* projHandler)
	: m_respawnTime(1)
{

	Application* app = Application::getInstance();
	Application::GameSettings* settings = &app->getGameSettings();

	Model* wModel = app->getResourceManager().getFBXModel("weapon").getModel();
	Model* hModel = app->getResourceManager().getFBXModel("projectile").getModel();
	Model* cModel1 = app->getResourceManager().getFBXModel("fisk").getModel();

	for (size_t i = 0; i < settings->players.size(); i++) {
		Weapon* tempWeapon = new Weapon(wModel, projHandler, settings->players[i].team);
		Hook* tempHook = new Hook(hModel);
		Character* tempChar = new Character(cModel1);
		tempChar->setHook(tempHook);
		tempChar->setWeapon(tempWeapon);
		tempChar->setLightColor(settings->players[i].color);
		tempChar->setTeam(settings->players[i].team);


		if (settings->players[i].port >= 0) {
			tempChar->setController(1);
			tempChar->setControllerPort(settings->players[i].port);
		}
		else
			tempChar->setController(0);


		addPlayer(tempChar);
	}

#ifdef _DEBUG
	if (settings->players.size() == 0) {
		Weapon* tempWeapon = new Weapon(wModel, projHandler, 1);
		Hook* tempHook = new Hook(hModel);
		Character* tempChar = new Character(cModel1);
		tempChar->setLightColor(Vector4(0.2f, 0.8f, 0.8f, 1.f));
		tempChar->setTeam(1);
		tempChar->setHook(tempHook);
		tempChar->setWeapon(tempWeapon);
		tempChar->setControllerPort(0);
		tempChar->setController(true);
		addPlayer(tempChar);
	}
	if (settings->players.size() < 4) {
		Weapon* tempWeapon = new Weapon(wModel, projHandler, 2);
		Hook* tempHook = new Hook(hModel);
		Character* tempChar = new Character(cModel1);
		tempChar->setLightColor(Vector4(0.8f, 0.2f, 0.8f, 1.f));
		tempChar->setTeam(2);
		tempChar->setHook(tempHook);
		tempChar->setWeapon(tempWeapon);
		tempChar->setControllerPort(1);
		tempChar->setController(true);
		addPlayer(tempChar);
	}
#endif

}


CharacterHandler::~CharacterHandler() {
	for (size_t i = 0; i < m_characters.size(); i++) {
		if (m_characters[i]) {
			delete m_characters[i];
		}
	}
}

void CharacterHandler::addPlayer(Character * player) {
	m_characters.push_back(player);
	m_respawnTimers.push_back(0);
}

void CharacterHandler::addSpawnPoint(unsigned int team, const DirectX::SimpleMath::Vector3& position) {
	m_spawns[team].push_back(position);
}

void CharacterHandler::killPlayer(unsigned int index) {
	if (index < m_characters.size()) {
		m_characters[index]->dead();
		m_characters[index]->setPosition(Vector3(0, 0, -100));
		m_respawnTimers[index] = 0.01f;
	}
}

void CharacterHandler::respawnPlayer(unsigned int id) {
	if (m_characters.size() > id) {
		m_respawnTimers[id] = 0;
		Vector3 respawnPos(0, 0, 0);
		unsigned int team = m_characters[id]->getTeam();
		if (m_spawns[team].size() > 0) {
			unsigned int spawn = unsigned int(Utils::rnd()*m_spawns[team].size());
			respawnPos = m_spawns[team][spawn];
		}
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
			if (m_characters[i]->getHealth() <= 0.0f) {
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