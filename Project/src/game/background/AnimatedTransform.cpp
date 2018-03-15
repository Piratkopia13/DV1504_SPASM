#include "AnimatedTransform.h"

AnimatedTransform::AnimatedTransform(const DirectX::SimpleMath::Vector3& startPos) {
	Transform::setTranslation(startPos);
	
	m_atTargetPos = true;
	m_atTargetRot = true;
}

AnimatedTransform::~AnimatedTransform() {

}

void AnimatedTransform::update(float dt) {
	/*POS*/
	if (!m_atTargetPos) {
		float sP = Utils::smootherstep(0.f, 1.f, m_currTPos / m_timeToAnimPos);
		Transform::setTranslation(lerp(m_startPos, m_targetPos, sP));

		m_currTPos += dt;

		if (m_currTPos >= m_timeToAnimPos) {
			m_currTPos = m_timeToAnimPos;
			m_atTargetPos = true;
		}

	}

	/*ROT*/
	if (!m_atTargetRot) {
		float sR = Utils::smootherstep(0.f, 1.f, m_currTRot / m_timeToAnimRot);
		DirectX::SimpleMath::Vector3 toRotate = slerp(m_startRot, m_targetRot, sR);
		Transform::setRotations(toRotate);

		m_currTRot += dt;

		if (m_currTRot >= m_timeToAnimRot) {
			m_currTRot = m_timeToAnimRot;
			m_atTargetRot = true;
		}

	}
}


void AnimatedTransform::setTargetPos(const DirectX::SimpleMath::Vector3& target, const float timeToAnimate) {
	m_startPos = Transform::getTranslation();
	m_timeToAnimPos = timeToAnimate;
	m_currTPos = 0.1f;
	m_targetPos = target;
	m_atTargetPos = false;
}

void AnimatedTransform::setTargetRot(const DirectX::SimpleMath::Vector3& target, const float timeToAnimate) {
	m_startRot = Transform::getRotations();
	m_timeToAnimRot = timeToAnimate;
	m_currTRot = 0.1f;
	m_targetRot = target;
	m_atTargetRot = false;
}

void AnimatedTransform::setVelocity(const DirectX::SimpleMath::Vector3& vel) {
	m_vel = vel;
}

void AnimatedTransform::setStartPos(const DirectX::SimpleMath::Vector3& startPos) {
	m_startPos = startPos;
}

void AnimatedTransform::setAcceleration(const DirectX::SimpleMath::Vector3& acc) {
	m_acc = acc;
}


DirectX::SimpleMath::Vector3 AnimatedTransform::getTargetPos() const {
	return m_targetPos;
}

DirectX::SimpleMath::Vector3 AnimatedTransform::getTargetRot() const {
	return m_targetRot;
}

DirectX::SimpleMath::Vector3 AnimatedTransform::getVelocity() const {
	return m_vel;
}

DirectX::SimpleMath::Vector3 AnimatedTransform::getAcceleration() const {
	return m_acc;
}

bool AnimatedTransform::atTargetPos() const {
	return m_atTargetPos;
}

bool AnimatedTransform::atTargetRot() const {
	return m_atTargetRot;
}


/* PRIVATE */
DirectX::SimpleMath::Vector4 AnimatedTransform::vec3ToQuat(const DirectX::SimpleMath::Vector3& vec) const {
	float s = vec.Length();
	if (s == 0.f)
		s = 1.f;
	return DirectX::SimpleMath::Vector4(vec.x / s, vec.y / s, vec.z / s, s);
}

DirectX::SimpleMath::Vector3 AnimatedTransform::quatToVec3(const DirectX::SimpleMath::Vector4& quat) const {
	if(quat.w > 0.f)
		return DirectX::SimpleMath::Vector3(quat.x * quat.w, quat.y * quat.w, quat.z * quat.w);

	return DirectX::SimpleMath::Vector3(quat.x, quat.y, quat.z);
}

DirectX::SimpleMath::Vector3 AnimatedTransform::nlerp(const DirectX::SimpleMath::Vector3& startPos, const DirectX::SimpleMath::Vector3& targetPos, float t) const {
	DirectX::SimpleMath::Vector3 lerp = DirectX::XMVectorLerp(startPos, targetPos, t);
	lerp.Normalize();
	return lerp;
}

DirectX::SimpleMath::Vector3 AnimatedTransform::lerp(const DirectX::SimpleMath::Vector3& startPos, const DirectX::SimpleMath::Vector3& targetPos, float t) const {
	return DirectX::XMVectorLerp(startPos, targetPos, t);
}

DirectX::SimpleMath::Vector3 AnimatedTransform::slerp(const DirectX::SimpleMath::Vector3& startRot, const DirectX::SimpleMath::Vector3& targetRot, float t) const {
	return quatToVec3(DirectX::XMQuaternionSlerp(vec3ToQuat(startRot), vec3ToQuat(targetRot), t));
}