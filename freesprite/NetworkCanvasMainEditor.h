#pragma once
#include "maineditor.h"
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

    void networkCanvasClientThread();
    void networkCanvasProcessCommandFromServer(std::string command);
    void networkCanvasSendInfoRequest();

    void reallocLayers(XY size, int numLayers);

public:
    NetworkCanvasMainEditor(NET_StreamSocket* socket);
};

