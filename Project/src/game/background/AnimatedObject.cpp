#include "AnimatedObject.h"

AnimatedObject::AnimatedObject(Model *drawModel, DirectX::SimpleMath::Vector3 startPos) {
	Object::setModel(drawModel);
	Object::setPosition(startPos);
	
	m_atTargetPos = true;
	m_atTargetRot = true;
}

AnimatedObject::~AnimatedObject() {

}

void AnimatedObject::update(float dt) {
	/*POS*/
	if (!m_atTargetPos) {
		float sP = Utils::smootherstep(0.f, 1.f, m_currTPos / m_timeToAnimPos);
		Object::getTransform().setTranslation(lerp(m_startPos, m_targetPos, sP));

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
		Object::getTransform().setRotations(toRotate);

		m_currTRot += dt;

		if (m_currTRot >= m_timeToAnimRot) {
			m_currTRot = m_timeToAnimRot;
			m_atTargetRot = true;
		}

	}
}

void AnimatedObject::draw() {
	Object::getModel()->setTransform(&getTransform());
	Object::getModel()->getMaterial()->setColor(Object::getLightColor());
	Object::getModel()->draw();
}


void AnimatedObject::setTargetPos(DirectX::SimpleMath::Vector3 & target, const float timeToAnimate) {
	m_startPos = Object::getTransform().getTranslation();
	m_timeToAnimPos = timeToAnimate;
	m_currTPos = 0.1f;
	m_targetPos = target;
	m_atTargetPos = false;
}

void AnimatedObject::setTargetRot(DirectX::SimpleMath::Vector3 & target, const float timeToAnimate) {
	m_startRot = Object::getTransform().getRotations();
	m_timeToAnimRot = timeToAnimate;
	m_currTRot = 0.1f;
	m_targetRot = target;
	m_atTargetRot = false;
}

void AnimatedObject::setVelocity(DirectX::SimpleMath::Vector3 & vel) {
	m_vel = vel;
}

void AnimatedObject::setAcceleration(DirectX::SimpleMath::Vector3 & acc) {
	m_acc = acc;
}


DirectX::SimpleMath::Vector3 AnimatedObject::getTargetPos() const {
	return m_targetPos;
}

DirectX::SimpleMath::Vector3 AnimatedObject::getTargetRot() const {
	return m_targetRot;
}

DirectX::SimpleMath::Vector3 AnimatedObject::getVelocity() const {
	return m_vel;
}

DirectX::SimpleMath::Vector3 AnimatedObject::getAcceleration() const {
	return m_acc;
}

bool AnimatedObject::atTargetPos() const {
	return m_atTargetPos;
}

bool AnimatedObject::atTargetRot() const {
	return m_atTargetRot;
}


/* PRIVATE */
DirectX::SimpleMath::Vector4 AnimatedObject::vec3ToQuat(DirectX::SimpleMath::Vector3 vec) {
	float s = vec.Length();
	if (s == 0.f)
		s = 1.f;
	return DirectX::SimpleMath::Vector4(vec.x / s, vec.y / s, vec.z / s, s);
}

DirectX::SimpleMath::Vector3 AnimatedObject::quatToVec3(DirectX::SimpleMath::Vector4 quat) {
	if(quat.w > 0.f)
		return DirectX::SimpleMath::Vector3(quat.x * quat.w, quat.y * quat.w, quat.z * quat.w);

	return DirectX::SimpleMath::Vector3(quat.x, quat.y, quat.z);
}

DirectX::SimpleMath::Vector3 AnimatedObject::nlerp(DirectX::SimpleMath::Vector3 startPos, DirectX::SimpleMath::Vector3 targetPos, float t) {
	DirectX::SimpleMath::Vector3 lerp = DirectX::XMVectorLerp(startPos, targetPos, t);
	lerp.Normalize();
	return lerp;
}

DirectX::SimpleMath::Vector3 AnimatedObject::lerp(DirectX::SimpleMath::Vector3 startPos, DirectX::SimpleMath::Vector3 targetPos, float t) {
	return DirectX::XMVectorLerp(startPos, targetPos, t);
}

DirectX::SimpleMath::Vector3 AnimatedObject::slerp(DirectX::SimpleMath::Vector3 startRot, DirectX::SimpleMath::Vector3 targetRot, float t) {
	return quatToVec3(DirectX::XMQuaternionSlerp(vec3ToQuat(startRot), vec3ToQuat(targetRot), t));
}