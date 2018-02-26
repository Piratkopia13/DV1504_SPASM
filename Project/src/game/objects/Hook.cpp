#include "Hook.h"
#include "../collision/CollisionHandler.h"

Hook::Hook(Model *drawModel) {
	model = drawModel;
	setLightColor(DirectX::SimpleMath::Vector4(3.f));
}

Hook::~Hook() {

}

void Hook::update(float dt, const DirectX::SimpleMath::Vector3& position) {
	if (m_triggerHeld) {
		m_direction = (m_position - position);
		m_direction.z = 0.f;
		m_distance = m_direction.Length();
		m_direction.Normalize();
		this->setPosition(position + (m_direction * m_distance * 0.5f));
		this->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(m_direction.y, m_direction.x)));
		this->getTransform().setNonUniScale(m_distance*10.0f, 1.0f, 1.0f);
	}
}

bool Hook::triggerPull(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) {
	m_triggerHeld = true;
	m_position = CollisionHandler::getInstance()->rayTraceLevel(position, direction);
	m_direction = (m_position - position);
	m_direction.z = 0.f;
	m_distance = m_direction.Length();
	if (m_distance > 1000) {
		m_triggerHeld = false;
	}
	return m_triggerHeld;
}

void Hook::triggerRelease() {
	m_triggerHeld = false;
	this->getTransform().setScale(0.f);
}

void Hook::draw() {
	model->setTransform(&getTransform());
	model->getMaterial()->setColor(this->lightColor);
	model->draw();
}

DirectX::SimpleMath::Vector3 Hook::getDirection() {
	DirectX::SimpleMath::Vector3 tempvec = m_direction;
	tempvec.Normalize();
	return tempvec;
}

float Hook::getLength(DirectX::SimpleMath::Vector3 playerPos){
	return (m_position - playerPos).Length();
}