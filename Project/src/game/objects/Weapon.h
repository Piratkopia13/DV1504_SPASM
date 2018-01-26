#pragma once

#include "common\Moveable.h"
#include "../ProjectileHandler.h"

class Weapon : public Moveable {
private:
	Model * m_drawModel;
	bool m_held;
	int m_team;

public:
	Weapon();
	Weapon(Model *drawModel, int team);
	virtual ~Weapon();

	const bool getHeld() const;

	void setHeld(bool held);
	void setModel(Model* newModel);
	void fire(DirectX::SimpleMath::Vector3 direction);
	void update(float dt);
	virtual void draw();
};