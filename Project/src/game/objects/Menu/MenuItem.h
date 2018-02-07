#pragma once
#include "../common/Object.h"
class MenuItem : public Object {
public:
	MenuItem();
	MenuItem(Model* model, const DirectX::SimpleMath::Vector3& pos);
	~MenuItem();

	bool useColor;

	virtual void draw();
};

