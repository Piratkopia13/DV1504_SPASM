#pragma once
#include <string>
#include <minmax.h>

class Profile {
public:
	struct ProfileStats;

	Profile(std::string name, size_t preOrder) {
		m_name = name;
		m_preOrder = preOrder;
		m_stats = { 0,0,0,0 };
	};
	Profile(std::string name, size_t preOrder, ProfileStats stats) {
		m_name = name;
		m_preOrder = preOrder;
		m_stats = stats;
	}
	~Profile() {};


	void addKills(size_t kills) {
		m_stats.kills += kills;
	}
	void addDeaths(size_t deaths) {
		m_stats.deaths += deaths;
	}
	void addWin() {
		m_stats.wins++;
	}
	void addLoss() {
		m_stats.losses++;
	}


	const float getKD() {
		return (float)m_stats.kills / (float)max((float)m_stats.deaths,1.0f);
	};
	const float getWinRatio() {
		return (float)m_stats.wins / (float)max((float)m_stats.losses,1.0f);
	};
	const std::string getName() {
		return m_name;
	};
	const ProfileStats getStats() {
		return m_stats;
	}
	const size_t preOrdered() {
		return m_preOrder;
	}
	const std::string getAsString() {
		return m_name + ":" +
			std::to_string(m_preOrder) + ":" +
			std::to_string(m_stats.kills) + ":" +
			std::to_string(m_stats.deaths) + ":" +
			std::to_string(m_stats.wins) + ":" +
			std::to_string(m_stats.losses);
	}

	struct ProfileStats {
		size_t kills;
		size_t deaths;
		size_t wins;
		size_t losses;
	};

private:
	std::string m_name;
	size_t m_preOrder;
	ProfileStats m_stats;
};