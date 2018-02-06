#pragma once

#include "../../objects/common/Object.h"
#include "../../../sail/utils/Timer.h"

class ControlNode : public Object {
public:
	ControlNode(Model* model);
	~ControlNode();

	virtual void draw();

	void setTeam(const int team);
	int getTeam();

	void capture(const int teamOne, const int teamTwo);

	bool updateNodeTimer(float dt);

private:
	int m_team;
	int teamOne = 1, teamTwo = 2;
	int m_teamCapturing;
	bool m_beingCaptured;

	float m_timeBeingCaptured;
	float m_timeTillCapture;
	float m_timeCaptured;
	Timer m_captureTimer;

	DirectX::SimpleMath::Vector4 m_teamZeroColor, m_teamOneColor, m_teamTwoColor, m_nodeColor;
};