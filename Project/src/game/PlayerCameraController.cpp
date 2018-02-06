#include "PlayerCameraController.h"
#include <algorithm>

using namespace DirectX;
using namespace DirectX::SimpleMath;

PlayerCameraController::PlayerCameraController(Camera* cam, const DirectX::SimpleMath::Vector2* mapSize)
	: CameraController(cam)
	, m_lockToMap(mapSize != nullptr)
	, m_extraZCurrent(0.f)
	, m_extraZTarget(0.f)
	, m_cameraYOffset(1.0f)
	, m_cameraZOffset(8.0f)
	, m_followSpeed(2.f)
	, m_moveSpeed(2)
	, m_position(Vector3(0, 0, 0))
	, m_target(Vector3(0, 0, 0))
	, m_back(Vector3(0, 0, -1))
	, m_up(Vector3(0, 1, 0))
	, m_moving(true)
	, m_useExtraZ(true)
{

	if (m_lockToMap)
		m_mapSize = Vector2(*mapSize);


	for (int i = 0; i < 4; i++)
		m_targets[i] = nullptr;
}

void PlayerCameraController::update(float dt) {

	updatePosition(dt);

	setCameraPosition(m_position + m_back * (m_cameraZOffset + m_extraZCurrent) + m_up * m_cameraYOffset);
	setCameralookAt(m_target);
}

void PlayerCameraController::setPosition(Vector3 pos)
{
	m_position = pos;
}

void PlayerCameraController::setTarget(Vector3 pos)
{
	m_target = pos;
}

void PlayerCameraController::setOffset(Vector3 offset)
{
	m_cameraZOffset = offset.z;
}

void PlayerCameraController::setMoveSpeed(float speed)
{
	m_moveSpeed = speed;
}

void PlayerCameraController::setFollowSpeed(float speed)
{
	m_followSpeed = speed;
}

void PlayerCameraController::setTargets(Object * focusObject1, Object * focusObject2, Object * focusObject3, Object * focusObject4)
{
	m_targets[0] = focusObject1;
	m_targets[1] = focusObject2;
	m_targets[2] = focusObject3;
	m_targets[3] = focusObject4;
}

Vector3 PlayerCameraController::getPosition()
{
	return m_position;
}

Vector3 PlayerCameraController::getTarget()
{
	return m_target;
}

void PlayerCameraController::setMoving(bool moving)
{
	m_moving = moving;
}

void PlayerCameraController::setUseExtraZ(bool use) {
	m_useExtraZ = use;
}

void PlayerCameraController::updatePosition(float dt)
{
	Vector3 newTarget(0, 0, 0);
	float lengthDiff = 0.0f;
	int nr = 0;
	float maxXDst = 0;
	float maxYDst = 0;
	for (int i = 0; i < 4; i++) {
		if (m_targets[i]) {
			newTarget += m_targets[i]->getTransform().getTranslation();
			nr++;
			for (int u = 0; u < 4; u++) {
				if (m_targets[u])
				{
					float xDst = abs(m_targets[i]->getTransform().getTranslation().x - m_targets[u]->getTransform().getTranslation().x);
					if (xDst > maxXDst)
						maxXDst = xDst;
					float yDst = abs(m_targets[i]->getTransform().getTranslation().y - m_targets[u]->getTransform().getTranslation().y);
					if (yDst > maxYDst)
						maxYDst = yDst;
				}
			}

			//extra movement / length if aiming outside of camera ? 

		}
	}



	static float r = 0.04f;
	static float r2 = 0.1f;
	static float z = -1.6f;
	static float t = 18.0f;
	static float t2 = 19.0f;

	//Calculate extra z
	if (m_useExtraZ) {

		//float horExtraZ = m_extraZ;
		//float vertExtraZ = m_extraZ;

		// TODO: Get MaxL for x and y, if x > y use horExtraZ otherwise vertExtraZ

		//if (maxXDst > maxYDst)
			//m_extraZ = (sin(r*Utils::clamp(maxXDst, 0.f, 60.f) + z) + 1) * t;
		//else
			//m_extraZ = (sin(r2*Utils::clamp(maxYDst, 0.f, 30.f) + z) + 1) * t2;


		float addedDst = maxXDst + maxYDst;
		float xFac, yFac;
		if (addedDst != 0.f) {
			xFac = maxXDst / addedDst;
			yFac = maxYDst / addedDst;
		}
		else {
			xFac = 0.5f;
			yFac = 0.5f;
		}
		m_extraZTarget = (sin(r*Utils::clamp(maxXDst, 0.f, 78.f) + z) + 1) * t * xFac + (sin(r2*Utils::clamp(maxYDst, 0.f, 41.f) + z) + 1) * t2 * yFac;
	}


	if (nr > 0) {
		newTarget /= float(nr);
		Vector3 moveVec = newTarget - m_target;
		
		//if (moveVec.LengthSquared() > 0.1f)
		//{
		//	moveVec.Normalize();
		//}
		m_target += moveVec * dt * m_followSpeed;
	}


	if (m_lockToMap) {
		const PerspectiveCamera* cam = dynamic_cast<const PerspectiveCamera*>(getCamera());
		if (!cam) {
			Logger::Error("CAMERA IS NOT PERSPECTIVE, MAYDAY");
		}
		else {

			float aspectRatio = 1.f / cam->getAspectRatio();
			float VFOVRad = cam->getFOV();
			float halfVFOVRad = VFOVRad / 2.f;
			float halfHFOVRad = atan(tan(VFOVRad / 2) / aspectRatio);

			// Clamp to map borders
			float maxMapSize = (m_mapSize.x < m_mapSize.y) ? m_mapSize.x : m_mapSize.y;
			float maxZ = (maxMapSize / 2.f) / tan(halfVFOVRad);
			if ((m_extraZTarget + m_cameraZOffset) > maxZ) {
				m_extraZTarget = maxZ - m_cameraZOffset;
			}
			m_extraZCurrent += (m_extraZTarget - m_extraZCurrent) * dt * m_followSpeed;


			float camZ = -getCameraPosition().z - 1;
			Vector3 min = Vector3(camZ * tan(halfHFOVRad), camZ * tan( halfVFOVRad ), -100000000000.f);
			Vector3 max = Vector3(m_mapSize.x - min.x, m_mapSize.y - min.y, 1000000000.f);
			//if (min.x < max.x)
				//m_target.x = Utils::clamp(m_target.x, min.x, max.x);
			m_target.x = (m_target.x < min.x) ? min.x : m_target.x;
			m_target.y = (m_target.y < min.y) ? min.y : m_target.y;

			//if (min.y < max.y)
				//m_target.y = Utils::clamp(m_target.y, min.y, max.y);

			//m_target.Clamp(min, max);

			//Logger::log(std::to_string(m_extraZ));
			//Logger::log(Utils::vec3ToStr(m_target));

		}

	}


	
	if (m_moving) {
		Vector3 diff = m_target - m_position;
		m_position += diff * dt * m_moveSpeed;
	}

}




