#pragma once
#include "../sail/Sail.h"
#include "objects\Character.h"

class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam, const DirectX::SimpleMath::Vector2* mapSize = nullptr);

	void update(float dt);
	void setTargets(Object* focusObject1, Object* focusObject2 = nullptr, Object* focusObject3 = nullptr, Object* focusObject4 = nullptr);
	void setPosition(DirectX::SimpleMath::Vector3 pos);
	void setTarget(DirectX::SimpleMath::Vector3 pos);
	void setOffset(DirectX::SimpleMath::Vector3 offset);
	void setMoveSpeed(float speed);
	void setFollowSpeed(float speed);


	DirectX::SimpleMath::Vector3 getPosition();
	DirectX::SimpleMath::Vector3 getTarget();

	void setMoving(bool stat);
	bool useExtra;

private:
	Object * targets[4];

	
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 target;


	DirectX::SimpleMath::Vector3 back;
	DirectX::SimpleMath::Vector3 up;

	bool m_lockToMap;
	DirectX::SimpleMath::Vector2 m_mapSize;


	bool moving;
	void updatePosition(float dt);


	float followSpeed;
	float moveSpeed;

	float m_cameraZOffset;
	float extraZ;
	float m_cameraYOffset;

};