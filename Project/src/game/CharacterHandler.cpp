#include "CharacterHandler.h"

using namespace DirectX::SimpleMath;

CharacterHandler::CharacterHandler(ParticleHandler* particleHandler, ProjectileHandler* projHandler)
	: m_respawnTime(1)
	, m_particleHandler(particleHandler)
{

	Application* app = Application::getInstance();
	Application::GameSettings* settings = &app->getGameSettings();

	Model* wModel = app->getResourceManager().getFBXModel("fisk/fisk_armR").getModel();
	Model* lModel = app->getResourceManager().getFBXModel("laser").getModel();
	Model* dModel = app->getResourceManager().getFBXModel("sphere").getModel();
	Model* hModel = app->getResourceManager().getFBXModel("projectile").getModel();
	Model* cModel1 = app->getResourceManager().getFBXModel("fisk/fisk_body").getModel();
	Model* cModel2 = app->getResourceManager().getFBXModel("fisk/fisk_armL").getModel();
	Model* cModel3 = app->getResourceManager().getFBXModel("fisk/fisk_head").getModel();

	for (size_t i = 0; i < settings->players.size(); i++) {
		Hook* tempHook = new Hook(hModel);
		Character* tempChar = new Character(cModel1, cModel2, cModel3);
		Weapon* tempWeapon = new Weapon(wModel, lModel, dModel, projHandler, tempChar);
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
		Hook* tempHook = new Hook(hModel);
		Character* tempChar = new Character(cModel1, cModel2, cModel3);
		Weapon* tempWeapon = new Weapon(wModel, lModel, dModel, projHandler, tempChar);
		tempChar->setWeapon(tempWeapon);
		tempChar->setLightColor(settings->teamOneColor);
		tempChar->setTeam(1);
		tempChar->setHook(tempHook);
		tempChar->setControllerPort(0);
		tempChar->setController(true);
		addPlayer(tempChar);
	}
	if (settings->players.size() < 4) {
		Hook* tempHook = new Hook(hModel);
		Character* tempChar = new Character(cModel1, cModel2, cModel3);
		Weapon* tempWeapon = new Weapon(wModel, lModel, dModel, projHandler, tempChar);
		tempChar->setWeapon(tempWeapon);
		tempChar->setLightColor(settings->teamTwoColor);
		tempChar->setTeam(2);
		tempChar->setHook(tempHook);
		tempChar->setControllerPort(1);
		tempChar->setController(false);
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

void CharacterHandler::addPlayer(Character* player) {
	m_characters.push_back(player);
	m_respawnTimers.push_back(0);
	m_particleHandler->addEmitter(player->m_thrusterEmitter);
}

void CharacterHandler::addSpawnPoint(unsigned int team, const DirectX::SimpleMath::Vector3& position) {
	m_spawns[team].push_back(position);
}

void CharacterHandler::killPlayer(unsigned int index) {
	if (index < m_characters.size()) {
		m_characters[index]->dead();
		m_characters[index]->setPosition(Vector3(0, 0, -100));
		m_respawnTimers[index] = 0.01f;
		int rnd = static_cast<int>(floor(Utils::rnd() * 2.f));
		float fRnd = Utils::rnd() * 0.4f + 0.8f;
		switch (rnd) {
		case 0:
			Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Explosion2, 3.f, fRnd);
			break;
		default:
			Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Explosion, 3.f, fRnd);
			break;
		}
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
		m_characters[id]->setPosition(respawnPos + Vector3(0.0f, 1.0f, 0.0f));
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

void CharacterHandler::processInput() {
	for (auto* character : m_characters)
		character->processInput();
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
