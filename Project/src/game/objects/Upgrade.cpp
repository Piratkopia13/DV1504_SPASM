#include "Upgrade.h"

using namespace DirectX::SimpleMath;
Upgrade::Upgrade(){
	m_auto = AutoFire();
	m_knockback = ProjectileKnockback();
	m_activeUpgrades = 0;

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
	m_color = Vector4(0, 0, 0, 0);
	m_activeUpgrades = 0;
	if (m_auto.active) {
		m_auto.update(dt);
		m_color += m_auto.color;
		m_activeUpgrades += m_auto.instances;
	}
	if (m_knockback.active) {
		m_knockback.update(dt);
		m_color += m_knockback.color;
		m_activeUpgrades += m_knockback.instances;
	}
	if (m_damage.active) {
		m_damage.update(dt);
		m_color += m_damage.color;
		m_activeUpgrades += m_damage.instances;
	}
	if (m_extraProj.active) {
		m_extraProj.update(dt);
		m_color += m_extraProj.color;
		m_activeUpgrades += m_extraProj.instances;
	}
	if (m_grav.active) {
		m_grav.update(dt);
		m_color += m_grav.color;
		m_activeUpgrades += m_grav.instances;
	}
	if (m_color.w >= 1) {
		m_color *= (1.f / m_color.w) * min(m_activeUpgrades, 5);
		//m_color = Vector4(1, 1, 1, 1) * min(m_activeUpgrades, 5);
	}
	else if (m_color.w == 0)
		m_color = Vector4(1, 0, 0, 1);
}

void Upgrade::combine(const Upgrade & other) {
	m_activeUpgrades++;
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

Vector4 Upgrade::getColor() const {
	return m_color;
}

int Upgrade::getActiveUprades() const {
	return m_activeUpgrades;
}