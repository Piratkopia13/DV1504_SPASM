#include "Upgrade.h"

using namespace DirectX::SimpleMath;
Upgrade::Upgrade(){
	m_auto = AutoFire();
	m_knockback = ProjectileKnockback();

}
Upgrade::Upgrade(int type) : Upgrade(){
	if (type == AUTO_FIRE) {
		m_auto.active = true;
	}
	if (type == PROJECTILE_KNOCKBACK) {
		m_knockback.active = true;
	}
	if (type == EXTRA_DAMAGE) {
		m_damage.active = true;
	}
	if (type == EXTRA_PROJECTILES) {
		m_extraProj.active = true;
	}
	if (type == NO_GRAVITY) {
		m_grav.active = true;
	}
}
Upgrade::~Upgrade() {

}

void Upgrade::update(float dt) {
	if (m_auto.active) {
		m_auto.update(dt);
	}
	if (m_knockback.active) {
		m_knockback.update(dt);
	}
	if (m_damage.active) {
		m_damage.update(dt);
	}
	if (m_extraProj.active) {
		m_extraProj.update(dt);
	}
	if (m_grav.active) {
		m_grav.update(dt);
	}
}

void Upgrade::combine(const Upgrade & other) {
	m_auto += other.m_auto;
	m_knockback += other.m_knockback;
	m_damage += other.m_damage;
	m_extraProj += other.m_extraProj;
	m_grav += other.m_grav;
	/*m_explosion += other.m_explosion;
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

bool Upgrade::knockbackActive()
{
	return m_knockback.active;
}

float Upgrade::knockbackRate()
{
	return m_knockback.amount;
}

float Upgrade::knockbackTime()
{
	return m_knockback.time.getPercent();
}

bool Upgrade::damageActive()
{
	return m_damage.active;
}

float Upgrade::damageMultiplier()
{
	return m_damage.multiplier;
}

float Upgrade::damageTime()
{
	return m_damage.time.getPercent();
}

bool Upgrade::multiActive()
{
	return m_extraProj.active;
}

unsigned int Upgrade::multiCount()
{
	return m_extraProj.nr;
}

float Upgrade::multiTime()
{
	return m_extraProj.time.getPercent();
}

bool Upgrade::gravActive()
{
	return m_grav.active;
}

float Upgrade::gravTime()
{
	return m_grav.time.getPercent();
}

