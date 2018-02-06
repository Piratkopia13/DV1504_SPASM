#pragma once

#include "../Gamemode.h"
#include "../../objects/Character.h"
#include "ControlNode.h"

class PayloadGamemode : public Gamemode {
public:
	PayloadGamemode(std::vector<Grid::Index> indices, DeferredRenderer& deferredRenderer);
	~PayloadGamemode();

	virtual void update(Character* player, float dt);
	virtual void draw();

private:
	/* Reference to the vector of all players */
	Character* m_player;

	std::vector<std::unique_ptr<ControlNode>> m_controlNodes;

	std::vector<Grid::Index> m_indices;

	std::unique_ptr<FbxModel> m_controlNodeModel;

	int m_numOfNodes;
};