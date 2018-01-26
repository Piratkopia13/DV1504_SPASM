#pragma once

#include "common\Moveable.h"

class Projectile : public Moveable {
private:
	Model * m_drawModel;
	float m_damage;

public:
	Projectile(Model *drawModel, DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 velocity, float damage, float mass);
	virtual ~Projectile();

	virtual void draw();
};