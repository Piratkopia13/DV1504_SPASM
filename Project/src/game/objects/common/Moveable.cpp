#include "Moveable.h"

Moveable::Moveable() {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_gravity = DirectX::SimpleMath::Vector3(0.f, 0.0f, 0.f);
	m_acceleration = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	this->grav = 0;
}

Moveable::~Moveable() {

}

void Moveable::move(const float dt) {



	this->getTransform().translate(m_velocity * dt);
	this->updateBoundingBox();
}

void Moveable::setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity) {
	m_velocity = newVelocity;
}

const DirectX::SimpleMath::Vector3& Moveable::getVelocity() {
	return m_velocity;
}
void Moveable::setAcceleration(const DirectX::SimpleMath::Vector3 &newAcceleration) {
	m_acceleration = newAcceleration;
}

void Moveable::addAcceleration(const DirectX::SimpleMath::Vector3& accel)
{
	this->m_acceleration += accel;
}