#pragma once

#include "../GameInfo.h"
#include "../gamemodes/Gamemode.h"
#include "AnimatedObject.h"

class TimeVisualization : public Object {

public:
	TimeVisualization(Level* level, Gamemode* gamemode);
	~TimeVisualization();

	void update(float dt);
	void draw();

private:
	Gamemode * m_gamemode;
	Level* m_level;

	std::vector<std::unique_ptr<AnimatedObject>> m_timeBlocks;
	std::vector<bool> m_toRemove;
	GameInfo* m_info;
	
	float m_timeLeft;
	float m_maxTime;
	float m_secondsPerBlock;

	bool m_run;

	Model* m_blockModel;

	DirectX::SimpleMath::Vector3 m_middle;

	unsigned int m_numOfBlocks;

};