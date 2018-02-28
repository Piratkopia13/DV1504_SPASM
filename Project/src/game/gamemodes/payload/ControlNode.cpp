#include "ControlNode.h"


ControlNode::ControlNode(Model* model) {
	setModel(model);
	m_team = -1;

	m_beingCaptured = false;
	m_timeTillCapture = 3.f;
	m_maxTimeCaptured = 10.f;
	m_timeCaptured = 0.f;

	m_teamZeroColor = DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f);
	m_teamOneColor = DirectX::SimpleMath::Vector4(3.f, 0.0f, 0.0f, 1.f);
	m_teamTwoColor = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 5.f, 1.f);

	m_teamOne.color = m_teamOneColor;
	m_teamOne.ownershipTime = 0.f;
	m_teamOne.id = 0;
	m_teamOne.capturing = false;
	m_teamOne.isOwner = false;
	m_teamOne.timeCapturing = 0.f;

	m_teamTwo.color = m_teamTwoColor;
	m_teamTwo.ownershipTime = 0.f;
	m_teamTwo.id = 1;
	m_teamTwo.capturing = false;
	m_teamTwo.isOwner = false;
	m_teamTwo.timeCapturing = 0.f;

	m_nodeColor = m_teamZeroColor;
	m_ownershipColor = m_teamZeroColor;
}

ControlNode::~ControlNode() {

}

void ControlNode::draw() {
	getModel()->setTransform(&getTransform());
	getModel()->getMaterial()->setColor(m_nodeColor);
	getModel()->draw();
}

void ControlNode::setTeamColor(const int team, const DirectX::SimpleMath::Vector4 & color) {
	switch (team) {
	case 0: 
		m_teamOneColor = color;
		m_teamOne.color = color;
		break;
	case 1:
		m_teamTwoColor = color;
		m_teamTwo.color = color;
		break;
	default:
		break;
	}
}

void ControlNode::setTeam(const int team) {
	m_team = team;
	if (team == 0) {
		m_teamOne.isOwner = true;
		m_teamOne.timeCapturing = m_timeTillCapture;
		m_teamTwo.isOwner = false;
		m_teamTwo.timeCapturing = 0.f;
	} 
	else if (team == 1) {
		m_teamTwo.isOwner = true;
		m_teamTwo.timeCapturing = m_timeTillCapture;
		m_teamOne.isOwner = false;
		m_teamOne.timeCapturing = 0.f;
	}
}

int ControlNode::getTeam() {
	return m_team;
}

void ControlNode::capture(const int teamOne, const int teamTwo) {
	// If no one is on the point, no one is capturing it
	if (teamOne > 0 || teamTwo > 0) {
		// First checks if theres equal amount of each team on point
		if (teamOne == teamTwo) {
			m_teamOne.capturing = true;
			m_teamTwo.capturing = true;
			m_beingCaptured = false;
		}
		// Checks if no one is currently capturing and puts a team on capturing depending on which team is more
		if (!m_teamOne.capturing && !m_teamTwo.capturing) {
			if (teamOne > teamTwo) {
				m_beingCaptured = true;
				if (!m_teamOne.isOwner)
					m_teamOne.capturing = true;
			}
			else if (teamOne < teamTwo) {
				m_beingCaptured = true;
				if (!m_teamTwo.isOwner)
					m_teamTwo.capturing = true;
			}
		}
		// Else if both is capturing, the point won't be captured
		else if (m_teamOne.capturing && m_teamTwo.capturing) {
			m_beingCaptured = false;
		}
		// Else if someone is currently holding capture of the point, and have left, and a new team enters
		else {
			if (m_teamOne.capturing) {
				// If team one is the current capturer, switch if team two are greater in numbers
				if (teamOne < teamTwo) {
					m_beingCaptured = true;
					m_teamTwo.capturing = true;
					m_teamOne.capturing = false;
				}
				else {
					m_beingCaptured = true;
					m_teamOne.capturing = true;
				}
			}

			if (m_teamTwo.capturing) {
				// If team two is the current capturer, switch if team one are greater in numbers
				if (teamTwo < teamOne) {
					m_beingCaptured = true;
					m_teamOne.capturing = true;
					m_teamTwo.capturing = false;
				}
				else {
					m_beingCaptured = true;
					m_teamTwo.capturing = true;
				}
			}
		}
	}
	else {
		m_beingCaptured = false;
	}
}

