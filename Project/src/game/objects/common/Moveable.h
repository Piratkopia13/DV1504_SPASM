#pragma once

#include "../common/Object.h"

class Moveable : public Object {
public:
	Moveable();
	virtual ~Moveable();

	void move(const float dt);
	void setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity);

	const DirectX::SimpleMath::Vector3& getVelocity();
 	void setAcceleration(const DirectX::SimpleMath::Vector3 &newAcceleration);
	void addAcceleration(const DirectX::SimpleMath::Vector3& accel );	virtual void draw() = 0;

private:

	DirectX::SimpleMath::Vector3 m_gravity;
	DirectX::SimpleMath::Vector3 m_velocity;
	DirectX::SimpleMath::Vector3 m_acceleration;
	float grav;

};
