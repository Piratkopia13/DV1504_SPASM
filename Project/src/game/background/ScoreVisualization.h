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
	unsigned int m_blocksPerTeam;

	std::vector<std::vector<std::unique_ptr<Block>>> m_scoreBlocks;
	std::vector<std::vector<DirectX::SimpleMath::Vector3>> m_targetPositions;
	std::vector<std::vector<float>> m_accelerations;
	std::vector<std::vector<DirectX::SimpleMath::Vector3>> m_velocities;
	float m_animationClock;

	Model* m_blockModel;
};