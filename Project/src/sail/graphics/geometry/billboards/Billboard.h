#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include "../Transform.h"

class Billboard{
public:
	Billboard(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& velocity);
	//Billboard(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Color, const DirectX::SimpleMath::Vector3& velocity);

	~Billboard();

	void update(const float dt);

	Transform& getTransform();

private:
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_velocity;
	DirectX::SimpleMath::Color m_color;

	Transform m_transform;

};