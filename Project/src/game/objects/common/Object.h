#pragma once

#include "../../../sail/Sail.h"
#include "../../../sail/graphics/geometry/Transform.h"
#include "../../../sail/graphics/geometry/spatial/AABB.h"

class Object {
private:
	AABB * m_boundingBox; //Pointer to be able to detect if a bounding box has been made
	Transform m_transformation;
	DirectX::SimpleMath::Vector3 m_pos;

public:
	Object();
	virtual ~Object();

	void setPosition(const DirectX::SimpleMath::Vector3 &newPosition);
	void move(const DirectX::SimpleMath::Vector3 &distanceToMove);

	Transform& getTransformation();
	AABB* getBoundingBox() const;

	virtual void draw() = 0;
};