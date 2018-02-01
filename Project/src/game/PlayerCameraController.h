#pragma once
#include "../sail/Sail.h"
#include "objects\Character.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam);

	void update(float dt);
	void setTargets(Object* focusObject1, Object* focusObject2, Object* focusObject3, Object* focusObject4);
	void setPosition(Vector3 pos);
	void setTarget(Vector3 pos);
	void setOffset(Vector3 offset);
	void setMoveSpeed(float speed);
	void setFollowSpeed(float speed);


	Vector3 getPosition();
	Vector3 getTarget();

	void setMoving(bool stat);

private:
	Object * targets[4];

	
	Vector3 position;
	Vector3 target;


	Vector3 back;
	Vector3 up;


	bool moving;
	void updatePosition(float dt);


	float followSpeed;
	float moveSpeed;

	float m_cameraZOffset;
	float extraZ;
	float m_cameraYOffset;

};