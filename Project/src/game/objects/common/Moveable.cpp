#include "Moveable.h"

Moveable::Moveable() {
	m_velocity = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_gravity = DirectX::SimpleMath::Vector3(0.f, -9.82f, 0.f);
	m_acceleration = DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f);
	m_gravScale = 1;
	m_grounded = true;
}

Moveable::~Moveable() {
}

void Moveable::move(const float dt, bool includeBoundingBoxRotation) {
	move(m_velocity * dt, includeBoundingBoxRotation);
}
void Moveable::updateVelocity(const float dt) {
	if (!m_grounded)
		m_velocity += (m_gravity*m_gravScale + m_acceleration) * dt;
	else
		m_velocity += m_acceleration * dt;
}

void Moveable::move(DirectX::SimpleMath::Vector3& toMove, bool includeBoundingBoxRotation) {
	this->getTransform().translate(toMove);
	this->updateBoundingBox(includeBoundingBoxRotation);
}

void Moveable::setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity) {
	m_velocity = newVelocity;
}

void Moveable::setGravScale(float scale) {
	m_gravScale = scale;
}

const DirectX::SimpleMath::Vector3& Moveable::getVelocity() {
	return m_velocity;
}
void Moveable::setAcceleration(const DirectX::SimpleMath::Vector3 &newAcceleration) {
	m_acceleration = newAcceleration;
}

void Moveable::addAcceleration(const DirectX::SimpleMath::Vector3& accel) {
	this->m_acceleration += accel;
}

void Moveable::setGrounded(bool grounded) {
	m_grounded = grounded;
}

bool Moveable::grounded() {
	return m_grounded;
}