#pragma once
#include "globals.h"
#include "mathops.h"

class Timer64
{
public:
	double percentElapsedTime(uint64_t ticks, int ticksOffset = 0) {
		return ticks == 0 ? 0 : dxmin(dxmax((double)((int64_t)elapsedTime()-ticksOffset), 0.0) / (double)ticks, 1.0);
	}
	uint64_t elapsedTime() {
		return started ? (SDL_GetTicks64() - startTime) : stopTime;
	}
	void start() {
		started = true;
		startTime = SDL_GetTicks64();
	}
	void stop() {
		if (started) {
			started = false;
			stopTime = elapsedTime();
		}
	}

	bool started = false;
	uint64_t startTime = 0;
	uint64_t stopTime = 0;
};

