#pragma once

#include "common\Moveable.h"
#include "../ProjectileHandler.h"

class Weapon : public Moveable {
private:
	ProjectileHandler * m_projectileHandler;
	bool m_held;
	int m_team;


	bool triggerHeld;
	float timeSinceFire;

public:
	bool automatic;
	float cooldownTime;
	Weapon();
	Weapon(Model *drawModel, ProjectileHandler* projHandler, int team);
	virtual ~Weapon();

	const bool getHeld() const;

	void setHeld(bool held);
	
	void triggerPull();
	void triggerRelease();
	void fire(DirectX::SimpleMath::Vector3 direction);



	void update(float dt, DirectX::SimpleMath::Vector3 direction);
	virtual void draw();
};