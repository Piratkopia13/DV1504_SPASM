#include "CharacterHandler.h"
#include "collision/CollisionHandler.h"

using namespace DirectX::SimpleMath;

CharacterHandler::CharacterHandler(ParticleHandler* particleHandler, ProjectileHandler* projHandler)
	: m_particleHandler(particleHandler)
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
	Model* legsModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botLegNames[0] + "_legs").getModel();

	bool keyboardUsed = false;
	for (size_t i = 0; i < m_info->getPlayers().size(); i++) {
		headModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botHeadNames[m_info->getPlayers()[i].headModel] + "_head").getModel();
		bodyModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botBodyNames[m_info->getPlayers()[i].bodyModel] + "_body").getModel();
		legsModel = app->getResourceManager().getFBXModel("fisk/" + m_info->botLegNames[m_info->getPlayers()[i].legModel] + "_legs").getModel();

		Hook* tempHook = new Hook(hookModel, particleHandler);
		Character* tempChar = new Character(bodyModel, armLeftModel, headModel, legsModel, i);
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

#ifdef _DEBUG
		if (!keyboardUsed && !Application::getInstance()->getInput().getGamePadState(i).IsConnected()) {
			tempChar->setController(false);
			keyboardUsed = true;
		}
#endif
		tempChar->setRespawnTime(m_info->convertedGameSettings.respawnTime);
		tempChar->m_playerHealth.setMax(m_info->convertedGameSettings.playerLife);
		tempChar->setGravScale(m_info->convertedGameSettings.gravity);
		addPlayer(tempChar);
	}

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

Vector3 CharacterHandler::getRandomSpawnPoint(UINT team) const {
	Vector3 respawnPos(0, 0, 0);
	if (m_spawns[team].size() > 0) {
		unsigned int spawn = unsigned int(Utils::rnd()*m_spawns[team].size());
		respawnPos = m_spawns[team][spawn];
	}
	return respawnPos + Vector3(0.f, 1.f, 0.f);
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


		m_characters[index]->setRespawnPoint(getRandomSpawnPoint(m_characters[index]->getTeam()));
		m_characters[index]->dead();
		m_info->getScore().addDeath(index);
		if(m_characters[index]->getLastAttacker() != -1)
			m_info->getScore().addKill(m_characters[index]->getLastAttacker());
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
		// Set spawn for new players
		if (m_characters[id]->m_nextRespawnPoint == Vector3::Zero)
			m_characters[id]->setPosition(getRandomSpawnPoint(m_characters[id]->getTeam()));

		m_characters[id]->living();
	}
}

void CharacterHandler::setRespawnTime(float time) {

	for (Character* chara : m_characters) {
		chara->setRespawnTime(time < 0 ? 0 : time);
	}
}

void CharacterHandler::update(float dt) {
	
	//if (m_characters[0]->m_inputDevice.controllerPort == 0) {
	//	Vector3 hitPoint;
	//	float t;
	//	Vector3 middleMuzzlePos = m_characters[0]->m_weapon->getNozzlePos();
	//	middleMuzzlePos.z = 0.f;
	//	bool hit = CollisionHandler::getInstance()->rayTraceAABB({ middleMuzzlePos, m_characters[0]->m_input.aim }, *m_characters[1]->getBoundingBox(), hitPoint, t);
	//	if (hit) {
	//		Logger::log("HIT @ " + Utils::vec3ToStr(hitPoint));
	//	}
	//}
	
	for (size_t i = 0; i < m_characters.size(); i++) {
		m_characters[i]->update(dt);
		if (m_respawnTimers[i] > 0) {
			if (m_respawnTimers[i] >= m_characters[i]->getRespawnTime()) {
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
