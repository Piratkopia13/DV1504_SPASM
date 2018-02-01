#pragma once
#include "../../sail/Sail.h"


struct CameraStuff {
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 target;


};

struct StartGameStuff{
	int used;
	int players;
	int ports[4];
	int teams[4];
	DirectX::SimpleMath::Vector4 color[4];
	int level;

	


};

struct Vibration {
	float players[4];
};



static CameraStuff gameCamera = { DirectX::SimpleMath::Vector3(0,0,0), DirectX::SimpleMath::Vector3(0,0,0) };
static StartGameStuff gameVariables =
{
	0,
	0,
	{-1, -1, -1, -1},
	{-1, -1, -1, -1 },
	{
		DirectX::SimpleMath::Vector4(1,1,1,1),
		DirectX::SimpleMath::Vector4(1,1,1,1),
		DirectX::SimpleMath::Vector4(1,1,1,1),
		DirectX::SimpleMath::Vector4(1,1,1,1)
	},
	0
};