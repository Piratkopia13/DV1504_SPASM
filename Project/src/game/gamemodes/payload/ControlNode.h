#pragma once

#include "../../objects/common/Object.h"
#include "../../../sail/utils/Timer.h"

class ControlNode : public Object {
public:
	ControlNode(Model* model);
	virtual ~ControlNode();

	virtual void draw();

	void setTeam(const int team);
	/*
		Gets the team that currently holds the controlpoint

		@return 1 or 2 depending the ownership
	*/
	int getTeam();

	/*
		Updates the controlnode's capture variables

		@param teamOne number of players from team one
		@param teamTwo number of players from team two
	*/
	void capture(const int teamOne, const int teamTwo);

	/*
		Deals with all the updates of the controlnode

		@param dt the delta time
		@return true if a point has been generated, false if not
	*/
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