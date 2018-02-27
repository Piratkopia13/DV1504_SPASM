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
	void setParticleHandler(ParticleHandler* newParticleHandler);

	virtual int checkWin();

private:
	void replacePoint();

private:
	std::vector<std::unique_ptr<ControlNode>> m_controlNodes;

	ParticleHandler* m_particleHandler;
	std::shared_ptr<ParticleEmitter> m_pointEmitter, m_pointEmitter2;
	DirectX::SimpleMath::Vector3 m_emitterPos;

	float m_emitterRotation;

	std::vector<Grid::Index> m_indices;

	int m_numOfNodes;
	int m_radius;
	int m_teamWin;
	int m_currentActivePoint;

	float m_scoreToWin;

	std::vector<std::vector<Block*>>& m_blocks;
	int m_levelWidth, m_levelHeight;

	DirectX::SimpleMath::Vector4 m_teamOneColor, m_teamTwoColor;
};