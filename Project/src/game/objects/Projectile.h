#pragma once

#include "common\Moveable.h"

class Projectile : public Moveable {
private:
	Model * m_drawModel;
	float m_damage;
	int m_team;

public:
	Projectile(const DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3 velocity, float damage, int team, float mass);
	virtual ~Projectile();

	int getTeam() const;
	float getDamage() const;

	void setModel(Model *drawModel);
	virtual void draw();
};