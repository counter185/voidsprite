#pragma once
#include "globals.h"
#include "mathops.h"

class Timer64
{
public:
    double percentElapsedTime(uint64_t ticks, int ticksOffset = 0) {
        return ticks == 0 ? 0 : dxmin(dxmax((double)((int64_t)elapsedTime()-ticksOffset), 0.0) / (double)ticks, 1.0);
    }
    double percentLoopingTime(uint64_t ticks, int ticksOffset = 0) {
        return ticks == 0 ? 0 : dxmin(dxmax((double)(((int64_t)elapsedTime()-ticksOffset) % ticks), 0.0) / (double)ticks, 1.0);
    }
    u64 elapsedTime() {
        return started ? (SDL_GetTicks64() - startTime) : stopTime;
    }
    u64 elapsedLoopingTime(u64 ticks) {
        return (started ? (SDL_GetTicks64() - startTime) : stopTime) % ticks;
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
    void setElapsedTime(u64 time) {
        if (started) {
            start();
            startTime -= time;
        }
        else {
            stopTime = time;
        }
    }

    bool started = false;
    u64 startTime = 0;
    u64 stopTime = 0;
};

