#include "Moveable.h"

Moveable::Moveable() {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_acceleration = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
}

Moveable::Moveable(const float mass) {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_mass = mass;
}

Moveable::~Moveable() {

}

void Moveable::move(const float dt) {

	DirectX::SimpleMath::Vector3 gravity = DirectX::SimpleMath::Vector3(0.f, -9.82f, 0.f);

	m_acceleration += gravity * dt;

	this->getTransform().translate(m_velocity + m_acceleration);
	
}

void Moveable::setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity) {
	m_velocity = newVelocity;
}

