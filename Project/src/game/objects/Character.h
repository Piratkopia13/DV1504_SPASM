#pragma once
#include "../../sail/Application.h"
#include "common/Moveable.h"
#include "../../sail/graphics/geometry/Model.h"
#include "Weapon.h"
#include "Hook.h"



class Character : public Moveable {
public:
	Character();
	Character(Model * bodyModel, Model * lArmModel, Model* headModel);
	Character(Model * bodyModel, Model * lArmModel, Model* headModel, unsigned int usingController, unsigned int port);
	virtual ~Character();

	void processInput();
	void update(float dt);
	void draw();

	void setController(bool usingController);
	void setControllerPort(unsigned int port);
	void setTeam(unsigned int team);
	bool isUsingController();
	unsigned int getPort();
	unsigned int getTeam();
	float getHealth();
	float getMaxHealth();
	bool isAlive();
	void damage(float dmg);

	void setVibration(unsigned int index, float strength = 1, float time = 1);
	void addVibration(unsigned int index, float strength = 1, float time = 1);

	void setWeapon(Weapon* weapon);
	void setHook(Hook* hook);
	void addUpgrade(const Upgrade& upgrade);


	void living();
	void dead();


private:
	Weapon* m_weapon;
	Hook* m_hook;

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
		float hookLength;
		float speed;
		bool inCover;
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
		float timeLeft;
	};


	InputDevice m_inputDevice;
	InputVectors m_input;
	Movement m_movement;
	Health m_playerHealth;
	ControllerVibration m_vibration[2];
	
	unsigned int m_currentTeam;

	Model * m_leftArm;
	Model * m_head;
	
	void jump();
	void stopJump();
	void fire();
	void hook();
	void stopHook();

	bool updateVibration(float dt);
	float sinDegFromVec(DirectX::SimpleMath:: Vector3 vec) {

		return atan2(vec.y, vec.x);
	}
}; 

