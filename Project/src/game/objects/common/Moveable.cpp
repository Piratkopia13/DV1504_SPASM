#include "Moveable.h"

Moveable::Moveable()
	: m_currLevel(nullptr) {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_gravity = DirectX::SimpleMath::Vector3(0.f, -9.82f, 0.f);
	m_acceleration = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	this->grav = 0;
}

Moveable::~Moveable() {
}

void Moveable::move(const float dt, bool checkColl) {
	if(!m_grounded && checkColl)
		m_velocity += m_gravity * dt;
	// Is only run if the object actually contains a pointer to the level
	if (m_currLevel)
		m_currLevel->collisionTest(*this, dt);



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

void Moveable::setCurrentLevel(Level* level) {
	m_currLevel = level;
}

void Moveable::setGrounded(bool grounded) {
	m_grounded = grounded;
}

bool Moveable::grounded() {
	return m_grounded;
}