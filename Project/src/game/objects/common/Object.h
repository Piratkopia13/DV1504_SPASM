#pragma once

#include "../../../sail/Sail.h"
#include "../../../sail/graphics/geometry/Model.h"

class Object {
private:
	Transform m_transform;
protected:
	Model * m_Model; //Pointer to be able to detect if a bounding box has been made
	AABB * boundingBox;

public:
	Object();
	virtual ~Object();

	void setPosition(const DirectX::SimpleMath::Vector3 &newPosition);
	void updateBoundingBox();
	void setModel(Model* model);
	Model* getModel();
	Transform& getTransform();

	virtual void draw() = 0;
};