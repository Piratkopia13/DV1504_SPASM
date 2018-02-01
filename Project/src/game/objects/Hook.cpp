#include "Hook.h"

Hook::Hook(Model *drawModel, Grid *levelGrid) {
	m_Model = drawModel;
	m_levelGrid = levelGrid;
}

Hook::~Hook() {

}

void Hook::update(float dt, DirectX::SimpleMath::Vector3 position) {
	if (m_triggerHeld) {
		m_direction = (m_position - position);
		m_direction.z = 0.f;
		m_distance = m_direction.Length();
		this->setPosition(position + (m_direction * 0.5f));
		m_direction.Normalize();
		this->getTransform().setRotations(DirectX::SimpleMath::Vector3(0.0f, 0.0f, atan2(m_direction.y, m_direction.x)));
		this->getTransform().setNonUniScale(m_distance*10.0f, 1.0f, 1.0f);
	}
}

void Hook::triggerPull(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 direction) {
	m_triggerHeld = true;
	m_position = m_levelGrid->raytraceBlock(position, direction);
}

void Hook::triggerRelease() {
	m_triggerHeld = false;
}

void Hook::draw() {
	if (m_triggerHeld) {
		m_Model->setTransform(&getTransform());
		m_Model->draw();
	}
}

DirectX::SimpleMath::Vector3 Hook::getDirection() {
	return m_direction;
}