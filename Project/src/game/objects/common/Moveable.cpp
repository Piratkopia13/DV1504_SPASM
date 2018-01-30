#include "Moveable.h"

Moveable::Moveable() {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_acceleration = DirectX::SimpleMath::Vector3(0.f, -9.82f, 0.f);
}

Moveable::Moveable(const float mass) {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_mass = mass;
}

Moveable::~Moveable() {

}

void Moveable::move(const float dt) {

	//m_velocity += m_acceleration * dt;

	this->getTransform().translate(m_velocity*dt);
	this->updateBoundingBox();
}

void Moveable::setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity) {
	m_velocity = newVelocity;
}

const DirectX::SimpleMath::Vector3& Moveable::getVelocity() {
	return m_velocity;
}
