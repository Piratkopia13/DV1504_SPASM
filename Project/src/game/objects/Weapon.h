#pragma once

#include "common\Moveable.h"
#include "../ProjectileHandler.h"

class Weapon : public Moveable {
private:
	bool m_held;
	int m_team;

public:
	Weapon();
	Weapon(Model *drawModel, int team);
	virtual ~Weapon();

	const bool getHeld() const;

	void setHeld(bool held);
	void fire(DirectX::SimpleMath::Vector3 direction);
	void update(float dt);
	virtual void draw();
};