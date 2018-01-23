#include "../sail/Sail.h"

class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam);

	virtual void update(float dt);//Probably won't need dt
	void update(float dt, const DirectX::SimpleMath::Vector3& playerPos);

private:

	float cameraOffset;

};