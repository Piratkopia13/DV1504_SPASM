#pragma once

#include "Projectile.h"
#include "../../sail/Sail.h"
#include "common/Object.h"

class Hook : public Object{
private:
	bool m_triggerHeld;
	float m_distance;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_direction;

public:
	Hook(Model *drawModel);
	~Hook();

	void update(float dt, const DirectX::SimpleMath::Vector3& position);
	bool triggerPull(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction);
	void triggerRelease();
	void draw();
	float getLength(DirectX::SimpleMath::Vector3 playerPos);

	DirectX::SimpleMath::Vector3 getDirection();

};