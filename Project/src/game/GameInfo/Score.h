#pragma once
#include <vector>
#include <string>

class Score {

public:
	struct PlayerStats;
	Score(size_t players) {
		for (size_t i = 0; i < players; i++) {
			m_gameScore.push_back({ 0,0,0,0 });
		}
	}
	~Score() {

	}
	void addPlayer() {
		m_gameScore.push_back({ 0,0,0,0.0f });
	}
	

	void addKill(size_t index) {
		if (index < m_gameScore.size())
			m_gameScore[index].kills++;
	}
	void addDeath(size_t index) {
		if (index < m_gameScore.size())
			m_gameScore[index].deaths++;
	}
	void addCapture(size_t index) {
		if (index < m_gameScore.size())
			m_gameScore[index].pointsCaptured++;
	}
	void addPoints(size_t index, float score) {
		if (index < m_gameScore.size())
			m_gameScore[index].gamePoints+= score;
	}


	const PlayerStats getPlayerStats(size_t index) {
		return m_gameScore[index];
	}

	struct PlayerStats {
		int kills;
		int deaths;
		int pointsCaptured;
		float gamePoints;
	};


private:
	std::vector<PlayerStats> m_gameScore;
};