#include "Object.h"

Object::Object() {
	m_pos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
}

Object::~Object() {

}

void Object::setPosition(DirectX::SimpleMath::Vector3 newPosition) {
	m_pos = newPosition;
}

DirectX::SimpleMath::Vector3 Object::getPosition() const {
	return m_pos;
}

AABB* Object::getBoundingBox() const {
	return m_boundingBox;
}