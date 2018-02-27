#pragma once
#include "objects/Upgrade.h"

class UpgradeHandler
{
public:
	class UpgradeSpawn;
	UpgradeHandler();
	~UpgradeHandler();

	void addSpawn(const DirectX::SimpleMath::Vector3& position, int type, float time);
	
	size_t getNrOfSpawners();
	UpgradeHandler::UpgradeSpawn* getSpawn(size_t index);

	void update(float dt);



	class UpgradeSpawn : public Object {
	public:
		UpgradeSpawn();
		UpgradeSpawn(const DirectX::SimpleMath::Vector3& _position, float _cooldown, int type);
		~UpgradeSpawn();

		void update(float dt);
		void draw();
		bool getOnline();

		void respawn();
		const Upgrade take();


	private:
		class UpgradeObject : public Object {
		public:
			UpgradeObject(const DirectX::SimpleMath::Vector3& pos);
			~UpgradeObject();
			void update(float dt);
			void draw();

			

		private:
			DirectX::SimpleMath::Vector3 m_spawnPosition;
			float m_offset[2];
			float m_rotationOffset[2];

		};


		Upgrade m_upgrade;
		UpgradeObject* m_upgradeObject;
		DirectX::SimpleMath::Vector3 m_position;
		bool m_online;
		float m_respawnTime;
		float m_timeLeft;
		int m_type;
		int m_onlineType;
		void reset();
	
	};

private:
	std::vector<UpgradeSpawn*> m_spawns;

	//Spawn Models
	Model* spawnModel = Application::getInstance()->getResourceManager().getFBXModel("fisk").getModel();

	//Upgrade Models
	Model* model1 = Application::getInstance()->getResourceManager().getFBXModel("weapon_upgrade").getModel();
};






