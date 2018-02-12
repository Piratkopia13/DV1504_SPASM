#pragma once

#include "../Gamemode.h"
#include "ControlNode.h"
#include "../../level/Grid.h"


class CharacterHandler;
class Block;
class PayloadGamemode : public Gamemode {
public:
	PayloadGamemode(std::vector<Grid::Index>& indices, std::vector<std::vector<Block*>> & blocks, const int levelWidth, const int levelHeight);
	virtual ~PayloadGamemode();

	virtual void update(CharacterHandler* charHandler, float dt);
	virtual void draw();

	void setTeamColor(const int team, const DirectX::SimpleMath::Vector4 & color);

	virtual int checkWin();

private:
	std::vector<std::unique_ptr<ControlNode>> m_controlNodes;

	std::vector<Grid::Index> m_indices;

	int m_numOfNodes;
	int m_radius;
	int m_teamWin;

	float m_scoreToWin;

	std::vector<std::vector<Block*>>& m_blocks;
	int m_levelWidth, m_levelHeight;
};