#pragma once

#include "../common/Object.h"
#include "../../level/Level.h"

class Moveable : public Object {
public:
	Moveable();
	virtual ~Moveable();

	void move(const float dt, bool checkColl = true);
	void setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity);

	const DirectX::SimpleMath::Vector3& getVelocity();
 	void setAcceleration(const DirectX::SimpleMath::Vector3 &newAcceleration);
	void addAcceleration(const DirectX::SimpleMath::Vector3& accel );
	void setCurrentLevel(Level* level);
	void setGrounded(bool grounded);
	bool grounded();
	virtual void draw() = 0;

private:

	DirectX::SimpleMath::Vector3 m_gravity;
	DirectX::SimpleMath::Vector3 m_velocity;
	DirectX::SimpleMath::Vector3 m_acceleration;
	float grav;
	bool m_grounded;

	Level* m_currLevel;
};
