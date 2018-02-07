#include "Projectile.h"

Projectile::Projectile(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& velocity, float damage, int team)
	: Moveable()
{
	setVelocity(velocity);
	setPosition(position);
	m_damage = damage;
	m_team = team;
}

Projectile::~Projectile() {

}

int Projectile::getTeam() const {
	return m_team;
}

float Projectile::getDamage() const {
	return m_damage;
}

void Projectile::draw() {
	model->setTransform(&getTransform());
	this->getModel()->getMaterial()->setColor(this->lightColor);
	model->draw();
}