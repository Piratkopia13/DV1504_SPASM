#include "ControlNode.h"


ControlNode::ControlNode(Model* model) {
	setModel(model);
	m_team = 0;

	m_beingCaptured = false;
	m_timeTillCapture = 2.f;
	m_teamCapturing = 0;
	m_timeBeingCaptured = 0.f;
	m_timeCaptured = 0.f;
	m_captureTimer.startTimer();

	m_teamZeroColor = DirectX::SimpleMath::Vector4(0.f, 0.f, 0.f, 1.f);
	m_teamOneColor = DirectX::SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f);
	m_teamTwoColor = DirectX::SimpleMath::Vector4(0.f, 0.f, 1.f, 1.f);

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
	if (teamOne != teamTwo) {
		if (m_teamCapturing == 0) {
			if (teamOne > teamTwo) {
				if (m_team != 1) {
					m_teamCapturing = 1;
					m_timeBeingCaptured = 0.f;
				}
				else
					m_teamCapturing = 0;
			}
			else {
				if (m_team != 2) {
					m_teamCapturing = 2;
					m_timeBeingCaptured = 0.f;
				}
				else
					m_teamCapturing = 0;
			}
		}
		else if (m_teamCapturing == 1) {
			if (teamOne < teamTwo) {
				m_teamCapturing = teamTwo;
				m_timeBeingCaptured = 0.f;
			}
		}
		else {
			if (teamTwo < teamOne) {
				m_teamCapturing = teamOne;
				m_timeBeingCaptured = 0.f;
			}
		}
	}
	else {
		m_teamCapturing = 0;
	}
}

bool ControlNode::updateNodeTimer(float dt) {
	if (m_team != m_teamCapturing && m_teamCapturing) {
		m_timeBeingCaptured += dt;
		if (m_timeBeingCaptured >= m_timeTillCapture) {
			m_team = m_teamCapturing;
			m_teamCapturing = 0;
			m_timeBeingCaptured = 0.f;
		}
	}

	if (m_timeBeingCaptured && m_teamCapturing) {
		if(m_teamCapturing == 1) 
			m_nodeColor = m_teamOneColor * (m_timeBeingCaptured / m_timeTillCapture) +
			((float(m_team)/2.f) * m_teamTwoColor * (1.f - (m_timeBeingCaptured / m_timeTillCapture)));
		else
			m_nodeColor = m_teamTwoColor * (m_timeBeingCaptured / m_timeTillCapture) +
			((float(m_team)) * m_teamOneColor * (1.f - (m_timeBeingCaptured / m_timeTillCapture)));;
	}
	else {
		switch (m_team) {
		case 1:
			m_nodeColor = m_teamOneColor;
			break;
		case 2:
			m_nodeColor = m_teamTwoColor;
			break;
		default:
			m_nodeColor = m_teamZeroColor;
			break;
		}
	}
	
	return false;
}