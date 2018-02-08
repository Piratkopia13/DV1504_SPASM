#include "Upgrade.h"

using namespace DirectX::SimpleMath;
Upgrade::Upgrade() : Object() {
	Object::setPosition(Vector3(5, 3, 0));
	m_auto = AutoFire();
	m_speed = ProjectileSpeed();
}
Upgrade::Upgrade(Vector3 pos, Model* model) : Upgrade() {
	Object::setPosition(pos);	
	Object::setModel(model);

	if (pos.x == 0) {
		m_auto = AutoFire();
		m_auto.active = true;
	}
	if (pos.x == 1) {
		m_speed = ProjectileSpeed();
		m_speed.active = true;
	}
}



Upgrade::~Upgrade() {

}

void Upgrade::update(float dt) {
	if (m_auto.active) {
		m_auto.update(dt);
	}
	if (m_speed.active) {
		m_speed.update(dt);
	}

}

void Upgrade::combine(Upgrade & other) {
	m_auto += other.m_auto;
	m_speed += other.m_speed;
	/*m_damage += other.m_damage;
	m_extraProj += other.m_extraProj;
	m_grav += other.m_grav;
	m_explosion += other.m_explosion;
	m_bouncy += other.m_bouncy;*/



}

bool Upgrade::autoActive()
{
	return m_auto.active;
}

float Upgrade::autoRate()
{
	return m_auto.fireRate;
}

float Upgrade::autoTime()
{
	return m_auto.time.getPercent();
}

bool Upgrade::speedActive()
{
	return m_speed.active;
}

float Upgrade::speedRate()
{
	return m_speed.speed;
}

float Upgrade::speedTime()
{
	return m_speed.time.getPercent();
}

void Upgrade::draw() {
	this->m_Model->setTransform(&getTransform());
	this->m_Model->getMaterial()->setColor(lightColor);
	this->m_Model->draw();
}
