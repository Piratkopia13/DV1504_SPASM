#pragma once

#include "../common/Object.h"
#include "../../level/Level.h"

class Moveable : public Object {
public:
	Moveable();
	virtual ~Moveable();

	void move(const float dt, bool includeBoundingBoxRotation = true);
	void updateVelocity(const float dt);
	void move(DirectX::SimpleMath::Vector3& toMove, bool includeBoundingBoxRotation = true);
	void setVelocity(const DirectX::SimpleMath::Vector3 &newVelocity);
	void setGravScale(float scale);

	const DirectX::SimpleMath::Vector3& getVelocity();
 	void setAcceleration(const DirectX::SimpleMath::Vector3 &newAcceleration);
	void addAcceleration(const DirectX::SimpleMath::Vector3& accel );
	void setGrounded(bool grounded);
	bool grounded();
	//TEST
	bool testHoles(DirectX::SimpleMath::Vector3 playerPos);
	//--

	virtual void draw() = 0;

private:

	DirectX::SimpleMath::Vector3 m_gravity;
	DirectX::SimpleMath::Vector3 m_velocity;
	DirectX::SimpleMath::Vector3 m_acceleration;
	float m_gravScale;
	bool m_grounded;

};
