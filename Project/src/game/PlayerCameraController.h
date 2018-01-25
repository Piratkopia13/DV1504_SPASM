#include "../sail/Sail.h"
#include "objects\common\Object.h"

class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam);

	void update(float dt, Object& focusObject);

private:

	float m_cameraOffset;

};