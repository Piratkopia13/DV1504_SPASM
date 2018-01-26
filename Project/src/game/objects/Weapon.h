#pragma once

#include "common\Moveable.h"

class Weapon : public Moveable {
private:
	Model * m_drawModel;
	bool m_held;
	int m_team;

public:
	Weapon();
	Weapon(Model *drawModel);
	virtual ~Weapon();

	const bool getHeld() const;

	void setHeld(bool held, int team);
	void setModel(Model* newModel);
	void fire();
	void update(float dt);
	virtual void draw();
};