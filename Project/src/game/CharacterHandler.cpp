#include "CharacterHandler.h"

using namespace DirectX::SimpleMath;

CharacterHandler::CharacterHandler(ParticleHandler* particleHandler, ProjectileHandler* projHandler)
	: m_respawnTime(1)
	, m_particleHandler(particleHandler)
{

	Application* app = Application::getInstance();
	m_info = GameInfo::getInstance();


	std::vector<Model*> bodies;

	Model* laserModel = app->getResourceManager().getFBXModel("laser").getModel();
	Model* projectileModel = app->getResourceManager().getFBXModel("sphere").getModel();
	Model* hookModel = app->getResourceManager().getFBXModel("projectile").getModel();
	Model* headModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botHeadNames[0] + "_head").getModel();
	Model* bodyModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botBodyNames[0] + "_body").getModel();
	Model* armLeftModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botArmNames[0] +"_armL").getModel();
	Model* armRightModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botArmNames[0] + "_armR").getModel();

	for (size_t i = 0; i < m_info->getPlayers().size(); i++) {
		headModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botHeadNames[m_info->getPlayers()[i].headModel] + "_head").getModel();
		bodyModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botBodyNames[m_info->getPlayers()[i].bodyModel] + "_body").getModel();


		Hook* tempHook = new Hook(hookModel);
		Character* tempChar = new Character(bodyModel, armLeftModel, headModel);
		Weapon* tempWeapon = new Weapon(armRightModel, laserModel, projectileModel, projHandler, particleHandler, tempChar);
		tempChar->setHook(tempHook);
		tempChar->setWeapon(tempWeapon);
		tempChar->setLightColor(m_info->getDefaultColor(m_info->getPlayers()[i].color, m_info->getPlayers()[i].hue));
		tempChar->setTeam(m_info->getPlayers()[i].team);


		if (m_info->getPlayers()[i].port >= 0) {
			tempChar->setController(1);
			tempChar->setControllerPort(m_info->getPlayers()[i].port);
		}
		else
			tempChar->setController(0);

		addPlayer(tempChar);
	}

#ifdef _DEBUG
	//if (settings->players.size() == 0) {
	//	Hook* tempHook = new Hook(hModel);
	//	Character* tempChar = new Character(cModel1, cModel2, cModel3);
	//	Weapon* tempWeapon = new Weapon(wModel, lModel, dModel, projHandler, particleHandler, tempChar);
	//	tempChar->setLightColor(settings->teamOneColor);
	//	tempChar->setTeam(1);
	//	tempChar->setHook(tempHook);
	//	tempChar->setWeapon(tempWeapon);
	//	tempChar->setControllerPort(0);
	//	tempChar->setController(true);
	//	addPlayer(tempChar);
	//}
	//if (settings->players.size() < 4) {
	//	Hook* tempHook = new Hook(hModel);
	//	Character* tempChar = new Character(cModel1, cModel2, cModel3);
	//	Weapon* tempWeapon = new Weapon(wModel, lModel, dModel, projHandler, particleHandler, tempChar);
	//	tempChar->setLightColor(settings->teamTwoColor);
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
	player->setParticleHandler(m_particleHandler);
	m_characters.push_back(player);
	m_respawnTimers.push_back(0);
	m_particleHandler->addEmitter(player->m_thrusterEmitter);
}

void CharacterHandler::addSpawnPoint(unsigned int team, const Vector3& position) {
	m_spawns[team].push_back(position);
}

void CharacterHandler::killPlayer(unsigned int index) {
	if (index < m_characters.size()) {

		// Death explosion
		m_particleHandler->addEmitter(std::shared_ptr<ParticleEmitter>(new ParticleEmitter(
			ParticleEmitter::EXPLOSION, m_characters[index]->getTransform().getTranslation(), Vector3(-0.5f), Vector3(7.f, 7.f, 4.f), 
				0.f, 75, 1.0f, 1.0f, Vector4(0.8f, 0.5f, 0.2f, 1.f), 0.2f, 75U, true, true)));

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
		m_characters[i]->update(dt);
		if (m_respawnTimers[i] > 0) {
			if (m_respawnTimers[i] >= m_respawnTime) {
				respawnPlayer(i);
				continue;
			}
			m_respawnTimers[i] += dt;
		}
		else {
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
