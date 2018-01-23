#include "PlayerCameraController.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PlayerCameraController::PlayerCameraController(Camera* cam)
	: CameraController(cam)
{
	cameraOffset = -10.0f;
}

void PlayerCameraController::update(float dt) {
	update(dt, Vector3(0.f, 0.f, 0.f));
}

void PlayerCameraController::update(float dt, const Vector3& playerPos) {
	
	setCameraPosition(Vector3(playerPos.x, playerPos.y, cameraOffset));
}

