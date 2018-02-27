#include "Hook.h"
#include "../collision/CollisionHandler.h"

Hook::Hook(Model *drawModel) {
	model = drawModel;
	m_hookVelocity = 60.f;
	setLightColor(DirectX::SimpleMath::Vector4(3.f));
}

Hook::~Hook() {

}

bool Hook::update(float dt, const DirectX::SimpleMath::Vector3& position) {
	if (m_triggerHeld && !m_outOfBounds) {
		m_direction = (m_position - position);
		m_direction.z = 0.f;
		m_distance = m_direction.Length();
		m_direction.Normalize();
		DirectX::SimpleMath::Vector3 tempPos = CollisionHandler::getInstance()->rayTraceLevel(position, m_direction);
		float tempDistance = (tempPos - position).Length();
		if (m_distance > tempDistance) {
			m_position = tempPos;
			m_distance = tempDistance;
		}
		m_timeHooked += dt;


		float hookPos = m_hookVelocity * m_timeHooked;

		hookPos = min(hookPos / m_distance, 0.5);

		this->setPosition(position + (m_direction * m_distance * hookPos));
		this->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(m_direction.y, m_direction.x)));
		this->getTransform().setNonUniScale(m_distance*hookPos*20.0f, 1.0f, 1.0f);
		if (hookPos == 0.5f)
			return true;
	}
	return false;
}

void Hook::triggerPull(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) {
	m_triggerHeld = true;
	m_outOfBounds = false;
	m_timeHooked = 0.0f;
	m_position = CollisionHandler::getInstance()->rayTraceLevel(position, direction);
	m_direction = (m_position - position);
	m_direction.z = 0.f;
	m_distance = m_direction.Length();
	if (m_distance > 10000) {
		m_outOfBounds = true;
	}
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