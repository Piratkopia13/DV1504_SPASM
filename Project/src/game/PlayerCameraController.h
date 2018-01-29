#include "../sail/Sail.h"
#include "objects\Character.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam);

	void update(float dt);
	void setTargets(Object* focusObject1, Object* focusObject2, Object* focusObject3, Object* focusObject4);

private:
	Object * targets[4];

	
	Vector3 position;
	Vector3 target;


	Vector3 back;
	Vector3 up;



	void updatePosition(float dt);


	float followSpeed;
	float moveSpeed;

	float m_cameraZOffset;
	float extraZ;
	float m_cameraYOffset;

};