#pragma once
#include "common/Object.h"
#include "../../sail/graphics/geometry/Model.h"

class Character : public Object {
public:
	Character();
	Character(Model * model);
	~Character();

	void input();
	void update(float dt);
	void draw();

	void setController(bool usingController);
	void setControllerPort(unsigned int port);

private:

	Model * model;
	bool usingController;
	unsigned int controllerPort;

	DirectX::SimpleMath::Vector3 inputVec;
	float speed;


}; 