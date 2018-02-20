#include "CharacterHandler.h"

using namespace DirectX::SimpleMath;

CharacterHandler::CharacterHandler(ParticleHandler* particleHandler, ProjectileHandler* projHandler)
	: m_respawnTime(1)
	, m_particleHandler(particleHandler)
{

	Application* app = Application::getInstance();
	Application::GameSettings* settings = &app->getGameSettings();


	std::vector<Model*> bodies;
	bodies.push_back(app->getResourceManager().getFBXModel("fisk/fisk_body").getModel());
	bodies.push_back(app->getResourceManager().getFBXModel("fisk/trashbot_body").getModel());
	bodies.push_back(app->getResourceManager().getFBXModel("fisk/unibot_body").getModel());

	Model* laserModel = app->getResourceManager().getFBXModel("laser").getModel();
	Model* projectileModel = app->getResourceManager().getFBXModel("sphere").getModel();
	Model* hookModel = app->getResourceManager().getFBXModel("projectile").getModel();
	Model* armLeftModel = app->getResourceManager().getFBXModel("fisk/fisk_armL").getModel();
	Model* headModel = app->getResourceManager().getFBXModel("fisk/fisk_head").getModel();
	Model* armRightModel = app->getResourceManager().getFBXModel("fisk/fisk_armR").getModel();

	for (size_t i = 0; i < settings->players.size(); i++) {
		Weapon* tempWeapon = new Weapon(armRightModel, laserModel, projectileModel, projHandler, settings->players[i].team);
		Hook* tempHook = new Hook(hookModel);
		Character* tempChar = new Character(bodies[settings->players[i].model], armLeftModel, headModel);
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
	//if (settings->players.size() == 0) {
	//	Weapon* tempWeapon = new Weapon(armRightModel, laserModel, projectileModel, projHandler, 1);
	//	Hook* tempHook = new Hook(hookModel);
	//	Character* tempChar = new Character(bodies[0], armLeftModel, headModel);
	//	tempChar->setLightColor(settings->players[0].color);
	//	tempChar->setTeam(1);
	//	tempChar->setHook(tempHook);
	//	tempChar->setWeapon(tempWeapon);
	//	tempChar->setControllerPort(0);
	//	tempChar->setController(true);
	//	addPlayer(tempChar);
	//}
	//if (settings->players.size() < 4) {
	//	Weapon* tempWeapon = new Weapon(armRightModel, laserModel, projectileModel, projHandler, 2);
	//	Hook* tempHook = new Hook(hookModel);
	//	Character* tempChar = new Character(bodies[0], armLeftModel, headModel);
	//	tempChar->setLightColor(settings->players[0].color);
	//	tempChar->setTeam(2);
	//	tempChar->setHook(tempHook);
	//	tempChar->setWeapon(tempWeapon);
	//	tempChar->setControllerPort(1);
	//	tempChar->setController(false);
	//	addPlayer(tempChar);
	//}
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
		int rnd = static_cast<int>(floor(Utils::rnd() * 10.f));
		switch (rnd) {
		case 0:
			Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Male_Death);
			break;
		case 1:
			Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Goblin_Death);
			break;
		default:
			Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Explosion);
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
