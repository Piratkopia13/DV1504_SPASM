#pragma once
#include "common/Moveable.h"
#include "../../sail/graphics/geometry/Model.h"

class Character : public Moveable {
public:
	Character();
	Character(Model * model);
	Character(Model * model, unsigned int usingController, unsigned int port);
	~Character();

	void input();
	void update(float dt);
	void draw();

	void setController(bool usingController);
	void setControllerPort(unsigned int port);
	void addVibration(unsigned int index, float addition);
	

private:

	Model * model;
	bool usingController;
	unsigned int controllerPort;

	DirectX::SimpleMath::Vector3 inputVec;
	float speed;
	float padVibration[4];



	void updateVibration(float dt);
}; 