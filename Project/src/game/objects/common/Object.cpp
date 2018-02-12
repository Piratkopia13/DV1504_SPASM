#include "Object.h"

using namespace DirectX::SimpleMath;

Object::Object()
	: model(nullptr)
	, lightColor(Vector4(1.f))
{
}

Object::~Object() {
	if (boundingBox) {
		delete this->boundingBox;
	}
}

void Object::setPosition(const DirectX::SimpleMath::Vector3 &newPosition) {
	m_transform.setTranslation(newPosition);
	updateBoundingBox();
}

void Object::updateBoundingBox()
{
	if (this->boundingBox) {
		//this->boundingBox->updateTranslation(this->getTransform().getTranslation());
		this->boundingBox->updateTransform(this->getTransform().getMatrix());
	}
}

void Object::setModel(Model* model)
{
	if (model) {
		this->model = model;
		if (&model->getAABB()) {
			if (this->boundingBox) {
				delete this->boundingBox;
			}
			this->boundingBox = new AABB(model->getAABB());
		}
		else {
			
			Logger::Error("BoundingBox not loaded");
		}
	}
}

void Object::setLightColor(DirectX::SimpleMath::Vector4 color)
{
	this->lightColor = color;
}

Model* Object::getModel()
{
	return this->model;
}

Transform& Object::getTransform() {
	return m_transform;
}

AABB* Object::getBoundingBox() {
	return boundingBox;
}