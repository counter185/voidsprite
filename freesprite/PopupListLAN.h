#pragma once
#include "BasePopup.h"

#include <thread>
#include <mutex>
#include "operation_queue.h"

class PopupListLAN :
    public BasePopup
{
private:
    std::thread* lanScanThread = NULL;
    bool runThread = true;
protected:
    std::mutex addressListMutex;
    std::map<std::string, bool> addrsFound;
    ScrollingPanel* listPanel;
    OperationQueue opQueue;
public:
    PopupListLAN();
    ~PopupListLAN();

    void tick() override;

    void populateList();
    void lanScannerThread();
};

