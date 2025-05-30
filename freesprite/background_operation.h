#pragma once

inline std::atomic<bool> g_bgOpRunning = false;
inline std::atomic<double> g_bgOpProgress = 0.0;
inline bool threadSet = false;
inline std::thread g_bgOpThread;
inline Timer64 g_bgOpStartTimer;

inline void g_startNewOperation(std::function<void()> function) {
    g_bgOpRunning = true;
    if (threadSet) {
        g_bgOpThread.join();
    }
    g_bgOpStartTimer.start();
    g_bgOpProgress = 0.0;
    threadSet = true;
    g_bgOpThread = std::thread([function]() {
        srand(time(NULL));
        function();
        g_bgOpRunning = false;
    });
}