#include "Moveable.h"

Moveable::Moveable() {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
}

Moveable::Moveable(const float mass) {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_mass = mass;
}

Moveable::~Moveable() {

}

void Moveable::update(const float dt) {
	this->getTransform().translate(this->m_velocity);
}

void Moveable::setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity) {
	m_velocity = newVelocity;
}

