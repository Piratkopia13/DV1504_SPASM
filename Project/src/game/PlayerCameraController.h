#include "../sail/Sail.h"
#include "objects\common\Object.h"

class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam);

	void update(float dt, Object& focusObject);
	void setTargets(Object& focusObject1, Object& focusObject2, Object& focusObject3, Object& focusObject4);

private:
	Object * targets[4];

	float m_cameraOffset;

};