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

    std::queue<std::string> chatMsgQueue;
    std::mutex chatMsgQueueMutex;

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
    void networkCanvasChatSendCallback(std::string content) override;

    Layer* newLayer() override;
    void deleteLayer(int index) override;
    void moveLayerUp(int index) override;
    void moveLayerDown(int index) override;
    void mergeLayerDown(int index) override;
    void duplicateLayer(int index) override;
    void rescaleAllLayersFromCommand(XY size) override;
    void resizeAllLayersFromCommand(XY size, bool byTile = false) override;
    void resizzeAllLayersByTilecountFromCommand(XY size) override;
    void integerScaleAllLayersFromCommand(XY scale, bool downscale = false) override;

    void tickAutosave() override {};    //don't do local recovery autosaves in client sessions... (maybe an option in settings?)
    void promptStartNetworkSession() override;

    void postErrorHostOnly();
    void endClientNetworkSession();
};

