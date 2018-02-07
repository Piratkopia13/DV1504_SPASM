#pragma once

#include "common\Moveable.h"
#include "../ProjectileHandler.h"

class Weapon : public Moveable {
public:

	Weapon();
	Weapon(Model *drawModel, ProjectileHandler* projHandler, int team);
	virtual ~Weapon();

	void update(float dt, const DirectX::SimpleMath::Vector3& direction);
	virtual void draw();

	const bool getHeld() const;
	void setHeld(bool held);

	void triggerPull();
	void triggerRelease();
	void fire(const DirectX::SimpleMath::Vector3& direction);

	ProjectileHandler& getProjectileHandler();

private:
	ProjectileHandler* m_projectileHandler;
	bool m_held;
	int m_team;
	bool m_triggerHeld;
	float m_timeSinceFire;

	bool m_automatic;
	float m_cooldownTime;

};