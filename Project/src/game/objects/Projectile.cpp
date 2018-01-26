#include "Projectile.h"

Projectile::Projectile(const DirectX::SimpleMath::Vector3& position, DirectX::SimpleMath::Vector3 velocity, float damage, int team, float mass) : Moveable(mass) {
	m_drawModel = nullptr;
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

void Projectile::setModel(Model *drawModel) {
	m_drawModel = drawModel;
}

void Projectile::draw() {
	m_drawModel->setTransform(&getTransform());
	m_drawModel->draw();
}