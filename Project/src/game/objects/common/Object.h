#pragma once

#include "../../../sail/Sail.h"
#include "../../../sail/graphics/geometry/Transform.h"
#include "../../../sail/graphics/geometry/spatial/AABB.h"

class Object {
private:
	AABB * m_boundingBox; //Pointer or not? Thinking pointer to be able to detect if a bounding box has been made?
	Transform m_transformation;
	DirectX::SimpleMath::Vector3 m_pos;

public:
	Object();
	virtual ~Object();

	void setPosition(DirectX::SimpleMath::Vector3 newPosition);

	DirectX::SimpleMath::Vector3 getPosition() const;
	AABB* getBoundingBox() const;
};