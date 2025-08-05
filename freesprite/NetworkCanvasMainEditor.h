#pragma once
#include "maineditor.h"
#include <mutex>

class NetworkCanvasMainEditor :
    public MainEditor
{
protected:
    std::thread* clientThread = NULL;
    NetworkCanvasClientInfo thisClientInfo;
    NET_StreamSocket* clientSocket = NULL;

    bool receivedInfo = false;
    XY lastINFOSize = {};
    int lastINFONumLayers = -1;
    u64 lastCanvasUpdate = 0;
    bool receivedDataOnce = false;

    std::mutex clientSideChangesMutex;
    std::map<int,bool> clientSideChanges;

    void networkCanvasClientThread();
    void networkCanvasProcessCommandFromServer(std::string command);
    void networkCanvasSendInfoRequest();
    void networkCanvasSendLocalChanges();
    void networkCanvasSendNewLayerRequest();

    void reallocLayers(XY size, int numLayers);

public:
    NetworkCanvasMainEditor(NET_StreamSocket* socket);

    std::string getName() override { return TL("vsp.collabeditor"); }

    void networkCanvasStateUpdated(int whichLayer) override;

    Layer* newLayer() override;
};

