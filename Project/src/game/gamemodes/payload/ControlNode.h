#pragma once

#include "../../objects/common/Object.h"
#include "../../../sail/utils/Timer.h"

class ControlNode : public Object {
public:
	ControlNode(Model* model);
	virtual ~ControlNode();

	virtual void draw();

	void setTeam(const int team);
	int getTeam();

	void capture(const int teamOne, const int teamTwo);

	bool updateNodeTimer(float dt);

	std::string getAsString();

private:
	struct Team {
		unsigned int id;
		bool capturing;
		bool isOwner;
		float timeCapturing;
		float ownershipTime;
		DirectX::SimpleMath::Vector4 color;
	};

private:
	int m_team;
	bool m_beingCaptured;

	Team m_teamOne, m_teamTwo;

	float m_timeTillCapture, m_timeTillScore;
	float m_timeCaptured;

	DirectX::SimpleMath::Vector4 m_teamZeroColor, m_teamOneColor, m_teamTwoColor, m_nodeColor;


};