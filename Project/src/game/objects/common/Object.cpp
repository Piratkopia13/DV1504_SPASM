#include "Object.h"

Object::Object() {
	m_boundingBox = nullptr;
	this->m_transform.setScale(0.01f);
}

Object::~Object() {

}

void Object::setPosition(const DirectX::SimpleMath::Vector3 &newPosition) {
	m_transform.setTranslation(newPosition);
}


Transform& Object::getTransform() {
	return m_transform;
}

AABB* Object::getBoundingBox() const {
	return m_boundingBox;
}