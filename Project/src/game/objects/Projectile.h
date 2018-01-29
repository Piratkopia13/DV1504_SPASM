#pragma once

#include "common\Moveable.h"

class Projectile : public Moveable {
private:
	float m_damage;
	int m_team;

public:
	Projectile(const DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3 velocity, float damage, int team, float mass);
	virtual ~Projectile();

	int getTeam() const;
	float getDamage() const;

	virtual void draw();
};