#pragma once

#include "../common/Object.h"

class Moveable : public Object {
public:
	Moveable();
	Moveable(const float mass);
	
	virtual ~Moveable();

	void move(const float dt);
	void setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity);
	virtual void draw() = 0;

private:

	float m_mass;
	DirectX::SimpleMath::Vector3 m_velocity;
	DirectX::SimpleMath::Vector3 m_acceleration;

};
