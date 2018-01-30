#pragma once
#include "../../sail/Application.h"
#include "common/Moveable.h"
#include "../../sail/graphics/geometry/Model.h"
#include "Weapon.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Character : public Moveable {
public:
	Character();
	Character(Model * model);
	Character(Model * model, unsigned int usingController, unsigned int port);
	~Character();

	void input(
		DirectX::GamePad::State& padState,
		GamePad::ButtonStateTracker& padTracker, 
		DirectX::Keyboard::State& keyState,
		Keyboard::KeyboardStateTracker& keyTracker);
	void update(float dt);
	void draw();

	void setController(bool usingController);
	void setControllerPort(unsigned int port);
	void addVibration(unsigned int index, float addition);
	unsigned int getPort();

	void setTeam(unsigned int team);
	void setWeapon(Weapon* weapon);


private:
	
	Weapon* currentWeapon;

	unsigned int currentTeam;
	bool usingController;
	unsigned int controllerPort;

	DirectX::SimpleMath::Vector3 inputVec;
	DirectX::SimpleMath::Vector3 aimVec;
	bool jumping;
	float jumpTimer;

	float speed;
	float padVibration[2];
	float vibrationReduction[2];
	void jump();
	void stopJump();
	void fire();
	void hook();

	bool updateVibration(float dt);
	float sinDegFromVec(Vector3 vec) {

		return atan2(vec.y, vec.x);
	}
}; 

