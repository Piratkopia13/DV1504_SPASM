#pragma once

#include "../Gamemode.h"
#include "ControlNode.h"
#include "../../level/Grid.h"


class CharacterHandler;
class PayloadGamemode : public Gamemode {
public:
	PayloadGamemode(std::vector<Grid::Index> indices, DeferredRenderer& deferredRenderer);
	~PayloadGamemode();

	virtual void update(CharacterHandler* charHandler, float dt);
	virtual void draw();

private:
	std::vector<std::unique_ptr<ControlNode>> m_controlNodes;

	std::vector<Grid::Index> m_indices;

	std::unique_ptr<FbxModel> m_controlNodeModel;

	int m_numOfNodes;
};