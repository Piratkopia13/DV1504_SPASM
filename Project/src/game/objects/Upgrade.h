#pragma once
#include "common/object.h"

class Upgrade{
public:
	Upgrade();
	Upgrade(int type);
	virtual ~Upgrade(); 

	void update(float dt);
	virtual void combine(const Upgrade& other);

	

	bool autoActive();
	float autoRate();
	float autoTime();

	bool speedActive();
	float speedRate();
	float speedTime();

	bool damageActive();
	float damageMultiplier();
	float damageTime();

	bool multiActive();
	float multiCount();
	float multiTime();

	bool gravActive();
	float gravTime();


	enum UpgradeType {
		RANDOM = -1,
		NONE,
		AUTO_FIRE,
		PROJECTILE_SPEED,
		EXTRA_DAMAGE,
		EXTRA_PROJECTILES,
		NO_GRAVITY,
		MAX = NO_GRAVITY
	};


private:


	struct UpgradeTimer {	
		bool active;
		float remaining;
		float percent;
		float cap;
		float div;
		
		float getPercent() {
			return 1-percent;
		}
		void update(float dt) {
			remaining -= dt;
			percent = remaining * div;
		}
		void setCap(float time) {
			remaining = time;
			cap = time;
			div = 1 / cap;
		}
		void addTime(float time) {
			remaining = min(remaining+time, cap);
			percent = remaining * div;
		}
		void reset() {
			setCap(cap);
		}
	
	};

	struct AutoFire {
	public:
		bool active;
		float fireRate;
		float baseRate;
		UpgradeTimer time;

		void update(float dt) {
			if (active) {
				time.update(dt);
				if (time.remaining <= 0) {
					reset();
				}
			}
		}
		void operator+=(const AutoFire& other) {
			if (other.active) {
				if(active)
					fireRate *= 0.5;
				time.addTime(other.time.remaining);
				active = true;
			}
		}
		void reset() {
			time.reset();
			active = false;
			fireRate = baseRate;
		}
		AutoFire() {
			active = false;
			fireRate = 0.3;
			baseRate = 0.3;
			time.setCap(10);
		}
		AutoFire(float _rate, float _time) : AutoFire(){
			fireRate = _rate;
			baseRate = _rate;
			time.setCap(_time);
		}
	};
	
	struct ProjectileSpeed {
	public:
		bool active;
		float speed;
		float baseSpeed;
		UpgradeTimer time;

		void update(float dt) {
			if (active) {
				time.update(dt);
				if (time.remaining <= 0) {
					reset();
				}
			}

			
		}
		void operator+=(const ProjectileSpeed& other) {
			if (other.active) {
				if (active)
					speed += other.speed;
				time.addTime(other.time.remaining);
				active = true;
			}
		}
		void reset() {
			time.reset();
			active = false;
			speed = baseSpeed;
		}
		ProjectileSpeed() {
			active = false;
			speed = 2;
			baseSpeed = 2;
			time.setCap(10);
			time.reset();
		}
		ProjectileSpeed(float _speed, float _time) : ProjectileSpeed() {
			speed = _speed;
			baseSpeed = _speed;
			time.setCap(_time);
		}
	};
	
	struct ExtraDamage {

		bool active;
		float multiplier;
		float baseMultiplier;
		UpgradeTimer time;

		void update(float dt) {
			if (active) {
				time.update(dt);
				if (time.remaining <= 0) {
					reset();
				}
			}
		}
		void operator+=(const ExtraDamage& other) {
			if (other.active) {
				if (active)
					multiplier *= 2;
				time.addTime(other.time.remaining);
				active = true;
			}
		}
		void reset() {
			time.reset();
			active = false;
			multiplier = baseMultiplier;
		}
		ExtraDamage() {
			active = false;
			multiplier = 2;
			baseMultiplier = 2;
			time.setCap(10);
		}
		ExtraDamage(float multi, float _time) : ExtraDamage() {
			multiplier = multi;
			baseMultiplier = multi;
			time.setCap(_time);
		}
	};
	
	struct ExtraProjectiles {

		bool active;
		unsigned int nr;
		unsigned int baseNr;
		UpgradeTimer time;

		void update(float dt) {
			if (active) {
				time.update(dt);
				if (time.remaining <= 0) {
					reset();
				}
			}
		}
		void operator+=(const ExtraProjectiles& other) {
			if (other.active) {
				if (active)
					nr += 2;
				time.addTime(other.time.remaining);
				active = true;
			}
		}
		void reset() {
			time.reset();
			active = false;
			nr = baseNr;
		}
		ExtraProjectiles() {
			active = false;
			nr = 2;
			baseNr = 2;
			time.setCap(10);
		}
		ExtraProjectiles(float multi, float _time) : ExtraProjectiles() {
			nr = multi;
			baseNr = multi;
			time.setCap(_time);
		}
	};
	
	struct NoGravity {
		bool active;
		UpgradeTimer time;

		void update(float dt) {
			if (active) {
				time.update(dt);
				if (time.remaining <= 0) {
					reset();
				}
			}
		}
		void operator+=(const NoGravity& other) {
			if (other.active) {
				
				time.addTime(other.time.remaining);
				active = true;
			}
		}
		void reset() {
			time.reset();
			active = false;
		}
		NoGravity() {
			active = false;			
			time.setCap(10);
		}
		NoGravity(float multi, float _time) : NoGravity() {
			time.setCap(_time);
		}
	};
	
	struct Explosion {
		bool active;
		float radius;
		bool rocket;
		UpgradeTimer time;
	};
	
	struct Bouncy {
		bool active;
		UpgradeTimer time;
	};

	AutoFire m_auto;
	ProjectileSpeed m_speed;
	ExtraDamage m_damage;
	ExtraProjectiles m_extraProj;
	NoGravity m_grav;
	Explosion m_explosion;
	Bouncy m_bouncy;

};





/*

struct Name {

		bool active;
		unsigned int stat;
		unsigned int baseStat;
		UpgradeTimer time;

		void update(float dt) {
			if (active) {
				time.update(dt);
				if (time.remaining <= 0) {
					reset();
				}
			}
		}
		void operator+=(const AutoFire& other) {
			if (other.active) {
				if (active)
					stat += 2;
				time.addTime(other.time.remaining);
				active = true;
			}
		}
		void reset() {
			time.reset();
			active = false;
			stat = baseStat;
		}
		Name() {
			active = false;
			stat = 2;
			baseStat = 2;
			time.setCap(10);
		}
		Name(float multi, float _time) : Name() {
			stat = multi;
			baseStat = multi;
			time.setCap(_time);
		}
	};



*/