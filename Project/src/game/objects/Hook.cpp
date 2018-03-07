#include "Hook.h"
#include "../collision/CollisionHandler.h"

Hook::Hook(Model *drawModel, ParticleHandler* particleHandler) {
	model = drawModel;
	m_hookVelocity = 60.f;
	setLightColor(DirectX::SimpleMath::Vector4(3.f));
	m_hookEmitter = std::shared_ptr<ParticleEmitter>(new ParticleEmitter(ParticleEmitter::EXPLOSION, DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f),
	DirectX::SimpleMath::Vector3(-0.5f, -0.5f, -0.5f), DirectX::SimpleMath::Vector3(2.0f, 2.0f, 2.0f), 0.f, 800, 0.15f, 0.5f, DirectX::SimpleMath::Vector4(0.8f, 0.5f, 0.2f, 1.f), 0.0f, 0U, true, false));
	particleHandler->addEmitter(m_hookEmitter);
}

Hook::~Hook() {

}

bool Hook::update(float dt, const DirectX::SimpleMath::Vector3& position) {
	if (m_triggerHeld) {
		m_direction = (m_position - position);
		m_direction.z = 0.f;
		m_distance = m_direction.Length();
		m_direction.Normalize();


		if (!m_outOfBounds) {
			DirectX::SimpleMath::Vector3 tempPos;
			float t;
			CollisionHandler::getInstance()->rayTraceLevel({ position, m_direction }, tempPos, t);
			float tempDistance = (tempPos - position).Length();
			if (m_distance > tempDistance) {
				m_position = tempPos;
				m_distance = tempDistance;
			}
		}
		m_timeHooked += dt;

	
		float hookPos;
		if (m_outOfBounds && m_timeHooked > 0.2f)
			hookPos = 0.0f;
		else
			hookPos = m_hookVelocity * m_timeHooked;
		hookPos = min(hookPos / m_distance, 0.5f);
		this->setPosition(position + (m_direction * m_distance * hookPos));
		this->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(m_direction.y, m_direction.x)));
		this->getTransform().setNonUniScale(m_distance*hookPos*20.0f, 2.0f, 1.0f);
		
		if (hookPos == 0.5f) {
			m_hookEmitter->updateSpawnsPerSecond(100.f);
			//m_hookEmitter->updateColor(this->lightColor * 6.f);
			m_hookEmitter->updateVelocityRndAdd(-m_direction -DirectX::SimpleMath::Vector3(0.5f));
			m_hookEmitter->updateEmitPosition(position + (m_direction * m_distance * hookPos * 2.f));
			return true;
		}
	}
	return false;
}

void Hook::triggerPull(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) {
	m_triggerHeld = true;
	m_outOfBounds = false;
	m_timeHooked = 0.0f;
	DirectX::SimpleMath::Vector3 hitPos;
	float t;
	if (CollisionHandler::getInstance()->rayTraceLevel({ position, direction }, hitPos, t)) {
		m_position = hitPos;

	}
	else {
		m_outOfBounds = true;
		m_position = (position + direction * 100.f);
	}
	/*m_direction = (m_position - position);
	m_direction.z = 0.f;
	m_distance = m_direction.Length();
	*/

}

void Hook::triggerRelease() {
	m_triggerHeld = false;
	m_hookEmitter->updateSpawnsPerSecond(0.f);
	this->getTransform().setScale(0.f);
}

void Hook::draw() {
	model->setTransform(&getTransform());
	model->getMaterial()->setColor(DirectX::SimpleMath::Vector4(3.0f));
	model->draw();
}

DirectX::SimpleMath::Vector3 Hook::getDirection() {
	DirectX::SimpleMath::Vector3 tempvec = m_direction;
	tempvec.Normalize();
	return tempvec;
}