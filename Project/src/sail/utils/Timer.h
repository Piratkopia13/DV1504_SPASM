#pragma once

#include <Windows.h>
#include <time.h>

class Timer {

public:

	void startTimer() {

		LARGE_INTEGER frequencyCount;
		QueryPerformanceFrequency(&frequencyCount);

		m_countsPerSecond = static_cast<double>(frequencyCount.QuadPart);

		QueryPerformanceCounter(&frequencyCount);
		m_counterStart = frequencyCount.QuadPart;

	}

	double getTime() {

		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		return static_cast<double>(currentTime.QuadPart - m_counterStart / m_countsPerSecond);

	}

	double getFrameTime() {

		LARGE_INTEGER currentTime;
		INT64 elapsedTime;
		QueryPerformanceCounter(&currentTime);

		elapsedTime = currentTime.QuadPart - m_oldframeTime;
		m_oldframeTime = currentTime.QuadPart;

		if (elapsedTime < 0)
			elapsedTime = 0;

		return elapsedTime / m_countsPerSecond;

	}

private:
	double m_countsPerSecond = 0.0;
	INT64 m_counterStart = 0;

	INT64 m_oldframeTime = 0;

};



class TimerH {
public:
	TimerH() {
		this->startTime = GetTickCount();
		this->lastTime = GetTickCount();
		this->smoothing = 0.95;
		this->avg = 0;
	}

	TimerH(const float smoothing) {
		this->smoothing = smoothing;
	}
	~TimerH() {

	}
	float reset() {
		this->startTime = GetTickCount();
		this->lastTime = GetTickCount();
		return 0;
	}
	float getTime() {
		return difftime(GetTickCount(), startTime);
	}
	float deltaTime() {
		float t = difftime(GetTickCount(), this->lastTime);
		this->lastTime = GetTickCount();

		this->avg = (this->avg * this->smoothing) + (t * (1 - this->smoothing));
		return t;
	}
	float getFPS() {
		return 1000 / this->avg;
	}
	float getAvg() {
		return this->avg;
	}

private:
	time_t startTime;
	time_t lastTime;
	float avg;
	float smoothing;

};