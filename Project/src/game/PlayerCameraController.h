#include "../sail/Sail.h"

class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam);

	virtual void update(float dt);
	void update(float dt, DirectX::SimpleMath::Vector3 playerPos);

private:

	float cameraOffset;

};