bool ControlNode::updateNodeTimer(float dt) {
	// Timing is removed if team one is not on the point, and their timing is what is active
	if (m_teamOne.capturing) {
		// If someone is on the point
		if (m_beingCaptured) {
			// Adds time if team two is not trying to capture at the same time as team one
			if (!m_teamTwo.capturing) {
				if (m_teamTwo.timeCapturing <= 0.f)
					m_teamOne.timeCapturing += dt;
				else {
					m_teamTwo.timeCapturing -= dt;
				}
			}
		}
		else if (!m_teamTwo.capturing) {
			m_teamOne.timeCapturing -= dt;
		}
	}
	
	// Timing is removed if team two is not on the point, and their timing is what is active
	if (m_teamTwo.capturing) {
		if (m_beingCaptured) {
			// Adds time if team one is not trying to capture at the same time as team two
			if (!m_teamOne.capturing) {
				if (m_teamOne.timeCapturing <= 0.f)
					m_teamTwo.timeCapturing += dt;
				else {
					m_teamOne.timeCapturing -= dt;
				}
			}
		}
		else if (!m_teamOne.capturing) {
			m_teamTwo.timeCapturing -= dt;
		}
	}


	if (m_teamOne.timeCapturing <= 0.f) {
		m_teamOne.capturing = false;
		m_teamOne.timeCapturing = 0.f;
	}

	if (m_teamTwo.timeCapturing <= 0.f) {
		m_teamTwo.capturing = false;
		m_teamTwo.timeCapturing = 0.f;
	}



	// If the point isn't being captured and either team is the current owner and no one is on the point, 
	// the timer goes back up for the team
	if (!m_beingCaptured) {
		if (m_teamOne.isOwner && m_teamOne.timeCapturing < m_timeTillCapture && !m_teamTwo.capturing) {
			m_teamOne.timeCapturing += dt;
			if (m_teamOne.timeCapturing >= m_timeTillCapture)
				m_teamOne.timeCapturing = m_timeTillCapture;
		}

		if (m_teamTwo.isOwner && m_teamTwo.timeCapturing < m_timeTillCapture && !m_teamOne.capturing) {
			m_teamTwo.timeCapturing += dt;
			if (m_teamTwo.timeCapturing >= m_timeTillCapture)
				m_teamTwo.timeCapturing = m_timeTillCapture;
		}
	}

	// Set team to not capture if the timer goes to zero
	if (m_teamOne.timeCapturing <= 0.f && m_teamOne.isOwner) {
		m_teamOne.capturing = false;
		m_teamOne.isOwner = false;
		m_team = -1;
		m_teamOne.timeCapturing = 0.f;
		m_ownershipColor = m_teamZeroColor;
	}
	// Else if the timer has passed the amount required to capture a point, set ownership
	else if (m_teamOne.timeCapturing > m_timeTillCapture) {
		m_teamOne.capturing = false;
		m_teamOne.isOwner = true;
		m_teamOne.timeCapturing = m_timeTillCapture;
		m_ownershipColor = m_teamOneColor;
	}

	// Set team to not capture if the timer goes to zero
	if (m_teamTwo.timeCapturing <= 0.f && m_teamTwo.isOwner) {
		m_teamTwo.capturing = false;
		m_teamTwo.isOwner = false;
		m_team = -1;
		m_teamTwo.timeCapturing = 0.f;
		m_ownershipColor = m_teamZeroColor;
	}
	// Else if the timer has passed the amount required to capture a point, set ownership
	else if (m_teamTwo.timeCapturing > m_timeTillCapture) {
		m_teamTwo.capturing = false;
		m_teamTwo.isOwner = true;
		m_teamTwo.timeCapturing = m_timeTillCapture;
		m_ownershipColor = m_teamTwoColor;
	}

	// Set the node color depending on the timing
	m_nodeColor = m_ownershipColor + DirectX::SimpleMath::Vector4(m_teamOne.color * (m_teamOne.timeCapturing * 0.7f) / m_timeTillCapture) +
		DirectX::SimpleMath::Vector4(m_teamTwo.color * (m_teamTwo.timeCapturing * 0.7f) / m_timeTillCapture);

	// Updates the timer for the capturepoints pointcounter
	if (m_teamOne.isOwner) {
		if (m_team != m_teamOne.id) {
			setTeam(m_teamOne.id);
			m_timeCaptured = 0.f;
		}
		m_timeCaptured += dt;
	}
	if (m_teamTwo.isOwner) {
		if (m_team != m_teamTwo.id) {
			setTeam(m_teamTwo.id);
			m_timeCaptured = 0.f;
		}
		m_timeCaptured += dt;
	}

	if (m_timeCaptured >= m_maxTimeCaptured) {
		reset();
		return true;
	}



	return false;
}

void ControlNode::reset() {
	m_team = -1;

	m_beingCaptured = false;
	m_timeCaptured = 0.f;

	m_teamOne.ownershipTime = 0.f;
	m_teamOne.capturing = false;
	m_teamOne.isOwner = false;
	m_teamOne.timeCapturing = 0.f;


	m_teamTwo.ownershipTime = 0.f;
	m_teamTwo.capturing = false;
	m_teamTwo.isOwner = false;
	m_teamTwo.timeCapturing = 0.f;

	m_ownershipColor = m_teamZeroColor;
}

std::string ControlNode::getAsString() {
	std::string toReturn;
	toReturn += "Team 1 - Owner: " + std::to_string(m_teamOne.isOwner) +
		" Capture timer: " + std::to_string(m_teamOne.timeCapturing) +
		" is capturing: " + std::to_string(m_teamOne.capturing) + "  ";

	toReturn += "Team 2 - Owner: " + std::to_string(m_teamTwo.isOwner) +
		" Capture timer: " + std::to_string(m_teamTwo.timeCapturing) +
		" is capturing: " + std::to_string(m_teamTwo.capturing) + "-" + std::to_string(m_beingCaptured);

	return toReturn;
}

float ControlNode::getCapturePercentage() {
	return max(m_teamOne.timeCapturing, m_teamTwo.timeCapturing) / m_timeTillCapture;
}