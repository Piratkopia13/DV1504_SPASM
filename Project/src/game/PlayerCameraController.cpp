#include "PlayerCameraController.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PlayerCameraController::PlayerCameraController(Camera* cam)
	: CameraController(cam)
{
	m_cameraOffset = -60.0f;
}

void PlayerCameraController::update(float dt, Object& focusObject) {
	
	setCameraPosition(focusObject.getTransform().getTranslation() + Vector3(0.0f, 25.0f, m_cameraOffset));
	setCameralookAt(focusObject.getTransform().getTranslation());
}

