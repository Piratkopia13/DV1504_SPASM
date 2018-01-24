#include "Object.h"

Object::Object() {
	m_boundingBox = nullptr;
}

Object::~Object() {

}

void Object::setPosition(const DirectX::SimpleMath::Vector3 &newPosition) {
	m_transform.setTranslation(newPosition);
}


Transform& Object::getTransformation() {
	return m_transformation;
}

AABB* Object::getBoundingBox() const {
	return m_boundingBox;
}