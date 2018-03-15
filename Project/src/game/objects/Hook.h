#pragma once

#include "Projectile.h"
#include "../../sail/Sail.h"
#include "common/Object.h"
#include "../ParticleHandler.h"

class Hook : public Object{
private:
	bool m_triggerHeld;
	bool m_outOfBounds;
	float m_distance;
	float m_timeHooked;
	float m_hookVelocity;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_direction;

	std::shared_ptr<ParticleEmitter> m_hookEmitter;

public:
	Hook(Model *drawModel, ParticleHandler* particleHandler);
	~Hook();

	bool update(float dt, const DirectX::SimpleMath::Vector3& position);
	void triggerPull(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction);
	void triggerRelease();
	void draw();

	DirectX::SimpleMath::Vector3 getDirection();

};