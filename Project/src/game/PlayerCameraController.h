#include "../sail/Sail.h"

class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam);

	virtual void update(float dt);
	void update(float dt, Terrain* collider);

private:

};