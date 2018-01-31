#pragma once

#include "Projectile.h"
#include "../../sail/Sail.h"
#include "common/Object.h"
#include "../level/Grid.h"

class Hook : public Object{
private:
	std::unique_ptr<FbxModel> m_hookModel;
	bool m_triggerHeld;
	Grid* m_levelGrid;
	float m_distance;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_direction;

public:
	Hook(Model *drawModel, Grid* levelGrid);
	~Hook();

	void update(float dt, DirectX::SimpleMath::Vector3 position);
	void triggerPull(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 direction);
	void triggerRelease();
	void draw();

};