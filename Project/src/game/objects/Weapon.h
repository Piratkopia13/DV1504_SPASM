#pragma once

#include "common\Moveable.h"
#include "../ProjectileHandler.h"
#include "Upgrade.h"

class Character;

class Weapon : public Moveable {
public:
	Weapon();
	Weapon(Model *armModel, Model* laserModel, Model* dotModel, ProjectileHandler* projHandler, ParticleHandler* particleHandler, Character* owner);
	virtual ~Weapon();

	void update(float dt, const DirectX::SimpleMath::Vector3& direction);
	void setHeld(bool held);
	const bool getHeld() const;
	DirectX::SimpleMath::Vector3 getNozzlePos() const;

	void addUpgrade(const Upgrade& upgrade);
	void triggerPull();
	void triggerRelease();
	void fire(const DirectX::SimpleMath::Vector3& direction);


	ProjectileHandler& getProjectileHandler();


	virtual void draw();
private:
	ProjectileHandler* m_projectileHandler;
	ParticleHandler* m_particleHandler;
	Character* m_owner;
	bool m_held;

	bool m_triggerHeld;
	float m_timeSinceFire;
	
	DirectX::SimpleMath::Vector3 m_nozzlePos;

	Upgrade * m_upgrade;

	struct Laser {
		Model* laserModel;
		Model* dotModel;
		Transform laserTransform;
		Transform dotTransform;
	};

	Laser m_laser;

};