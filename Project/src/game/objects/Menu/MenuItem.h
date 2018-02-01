#pragma once
#include "../common/Object.h"
class MenuItem :
	public Object
{
public:
	MenuItem();
	MenuItem(Model* model, DirectX::SimpleMath::Vector3 pos);

	bool useColor;

	void draw();
	~MenuItem();
};

