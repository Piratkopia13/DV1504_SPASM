#include "Projectile.h"

Projectile::Projectile(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& velocity, float damage, Character* owner)
	: Moveable()
{
	setVelocity(velocity);
	setPosition(position);
	m_damage = damage;
	m_owner = owner;
}

Projectile::~Projectile() {

}

Character* Projectile::getOwner() const {
	return m_owner;
}

float Projectile::getDamage() const {
	return m_damage;
}

void Projectile::draw() {
	model->setTransform(&getTransform());
	this->getModel()->getMaterial()->setColor(this->lightColor);
	model->draw();
}