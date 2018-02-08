#pragma once

#include "common\Moveable.h"

class Projectile : public Moveable {
public:
	Projectile(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& velocity, float damage, int team);
	virtual ~Projectile();

	int getTeam() const;
	float getDamage() const;

	virtual void draw();

private:
	float m_damage;
	int m_team;

};