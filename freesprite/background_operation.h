#pragma once
#include <thread>
#include <mutex>
#include <queue>

#include "globals.h"
#include "Timer64.h"

struct AsyncOp {
    bool done = false;
    std::thread* thread = NULL;
};

class OperationProgressReport {
protected:
    std::vector<std::string> reports;
    std::recursive_mutex reportMutex;
public:
    virtual void resetProgress() {
        std::lock_guard<std::recursive_mutex> lock(reportMutex);
        reports.clear();
    }
    virtual void enterSection(std::string name) {
        std::lock_guard<std::recursive_mutex> lock(reportMutex);
        reports.push_back(name);
    }
    virtual void exitSection() {
        std::lock_guard<std::recursive_mutex> lock(reportMutex);
        if (reports.size() > 0) {
            reports.pop_back();
        }
    }
    std::vector<std::string> getCurrentSections() {
        std::lock_guard<std::recursive_mutex> lock(reportMutex);
        return reports;
    }
};

class PrintOnlyProgressReport : public OperationProgressReport {
    public:
    virtual void enterSection(std::string name) override {
        loginfo(frmt("Entered section: {}", name));
    }
    virtual void exitSection() override {}

};


inline std::atomic<bool> g_bgOpRunning = false;
inline std::atomic<double> g_bgOpProgress = 0.0;
inline bool threadSet = false;
inline std::thread g_bgOpThread;
inline OperationProgressReport* g_bgOpProgressReport = new OperationProgressReport();
inline PrintOnlyProgressReport* g_printOnlyProgressReport = new PrintOnlyProgressReport();
inline Timer64 g_bgOpStartTimer;

inline std::recursive_mutex mainThreadOpMutex;
inline std::queue<std::function<void()>> g_mainThreadOperations;

inline std::vector<AsyncOp*> g_asyncOpThreads;

inline void g_startNewOperation(std::function<void(OperationProgressReport*)> function) {
    g_bgOpProgressReport->resetProgress();
#if __EMSCRIPTEN__
    function(g_bgOpProgressReport);
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
        function(g_bgOpProgressReport);
        g_bgOpRunning = false;
    });
#endif
}
inline void g_startNewOperation(std::function<void()> function) {
    g_startNewOperation([function](OperationProgressReport* report) {
        function();
    });
}

inline void g_startNewAsyncOperation(std::function<void()> function) {
    g_bgOpProgressReport->resetProgress();
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
    std::lock_guard<std::recursive_mutex> lock(mainThreadOpMutex);
    g_mainThreadOperations.push(function);
}

inline void g_runMainThreadOperations() {
    mainThreadOpMutex.lock();
    while (!g_mainThreadOperations.empty()) {
        auto& nextOp = g_mainThreadOperations.front();
        mainThreadOpMutex.unlock();
        //allow recursively adding main thread ops
        nextOp();
        mainThreadOpMutex.lock();
        g_mainThreadOperations.pop();
    }
    mainThreadOpMutex.unlock();
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
