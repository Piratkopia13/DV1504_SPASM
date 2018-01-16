#include "sail/Sail.h"

class PlayerCameraController : public CameraController {
public:

	PlayerCameraController(Camera* cam);

	virtual void update(float dt);
	void update(float dt, Terrain* collider);

private:
	float m_playerHeight;

	float m_yaw, m_pitch, m_roll;
	DirectX::SimpleMath::Vector3 m_facing, m_right;

};