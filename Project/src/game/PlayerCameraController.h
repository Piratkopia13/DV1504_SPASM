#include "../sail/Sail.h"
#include "objects\Character.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
class PlayerCameraController : public CameraController {
public:
	PlayerCameraController(Camera* cam, const DirectX::SimpleMath::Vector2* mapSize = nullptr);

	void update(float dt);
	void setTargets(Object* focusObject1, Object* focusObject2 = nullptr, Object* focusObject3 = nullptr, Object* focusObject4 = nullptr);

private:
	Object * targets[4];

	
	Vector3 position;
	Vector3 target;


	Vector3 back;
	Vector3 up;

	bool m_lockToMap;
	DirectX::SimpleMath::Vector2 m_mapSize;

	void updatePosition(float dt);


	float followSpeed;
	float moveSpeed;

	float m_cameraZOffset;
	float extraZ;
	float m_cameraYOffset;

};