#include "Object.h"

Object::Object() {
	m_pos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	m_boundingBox = nullptr;
}

Object::~Object() {

}

void Object::setPosition(const DirectX::SimpleMath::Vector3 &newPosition) {
	m_transformation.setTranslation(newPosition);
}

void Object::move(const DirectX::SimpleMath::Vector3 &distanceToMove) {
	m_transformation.translate(distanceToMove);
}

Transform& Object::getTransformation() {
	return m_transformation;
}

AABB* Object::getBoundingBox() const {
	return m_boundingBox;
}