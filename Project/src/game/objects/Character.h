#pragma once
#include "../../sail/Application.h"
#include "common/Moveable.h"
#include "../../sail/graphics/geometry/Model.h"
#include "Weapon.h"
#include "Hook.h"
#include "../ParticleHandler.h"
#include "../collision/CollisionHandler.h"


class Character : public Moveable {
friend CharacterHandler;
public:
	Character();
	Character(Model * bodyModel, Model * lArmModel, Model* headModel, Model* legsModel, int index);
	Character(Model * bodyModel, Model * lArmModel, Model* headModel, Model* legsModel, int index, unsigned int usingController, unsigned int port);
	virtual ~Character();

	void processInput();
	void update(float dt);
	void draw();

	void setController(bool usingController);
	void setControllerPort(unsigned int port);
	void setTeam(unsigned int team);
	void setParticleHandler(ParticleHandler* particleHandler);
	bool isUsingController();
	unsigned int getPort();
	unsigned int getTeam();
	float getHealth();
	float getMaxHealth();
	bool isAlive();
	void damage(float dmg);
	const DirectX::SimpleMath::Vector3& getAimDirection() const;
	void hitByProjectile(const CollisionHandler::CharacterHitResult& hitResult);

	void VibrateController(unsigned int index, float strength = 1, float timeDecreaseMul = 1);

	void setWeapon(Weapon* weapon);
	void setHook(Hook* hook);
	void addUpgrade(const Upgrade& upgrade);

	void setRespawnTime(float time);
	float getRespawnTime() const;
	void living();
	void setRespawnPoint(const DirectX::SimpleMath::Vector3& respawnPoint);
	void dead();
	int getLastAttacker() const;
	int getIndex() const;
	void setLightColor(const DirectX::SimpleMath::Vector4& color);

private:
	Weapon* m_weapon;
	Hook* m_hook;

	ParticleHandler* m_particleHandler;
	std::shared_ptr<ParticleEmitter> m_thrusterEmitter;

	DirectX::SimpleMath::Vector3 m_nextRespawnPoint;
	DirectX::SimpleMath::Vector4 m_lightColor;
	// Ghost vars
	DirectX::SimpleMath::Vector3 m_deathPoint;
	float m_deathInterp;

	struct InputDevice {
		bool controller;
		unsigned int controllerPort;
	};
	struct InputVectors {
		DirectX::SimpleMath::Vector3 movement;
		DirectX::SimpleMath::Vector3 aim;
	};	
	struct Movement {
		bool hooked;
		float speed;
		bool inCover;
		float xDirection;
	};
	struct Health {
		float current;
		float max;
		float regen;
		bool alive;
		float healthPercent;
		float div;
		void setHealth(float hp) {
			if (hp > max)
				hp = max;
			current = hp;
			updatePercent();
		}
		void addHealth(float hp) {
			current += hp;
			if (current > max)
				current = max;
			updatePercent();
		}
		void setMax(float hp) {
			max = hp;
			div = 1 / max;
		}
		void updatePercent() {
			healthPercent = current * div;
		}

	};
	struct ControllerVibration {
		float currentStrength;
		float decreaseMul;
	};


	InputDevice m_inputDevice;
	InputVectors m_input;
	Movement m_movement;
	Health m_playerHealth;
	ControllerVibration m_vibration[2];
	float m_vibFreq;
	float m_vibDeltaAcc;
	float m_respawnTime;
	
	unsigned int m_currentTeam;

	Model * m_leftArm;
	Model * m_head;
	Model * m_legs;
	
	void jump();
	void stopJump();
	void fire();
	void hook();
	void stopHook();

	int m_playerIndex;
	int m_lastAttackerIndex;
	float m_resetAttacker;

	bool updateVibration(float dt);
	float sinDegFromVec(DirectX::SimpleMath::Vector3 vec) {

		return atan2(vec.y, vec.x);
	}
}; 

