#include "PlayerCameraController.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PlayerCameraController::PlayerCameraController(Camera* cam)
	: CameraController(cam)
{
	m_yaw = 90.f;
	m_pitch = -20.f;
	m_roll = 0.f;

	m_facing = Vector3(0.f, 0.f, 1.f);
	m_right = Vector3(1.0f, 0.f, 0.f);
	setCameraDirection(Vector3(0.f, 0.f, 1.0f));

	m_playerHeight = 1.8f;
}

void PlayerCameraController::update(float dt) {
	Application* app = Application::getInstance();

	auto& keyboard = app->getInput().keyboard;
	auto kbState = app->getInput().keyboardState;
	auto gpState = app->getInput().gamepadState;

	float movementSpeed = dt;
	float lookSensitivityMouse = .1f;
	float lookSensitivityController = 90.0f * 0.016f;

	// Increase speed if shift or right trigger is pressed
	if (kbState.LeftShift)
		movementSpeed *= 5.f;
	if (gpState.triggers.right > 0)
		movementSpeed *= gpState.triggers.right * 5.f;

	// Increase controller sensitivity with the left trigger
	if (gpState.triggers.left > 0)
		lookSensitivityController *= gpState.triggers.left * 5.0f;


	//
	// Forwards / backwards motion
	//

	// Gamepad
	setCameraPosition(getCameraPosition() + m_facing * gpState.thumbSticks.leftY * movementSpeed);

	// Keyboard
	if (kbState.W)
		setCameraPosition(getCameraPosition() + m_facing * movementSpeed);
	if (kbState.S)
		setCameraPosition(getCameraPosition() - m_facing * movementSpeed);

	//
	// Side to side motion
	//

	// Gamepad
	setCameraPosition(getCameraPosition() + m_right * gpState.thumbSticks.leftX * movementSpeed);

	
	// Keyboard
	if (kbState.A)
		setCameraPosition(getCameraPosition() - m_right * movementSpeed);
	if (kbState.D)
		setCameraPosition(getCameraPosition() + m_right * movementSpeed);

	//
	// Look around motion
	//

	// Gamepad
	m_pitch += gpState.thumbSticks.rightY * lookSensitivityController;
	m_yaw -= gpState.thumbSticks.rightX * lookSensitivityController;

	// Toggle cursor capture on right click
	if (app->getInput().wasJustPressed(Application::Input::MouseButton::RIGHT)) {
		app->getInput().showCursor(app->getInput().isCursorHidden());
	}

	if (app->getInput().isCursorHidden()) {
		m_pitch -= (float)(app->getInput().getMouseDY()) * lookSensitivityMouse;
		m_yaw -= (float)(app->getInput().getMouseDX()) * lookSensitivityMouse;
	}

	// Lock pitch to the range -89 - 89
	if (m_pitch >= 89)
		m_pitch = 89;
	else if (m_pitch <= -89)
		m_pitch = -89;

	// Lock yaw to the range 0 - 360
	if (m_yaw >= 360)
		m_yaw -= 360;
	else if (m_yaw <= 0)
		m_yaw += 360;

	Vector3 forwards(
		std::cos(XMConvertToRadians(m_pitch)) * std::cos(XMConvertToRadians(m_yaw)),
		std::sin(XMConvertToRadians(m_pitch)),
		std::cos(XMConvertToRadians(m_pitch)) * std::sin(XMConvertToRadians(m_yaw))
	);
	forwards.Normalize();
	setCameraDirection(forwards);

	m_facing = Vector3(
		std::cos(XMConvertToRadians(m_yaw)),
		0.f,
		std::sin(XMConvertToRadians(m_yaw))
	);
	m_facing.Normalize();
	m_right = Vector3::Up.Cross(m_facing);

}