#include "Projectile.h"

Projectile::Projectile(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& velocity, float damage, float knockbackAmount, int team, int owner)
	: Moveable()
{
	setVelocity(velocity);
	setPosition(position);
	m_damage = damage;
	m_team = team;
	m_owner = owner;
	m_knockbackAmount = knockbackAmount;
	setGrounded(false);
}

Projectile::~Projectile() {

}

Character* Projectile::getOwner() const {
	return m_owner;
}

int Projectile::getOwner() const {
	return m_owner;
}

float Projectile::getDamage() const {
	return m_damage;
}

float Projectile::getKnockbackAmount() const {
	return m_knockbackAmount;
}

void Projectile::draw() {
	model->setTransform(&getTransform());
	this->getModel()->getMaterial()->setColor(this->lightColor);
	model->draw();
}