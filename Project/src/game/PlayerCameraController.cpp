#include "PlayerCameraController.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PlayerCameraController::PlayerCameraController(Camera* cam)
	: CameraController(cam)
{
	this->m_cameraZOffset = 8.0f;
	this->m_cameraYOffset = 1.0f;
	this->extraZ = 0.0f;

	
	this->followSpeed = 4;
	this->moveSpeed = 3;


	this->position = Vector3(0, 0, 0);
	this->target = Vector3(0, 0, 0);
	this->back = Vector3(0, 0, -1);
	this->up = Vector3(0, 1, 0);

	this->moving = true;

	for (int i = 0; i < 4; i++)
		this->targets[i] = nullptr;
}

void PlayerCameraController::update(float dt) {
	
	this->updatePosition(dt);

	setCameraPosition(this->position + this->back * (m_cameraZOffset+this->extraZ) + this->up * m_cameraYOffset);
	setCameralookAt(this->target);
}

void PlayerCameraController::setPosition(Vector3 pos)
{
	this->position = pos;
}

void PlayerCameraController::setTarget(Vector3 pos)
{
	this->target = pos;
}

void PlayerCameraController::setOffset(Vector3 offset)
{
	this->m_cameraZOffset = offset.z;
}

void PlayerCameraController::setMoveSpeed(float speed)
{
	this->moveSpeed = speed;
}

void PlayerCameraController::setFollowSpeed(float speed)
{
	this->followSpeed = speed;
}

void PlayerCameraController::setTargets(Object * focusObject1, Object * focusObject2, Object * focusObject3, Object * focusObject4)
{
	this->targets[0] = focusObject1;
	this->targets[1] = focusObject2;
	this->targets[2] = focusObject3;
	this->targets[3] = focusObject4;
}

Vector3 PlayerCameraController::getPosition()
{
	return this->position;
}

Vector3 PlayerCameraController::getTarget()
{
	return this->target;
}

void PlayerCameraController::setMoving(bool stat)
{
	this->moving = stat;
}

void PlayerCameraController::updatePosition(float dt)
{
	Vector3 newTarget(0, 0, 0);
	float lengthDiff = 0.0f;
	int nr = 0;
	float maxL = 0;
	for (int i = 0; i < 4; i++) {
		if (this->targets[i]) {
			newTarget += this->targets[i]->getTransform().getTranslation();
			nr++;
			for (int u = 0; u < 4; u++) {
				if (this->targets[u])
				{
					float l = Vector3::Distance(this->targets[i]->getTransform().getTranslation(), this->targets[u]->getTransform().getTranslation());
					if (l > maxL)
						maxL = l;
				}
			}

			//extra movement / length if aiming outside of camera ? 

		}
	}


	
	static float r = 0.08f;
	static float z = -1.9f;
	static float t = 15.0f;

	//Calculate extra length if further than 4 units
	if (maxL < 40.8f && maxL >= 4.115f)
		this->extraZ = (sin(r*maxL + z) + 1) * t;


	if (nr > 0) {
		newTarget /= float(nr); 
		Vector3 moveVec = newTarget - this->target;
		
		//if (moveVec.LengthSquared() > 0.1f)
		//{
		//	moveVec.Normalize();
		//}
		this->target += moveVec * dt * followSpeed;
	}
	
	
	if (this->moving) {
		Vector3 diff = this->target - this->position;
		this->position += diff * dt;
	}

}




