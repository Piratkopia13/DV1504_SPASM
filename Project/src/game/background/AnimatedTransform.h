#pragma once

#include "../../sail/Sail.h"
#include "../../sail/graphics/geometry/Transform.h"

class AnimatedTransform : public Transform {

public:
	AnimatedTransform(DirectX::SimpleMath::Vector3 startPos = DirectX::SimpleMath::Vector3::Zero);
	~AnimatedTransform();

	void update(float dt);

	void setTargetPos(DirectX::SimpleMath::Vector3 & target, const float timeToAnimate = 1.f);
	void setTargetRot(DirectX::SimpleMath::Vector3 & target, const float timeToAnimate = 1.f);
	void setVelocity(DirectX::SimpleMath::Vector3 & vel);
	void setAcceleration(DirectX::SimpleMath::Vector3 & acc);
	
	DirectX::SimpleMath::Vector3 getTargetPos() const;
	DirectX::SimpleMath::Vector3 getTargetRot() const;
	DirectX::SimpleMath::Vector3 getVelocity() const;
	DirectX::SimpleMath::Vector3 getAcceleration() const;
	bool atTargetPos() const;
	bool atTargetRot() const;

private:
	DirectX::SimpleMath::Vector4 vec3ToQuat(DirectX::SimpleMath::Vector3 vec);
	DirectX::SimpleMath::Vector3 quatToVec3(DirectX::SimpleMath::Vector4 quat);
	DirectX::SimpleMath::Vector3 nlerp(DirectX::SimpleMath::Vector3 startPos, DirectX::SimpleMath::Vector3 targetPos, float t);
	DirectX::SimpleMath::Vector3 lerp(DirectX::SimpleMath::Vector3 startPos, DirectX::SimpleMath::Vector3 targetPos, float t);
	DirectX::SimpleMath::Vector3 slerp(DirectX::SimpleMath::Vector3 startRot, DirectX::SimpleMath::Vector3 targetRot, float t);

private:
	DirectX::SimpleMath::Vector3 m_startPos;
	DirectX::SimpleMath::Vector3 m_targetPos;
	DirectX::SimpleMath::Vector3 m_startRot;
	DirectX::SimpleMath::Vector3 m_targetRot;
	DirectX::SimpleMath::Vector3 m_acc;
	DirectX::SimpleMath::Vector3 m_vel;
	
	float m_timeToAnimPos;
	float m_currTPos;
	float m_timeToAnimRot;
	float m_currTRot;
	float m_maxVel;
	float m_maxAcc;

	bool m_atTargetPos;
	bool m_atTargetRot;

};