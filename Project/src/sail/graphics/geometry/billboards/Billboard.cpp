#include "Billboard.h"

Billboard::Billboard(const DirectX::SimpleMath::Vector3& positon, const DirectX::SimpleMath::Vector3& velocity = DirectX::SimpleMath::Vector3::One) {
	m_position = positon;
	m_velocity = velocity;
}

Billboard::~Billboard(){}

void Billboard::update(const float dt) {
	m_position += m_velocity * dt;
}

Transform& Billboard::getTransform() {
	return m_transform;
}