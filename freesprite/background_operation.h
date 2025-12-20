#pragma once
#include <thread>
#include <mutex>

#include "globals.h"
#include "Timer64.h"

inline std::atomic<bool> g_bgOpRunning = false;
inline std::atomic<double> g_bgOpProgress = 0.0;
inline bool threadSet = false;
inline std::thread g_bgOpThread;
inline Timer64 g_bgOpStartTimer;

inline std::mutex mainThreadOpMutex;
inline std::vector<std::function<void()>> g_mainThreadOperations;

struct AsyncOp {
    bool done = false;
    std::thread* thread = NULL;
};

inline std::vector<AsyncOp*> g_asyncOpThreads;

inline void g_startNewOperation(std::function<void()> function) {
#if __EMSCRIPTEN__
    function();
#else
    g_bgOpRunning = true;
    if (threadSet) {
        g_bgOpThread.join();
    }
    g_bgOpStartTimer.start();
    g_bgOpProgress = 0.0;
    threadSet = true;
    g_bgOpThread = std::thread([function]() {
        srand(time(NULL) * 1000 + SDL_GetTicks());
        function();
        g_bgOpRunning = false;
    });
#endif
}

inline void g_startNewAsyncOperation(std::function<void()> function) {
#if __EMSCRIPTEN__
    function();
#else
    AsyncOp* op = new AsyncOp();
    g_asyncOpThreads.push_back(op);
    op->thread = new std::thread([function, op]() {
        srand(time(NULL) * 1000 + SDL_GetTicks());
        function();
        op->done = true;
    });
#endif
}

inline void g_startNewMainThreadOperation(std::function<void()> function) {
    std::lock_guard<std::mutex> lock(mainThreadOpMutex);
    g_mainThreadOperations.push_back(function);
}

inline void g_runMainThreadOperations() {
    std::lock_guard<std::mutex> lock(mainThreadOpMutex);
    for (auto& op : g_mainThreadOperations) {
        op();
    }
    g_mainThreadOperations.clear();
}
inline void g_cleanUpDoneAsyncThreads() {
    for (int x = 0; x < g_asyncOpThreads.size(); x++) {
        if (g_asyncOpThreads[x]->done) {
            if (g_asyncOpThreads[x]->thread != NULL) {
                g_asyncOpThreads[x]->thread->join();
                delete g_asyncOpThreads[x]->thread;
            }
            delete g_asyncOpThreads[x];
            g_asyncOpThreads.erase(g_asyncOpThreads.begin() + x);
            x--;
        }
    }
}
inline void g_waitAndRemoveAllBgOpAndAsyncThreads() {
    if (threadSet) {
        g_bgOpThread.join();
    }
    for (int x = 0; x < g_asyncOpThreads.size(); x++) {
        if (g_asyncOpThreads[x]->thread != NULL) {
            g_asyncOpThreads[x]->thread->join();
            delete g_asyncOpThreads[x]->thread;
        }
        delete g_asyncOpThreads[x];
    }
}
