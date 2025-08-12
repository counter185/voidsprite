#pragma once
#include "maineditor.h"
#include "operation_queue.h"
#include <mutex>

#include "PopupSetupNetworkCanvas.h"


class NetworkCanvasMainEditor :
    public MainEditor
{
protected:
    std::thread* clientThread = NULL;
    NET_StreamSocket* clientSocket = NULL;

    std::string targetIP;
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
    NetworkCanvasMainEditor(std::string displayIP, PopupSetNetworkCanvasData userData, NET_StreamSocket* socket);
    ~NetworkCanvasMainEditor() {
        endClientNetworkSession();
    }

    std::string getName() override { return TL("vsp.collabeditor") + ": " + targetIP; }
    std::string getRPCString() override { return TL("vsp.collabeditor"); }

    void networkCanvasStateUpdated(int whichLayer) override;

    Layer* newLayer() override;
    void deleteLayer(int index) override;
    void moveLayerUp(int index) override;
    void moveLayerDown(int index) override;
    void mergeLayerDown(int index) override;
    void duplicateLayer(int index) override;
    void promptStartNetworkSession() override;

    void endClientNetworkSession();
};

