#include "Object.h"

Object::Object() {
	m_Model = nullptr;
}

Object::~Object() {
	delete this->boundingBox;
}

void Object::setPosition(const DirectX::SimpleMath::Vector3 &newPosition) {
	m_transform.setTranslation(newPosition);
}

void Object::updateBoundingBox()
{
	if(this->boundingBox)
		this->boundingBox->updateTransform(this->getTransform().getMatrix());
}

void Object::setModel(Model * model)
{
	if (model) {
		this->m_Model = model;
		if(&model->getAABB())
			this->boundingBox = new AABB(model->getAABB());
		else {
			
			Logger::Error("BoundingBox not loaded");
		}
	}
}

Model* Object::getModel()
{
	return m_Model;
}

Transform& Object::getTransform() {
	return m_transform;
}