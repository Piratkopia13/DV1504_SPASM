#pragma once

#include "common\Moveable.h"
#include "../ProjectileHandler.h"
#include "Upgrade.h"

class Weapon : public Moveable {
public:
	Weapon();
	Weapon(Model *drawModel, ProjectileHandler* projHandler, int team);
	virtual ~Weapon();

	void update(float dt, DirectX::SimpleMath::Vector3 direction);

	void setHeld(bool held);
	const bool getHeld() const;

	void addUpgrade(Upgrade* upgrade);
	void triggerPull();
	void triggerRelease();

	ProjectileHandler& getProjectileHandler();


	virtual void draw();
private:
	ProjectileHandler * m_projectileHandler;
	bool m_held;
	int m_team;

	bool m_triggerHeld;
	float m_timeSinceFire;

	Upgrade * m_upgrade;

private:

	void fire(DirectX::SimpleMath::Vector3 direction);

};