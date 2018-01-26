#include "Projectile.h"

Projectile::Projectile(Model *drawModel, DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 velocity, float damage, float mass) : Moveable(mass) {
	m_drawModel = drawModel;
	setVelocity(velocity);
	setPosition(position);
	m_damage = damage;
}

Projectile::~Projectile() {

}

void Projectile::draw() {
	m_drawModel->setTransform(&getTransform());
	m_drawModel->draw();
}