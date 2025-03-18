#pragma once
#include "globals.h"
#include "mathops.h"

class Timer64
{
public:
    double percentElapsedTime(u64 ticks, int ticksOffset = 0) {
        return ticks == 0 ? 0 : dxmin(dxmax((double)((s64)elapsedTime()-ticksOffset), 0.0) / (double)ticks, 1.0);
    }
    u64 elapsedTime() {
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
    void startIfNotStarted() {
        if (!started) {
            start();
        }
    }

    bool started = false;
    u64 startTime = 0;
    u64 stopTime = 0;
};

