#include "PlayerCameraController.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

PlayerCameraController::PlayerCameraController(Camera* cam)
	: CameraController(cam)
{
	this->m_cameraZOffset = 5.0f;
	this->m_cameraYOffset = 1.0f;
	this->extraZ = 0.0f;

	
	this->followSpeed = 40;
	this->moveSpeed = 36;


	this->position = Vector3(0, 0, 0);
	this->target = Vector3(0, 0, 0);
	this->back = Vector3(0, 0, -1);
	this->up = Vector3(0, 1, 0);

	for (int i = 0; i < 4; i++)
		this->targets[i] = nullptr;
}

void PlayerCameraController::update(float dt) {
	
	this->updatePosition(dt);

	setCameraPosition(this->position + this->back * (m_cameraZOffset+this->extraZ) + this->up * m_cameraYOffset);
	setCameralookAt(this->target);
}

void PlayerCameraController::setTargets(Object * focusObject1, Object * focusObject2, Object * focusObject3, Object * focusObject4)
{
	this->targets[0] = focusObject1;
	this->targets[1] = focusObject2;
	this->targets[2] = focusObject3;
	this->targets[3] = focusObject4;
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


	


	this->extraZ = maxL * 0.02f + maxL;
	if (nr > 0) {
		newTarget /= float(nr); 
		Vector3 moveVec = newTarget - this->target;
		
		if (moveVec.LengthSquared() > 0.1f)
		{
			moveVec.Normalize();
		}
		this->target += moveVec * dt * followSpeed;
	}
	
	


	Vector3 diff = this->target - this->position;
	
	this->position += diff * dt;
}




