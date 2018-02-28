#pragma once

#include <vector>

#include "../GameInfo.h"
#include "../gamemodes/Gamemode.h"
#include "../objects/Block.h"

class ScoreVisualization {
public:
	ScoreVisualization(Level* level, Gamemode* currentGamemode);
	~ScoreVisualization();

	void update(float dt);
	
	void draw();
private:
	Gamemode* m_currentGamemode;

	unsigned int m_numberOfTeams;
	std::vector<float> teamScores;

	std::vector<std::unique_ptr<Block>> m_blocks;
	Model* m_blockModel;
};