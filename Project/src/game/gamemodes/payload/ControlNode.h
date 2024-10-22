#pragma once

#include "../../objects/common/Object.h"
#include "../../../sail/utils/Timer.h"

class ControlNode : public Object {
public:
	ControlNode(Model* model);
	virtual ~ControlNode();

	virtual void draw();

	void setTeamColor(const int team, const DirectX::SimpleMath::Vector4 & color);

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

	/*
	Resets all the values of the controlnode
	*/
	void reset();

	std::string getAsString();

	float getCapturePercentage();

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

	float m_timeTillCapture, m_maxTimeCaptured;
	float m_timeCapturedTeamOne, m_timeCapturedTeamTwo;

	DirectX::SimpleMath::Vector4 m_teamZeroColor, m_teamOneColor, m_teamTwoColor, m_nodeColor, m_ownershipColor;


};