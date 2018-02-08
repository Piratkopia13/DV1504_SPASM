#pragma once

#include "../../../sail/Sail.h"
#include "../../../sail/graphics/geometry/Model.h"

class Object {
private:
	Transform m_transform;
protected:
	Model* model; //Pointer to be able to detect if a bounding box has been made
	AABB* boundingBox;
	DirectX::SimpleMath::Vector4 lightColor;

public:
	Object();
	virtual ~Object();

	void setPosition(const DirectX::SimpleMath::Vector3 &newPosition);
	void updateBoundingBox();
	void setModel(Model* model);
	void setLightColor(DirectX::SimpleMath::Vector4 color);
	Model* getModel();
	Transform& getTransform();
	AABB* getBoundingBox();

	virtual void draw() = 0;
};