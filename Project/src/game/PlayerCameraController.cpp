#include "PlayerCameraController.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PlayerCameraController::PlayerCameraController(Camera* cam)
	: CameraController(cam)
{
	setCameraDirection(Vector3(0.f, 0.f, 1.0f));
}

void PlayerCameraController::update(float dt) {

}

