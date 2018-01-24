#include "Object.h"

Object::Object() {
	m_boundingBox = nullptr;
}

Object::~Object() {

}

void Object::setPosition(const DirectX::SimpleMath::Vector3 &newPosition) {
	m_transform.setTranslation(newPosition);
}

void Object::move(const DirectX::SimpleMath::Vector3 &distanceToMove) {
	m_transform.translate(distanceToMove);
}

Transform& Object::getTransform() {
	return m_transform;
}

AABB* Object::getBoundingBox() const {
	return m_boundingBox;
}