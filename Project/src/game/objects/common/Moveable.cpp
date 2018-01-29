#include "Moveable.h"

Moveable::Moveable() {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_gravity = DirectX::SimpleMath::Vector3(0.f, -0.0f, 0.f);
	m_acceleration = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
}

Moveable::~Moveable() {

}

void Moveable::move(const float dt) {

	m_acceleration += m_gravity;

	m_velocity += m_acceleration * dt;

	this->getTransform().translate(m_velocity*dt);
	this->updateBoundingBox();
}

void Moveable::setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity) {
	m_velocity = newVelocity;
}

void Moveable::setAcceleration(const DirectX::SimpleMath::Vector3 &newAcceleration) {
	m_acceleration = newAcceleration;
}

