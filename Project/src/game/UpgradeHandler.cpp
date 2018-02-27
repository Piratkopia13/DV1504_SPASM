#include "UpgradeHandler.h"


using namespace DirectX::SimpleMath;

UpgradeHandler::UpgradeHandler() {
	Application* app = Application::getInstance();
	Application::GameSettings* settings = &app->getGameSettings();

	Model* wModel = app->getResourceManager().getFBXModel("weapon").getModel();
}


UpgradeHandler::~UpgradeHandler() {
	for (size_t i = 0; i < m_spawns.size(); i++) {
		Memory::safeDelete(m_spawns[i]);
	}
}

void UpgradeHandler::addSpawn(const DirectX::SimpleMath::Vector3 & pos, int type, float time) {

	UpgradeSpawn* temp = new UpgradeSpawn(pos, time, type);
	m_spawns.push_back(temp);
	
}

size_t UpgradeHandler::getNrOfSpawners()
{
	return m_spawns.size();
}

UpgradeHandler::UpgradeSpawn * UpgradeHandler::getSpawn(size_t index)
{
	return m_spawns[index];
}

void UpgradeHandler::update(float dt) {
	for (size_t i = 0; i < m_spawns.size(); i++) {
		m_spawns[i]->update(dt);
	}
}




/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////




UpgradeHandler::UpgradeSpawn::UpgradeSpawn() {

}

UpgradeHandler::UpgradeSpawn::UpgradeSpawn(const DirectX::SimpleMath::Vector3& _position, float _cooldown, int type) {
	setModel(Application::getInstance()->getResourceManager().getFBXModel("upgrade_spawner").getModel());
	setPosition(_position);
	updateBoundingBox();
	m_upgradeObject = new UpgradeObject(_position+Vector3(0,0.5f,0));
	m_upgradeObject->setModel(Application::getInstance()->getResourceManager().getFBXModel("weapon_upgrade").getModel());
	m_upgradeObject->updateBoundingBox();
	m_respawnTime = _cooldown;
	m_type = type;

	respawn();
}

UpgradeHandler::UpgradeSpawn::~UpgradeSpawn() {
	Memory::safeDelete(m_upgradeObject);
}

void UpgradeHandler::UpgradeSpawn::update(float dt) {

	if (!m_online) {

		m_timeLeft -= dt;
		if (m_timeLeft <= 0) {
			respawn();
		}

	}
	else {
		m_upgradeObject->update(dt);
	}
}

void UpgradeHandler::UpgradeSpawn::draw() {

	model->setTransform(&getTransform());
	model->getMaterial()->setColor(lightColor);
	model->draw();

	if (m_online) {
		m_upgradeObject->draw();
	}
}

bool UpgradeHandler::UpgradeSpawn::getOnline()
{
	return m_online;
}

void UpgradeHandler::UpgradeSpawn::respawn() {

	m_onlineType = m_type;
	if (m_type == -1) {
		m_onlineType = (int)(Utils::rnd() * Upgrade::MAX - 1) + 1;
	}

	if (m_onlineType == Upgrade::AUTO_FIRE) {
		m_upgradeObject->setLightColor(Vector4(1, 0, 0, 1));
	}
	if (m_onlineType == Upgrade::PROJECTILE_KNOCKBACK) {
		m_upgradeObject->setLightColor(Vector4(0, 1, 0, 1));
	}
	if (m_onlineType == Upgrade::EXTRA_DAMAGE) {
		m_upgradeObject->setLightColor(Vector4(0, 0, 1, 1));
	}
	if (m_onlineType == Upgrade::EXTRA_PROJECTILES) {
		m_upgradeObject->setLightColor(Vector4(0, 1, 1, 1));
	}
	if (m_onlineType == Upgrade::NO_GRAVITY) {
		m_upgradeObject->setLightColor(Vector4(1, 1, 0, 1));
	}
	m_online = true;
	m_timeLeft = 0;
}

const Upgrade UpgradeHandler::UpgradeSpawn::take() {
	
	m_online = false;
	m_timeLeft = m_respawnTime;
	float rnd = Utils::rnd() * 0.2f + 0.8f;
	Application::getInstance()->getResourceManager().getSoundManager()->playSoundEffect(SoundManager::SoundEffect::Pickup, 0.2f, rnd);
	return Upgrade(m_onlineType);
}

void UpgradeHandler::UpgradeSpawn::reset() {
	m_online = true;
	m_timeLeft = 0;
	

}



////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////


UpgradeHandler::UpgradeSpawn::UpgradeObject::UpgradeObject(const DirectX::SimpleMath::Vector3& pos) {
	setPosition(pos);
	m_spawnPosition = pos;

}

UpgradeHandler::UpgradeSpawn::UpgradeObject::~UpgradeObject() {

}

void UpgradeHandler::UpgradeSpawn::UpgradeObject::update(float dt) {
	static Vector3 directionVec(0, 1, 0);
	static float offset = 0;
	static int direction = 0;
	static float rotationOffset = 0;

	if (direction == 0) {
		offset += 0.2f*dt;
		if (offset > 0.3) {
			direction = 1;
		}
	}
	else if(direction == 1) {
		offset -= 0.2f*dt;
		if (offset < -0.3f) {
			direction = 0;
		}
	}

	rotationOffset = (rotationOffset + 0.2f*dt);
	if (rotationOffset > 6.28f)
		rotationOffset = 0;
	

	setPosition(m_spawnPosition + directionVec * offset);
	getTransform().setRotations(Vector3(0, rotationOffset, 0));
}

void UpgradeHandler::UpgradeSpawn::UpgradeObject::draw() {
	model->setTransform(&getTransform());
	model->getMaterial()->setColor(lightColor);
	model->draw();
}
