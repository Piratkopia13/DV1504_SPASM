#include "Projectile.h"

Projectile::Projectile(const DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3 velocity, float damage, int team) : Moveable() {
	setVelocity(DirectX::SimpleMath::Vector3(velocity));
	setPosition(DirectX::SimpleMath::Vector3(position));
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
	m_Model->setTransform(&getTransform());
	this->getModel()->getMaterial()->setColor(this->lightColor);
	m_Model->draw();
}