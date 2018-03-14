#pragma once

#include "../../sail/Sail.h"
#include "../../sail/graphics/geometry/Transform.h"

class AnimatedTransform : public Transform {

public:
	AnimatedTransform(const DirectX::SimpleMath::Vector3& startPos = DirectX::SimpleMath::Vector3::Zero);
	~AnimatedTransform();

	void update(float dt);

	void setTargetPos(const DirectX::SimpleMath::Vector3& target, const float timeToAnimate = 1.f);
	void setTargetRot(const DirectX::SimpleMath::Vector3& target, const float timeToAnimate = 1.f);
	void setVelocity(const DirectX::SimpleMath::Vector3& vel);
	void setAcceleration(const DirectX::SimpleMath::Vector3& acc);
	
	DirectX::SimpleMath::Vector3 getTargetPos() const;
	DirectX::SimpleMath::Vector3 getTargetRot() const;
	DirectX::SimpleMath::Vector3 getVelocity() const;
	DirectX::SimpleMath::Vector3 getAcceleration() const;
	bool atTargetPos() const;
	bool atTargetRot() const;

private:
	DirectX::SimpleMath::Vector4 vec3ToQuat(const DirectX::SimpleMath::Vector3& vec) const;
	DirectX::SimpleMath::Vector3 quatToVec3(const DirectX::SimpleMath::Vector4& quat) const;
	DirectX::SimpleMath::Vector3 nlerp(const DirectX::SimpleMath::Vector3& startPos, const DirectX::SimpleMath::Vector3& targetPos, float t) const;
	DirectX::SimpleMath::Vector3 lerp(const DirectX::SimpleMath::Vector3& startPos, const DirectX::SimpleMath::Vector3& targetPos, float t) const;
	DirectX::SimpleMath::Vector3 slerp(const DirectX::SimpleMath::Vector3& startRot, const DirectX::SimpleMath::Vector3& targetRot, float t) const;

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