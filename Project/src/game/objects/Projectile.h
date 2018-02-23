#pragma once

#include "common\Moveable.h"

class Character;
class Projectile : public Moveable {
public:
	Projectile(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& velocity, float damage, Character* owner);
	virtual ~Projectile();

	Character* getOwner() const;
	float getDamage() const;
	float getKnockbackAmount() const;

	virtual void draw();

private:
	float m_damage;
	float m_knockbackAmount;
	int m_team;
	Character* m_owner;

};