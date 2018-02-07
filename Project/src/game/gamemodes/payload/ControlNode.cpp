#include "ControlNode.h"


ControlNode::ControlNode(Model* model) {
	setModel(model);
	m_team = 0;

	m_beingCaptured = false;
	m_timeTillCapture = 5.f;
	m_timeTillScore = 3.f;
	m_timeCaptured = 0.f;

	m_teamZeroColor = DirectX::SimpleMath::Vector4(0.f, 0.f, 0.f, 1.f);
	m_teamOneColor = DirectX::SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f);
	m_teamTwoColor = DirectX::SimpleMath::Vector4(0.f, 0.f, 1.f, 1.f);

	m_teamOne.color = m_teamOneColor;
	m_teamOne.ownershipTime = 0.f;
	m_teamOne.id = 1;
	m_teamOne.capturing = false;
	m_teamOne.isOwner = false;
	m_teamOne.timeCapturing = 0.f;


	m_teamTwo.color = m_teamTwoColor;
	m_teamTwo.ownershipTime = 0.f;
	m_teamTwo.id = 2;
	m_teamTwo.capturing = false;
	m_teamTwo.isOwner = false;
	m_teamTwo.timeCapturing = 0.f;

	m_nodeColor = m_teamZeroColor;
}

ControlNode::~ControlNode() {

}

void ControlNode::draw() {
	getModel()->setTransform(&getTransform());
	getModel()->getMaterial()->setColor(m_nodeColor);
	getModel()->draw();
}

void ControlNode::setTeam(const int team) {
	m_team = team;
}

int ControlNode::getTeam() {
	return m_team;
}

void ControlNode::capture(const int teamOne, const int teamTwo) {
	if (teamOne > 0 || teamTwo > 0) {
		if (teamOne == teamTwo) {
			m_teamOne.capturing = true;
			m_teamTwo.capturing = true;
			m_beingCaptured = false;
		}
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
		else if (m_teamOne.capturing && m_teamTwo.capturing) {
			m_beingCaptured = false;
		}
		else {
			if (m_teamOne.capturing) {
				if (teamOne < teamTwo) {
					m_beingCaptured = true;
					m_teamTwo.capturing = true;
				}
				else {
					m_beingCaptured = true;
					m_teamOne.capturing = true;
				}
			}

			if (m_teamTwo.capturing) {
				if (teamTwo < teamOne) {
					m_beingCaptured = true;
					m_teamOne.capturing = true;
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
	if (m_teamOne.capturing) {
		if (m_beingCaptured) {
			if (!m_teamTwo.capturing) {
				if (m_teamTwo.timeCapturing <= 0.f)
					m_teamOne.timeCapturing += dt;
				else
					m_teamTwo.timeCapturing -= dt;
			}
			else
				m_teamOne.timeCapturing -= dt;
		}
		else if (m_teamOne.capturing && !m_teamTwo.capturing) 
			m_teamOne.timeCapturing -= dt;
	}
	
	if (m_teamTwo.capturing) {
		if (m_beingCaptured) {
			if (!m_teamOne.capturing) {
				if (m_teamOne.timeCapturing <= 0.f)
					m_teamTwo.timeCapturing += dt;
				else
					m_teamOne.timeCapturing -= dt;
			}
			else
				m_teamTwo.timeCapturing -= dt;
		}
		else if (m_teamTwo.capturing && !m_teamOne.capturing)
			m_teamTwo.timeCapturing -= dt;
	}

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

	if (m_teamOne.timeCapturing <= 0.f) {
		m_teamOne.capturing = false;
		m_teamOne.isOwner = false;
		m_teamOne.timeCapturing = 0.f;
	}
	else if (m_teamOne.timeCapturing > m_timeTillCapture) {
		m_teamOne.capturing = false;
		m_teamOne.isOwner = true;
		m_teamOne.timeCapturing = m_timeTillCapture;
	}
	
	if (m_teamTwo.timeCapturing <= 0.f) {
		m_teamTwo.capturing = false;
		m_teamTwo.isOwner = false;
		m_teamTwo.timeCapturing = 0.f;
	}
	else if (m_teamTwo.timeCapturing > m_timeTillCapture) {
		m_teamTwo.capturing = false;
		m_teamTwo.isOwner = true;
		m_teamTwo.timeCapturing = m_timeTillCapture;
	}

	m_nodeColor = DirectX::SimpleMath::Vector4(m_teamOne.color * pow(m_teamOne.timeCapturing / m_timeTillCapture, 2.f)) +
		DirectX::SimpleMath::Vector4(m_teamTwo.color * pow(m_teamTwo.timeCapturing / m_timeTillCapture, 2.f));
	
	if (m_teamOne.isOwner) {
		if (m_team != m_teamOne.id) {
			setTeam(m_teamOne.id);
			m_timeCaptured = 0.f;
		}
		else {
			m_timeCaptured += dt;
			if (m_timeCaptured >= m_timeTillScore) {
				m_timeCaptured = 0.f;
				return true;
			}
		}
	}
	if (m_teamTwo.isOwner) {
		if (m_team != m_teamTwo.id) {
			setTeam(m_teamTwo.id);
			m_timeCaptured = 0.f;
		}
		else {
			m_timeCaptured += dt;
			if (m_timeCaptured >= m_timeTillScore) {
				m_timeCaptured = 0.f;
				return true;
			}
		}
	}



	return false;
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