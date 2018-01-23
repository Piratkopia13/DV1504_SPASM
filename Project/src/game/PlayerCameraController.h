#include "../sail/Sail.h"

class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam);

	void update(float dt);

private:
	float m_playerHeight;

	float m_yaw, m_pitch, m_roll;
	DirectX::SimpleMath::Vector3 m_facing, m_right;

};