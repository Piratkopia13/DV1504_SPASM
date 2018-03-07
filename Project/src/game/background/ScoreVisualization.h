#pragma once

#include <vector>

#include "../GameInfo.h"
#include "../gamemodes/Gamemode.h"
#include "../objects/Block.h"

class ScoreVisualization : public Object {
public:
	ScoreVisualization(Level* level, Gamemode* currentGamemode);
	~ScoreVisualization();

	void update(float dt);
	
	void draw();
private:
	Gamemode* m_currentGamemode;
	Level* m_currentLevel;

	unsigned int m_numberOfTeams;
	std::vector<float> teamScores;

	std::vector<std::unique_ptr<Block>> m_scoreBlocks;
	std::vector<std::unique_ptr<Block>> m_randomBlocks;
	std::vector<DirectX::SimpleMath::Vector3> m_targetPositions;
	std::vector<float> m_accelerations;
	std::vector<DirectX::SimpleMath::Vector3> m_velocities;
	float m_animationClock;

	Model* m_blockModel;
};