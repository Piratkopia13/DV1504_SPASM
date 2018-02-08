#pragma once
#include "common/object.h"

class Upgrade : public Object {
public:
	Upgrade();
	Upgrade(DirectX::SimpleMath::Vector3 pos, Model* model = nullptr);
	virtual ~Upgrade(); 

	void update(float dt);
	virtual void combine(Upgrade& other);

	

	bool autoActive();
	float autoRate();
	float autoTime();

	bool speedActive();
	float speedRate();
	float speedTime();

	virtual void draw();

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
		void operator+=(AutoFire& other) {
			if (other.active) {
				if(active)
					fireRate *= other.fireRate;
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
			fireRate = 0.5;
			baseRate = 0.5;
			time.setCap(10);
			time.reset();
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
		void operator+=(ProjectileSpeed& other) {
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
		UpgradeTimer time;
	};
	
	struct ExtraProjectiles {
		bool active;
		unsigned int nr;
		UpgradeTimer time;
	};
	
	struct NoGravity {
		bool active;
		UpgradeTimer time;
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

