#include "NetworkCanvasMainEditor.h"
#include "FileIO.h"
#include <thread>
#include "json/json.hpp"
#include "background_operation.h"

#include "EditorLayerPicker.h"
#include "Notification.h"
#include "CollapsableDraggablePanel.h"

using namespace nlohmann;

void NetworkCanvasMainEditor::networkCanvasClientThread()
{
#if VSP_NETWORKING
    networkCanvasSendAUTH();

    std::string commandBuffer = "";
    networkCanvasSendInfoRequest();
    while (networkRunning) {
        try {
            u64 ticksNow = SDL_GetTicks();
            if (networkReadCommandIfAvailable(clientSocket, commandBuffer)) {
                networkCanvasProcessCommandFromServer(commandBuffer);
                commandBuffer = "";
            }
            if ((ticksNow - lastCanvasUpdate) > 64) {
                networkCanvasSendInfoRequest();
                networkSendCommand(clientSocket, "UPDD");
                lastCanvasUpdate = ticksNow;

                networkCanvasSendLocalChanges();
            }
            {
                std::lock_guard<std::mutex> guard(chatMsgQueueMutex);
                if (!chatMsgQueue.empty()) {
                    networkSendCommand(clientSocket, "CHTQ");
                    networkSendString(clientSocket, chatMsgQueue.front());
                    chatMsgQueue.pop();
                }
            }
        }
        catch (std::exception& e) {
            logerr(frmt("Network client error:\n {}", e.what()));
            break;
        }
    }
    networkRunning = false;

    NET_DestroyStreamSocket(clientSocket);
    mainThreadOps.add([this]() {
        g_addNotification(Notification(TL("vsp.collabeditor.error.disconnected"), ""));
        closeNextTick = true;
    });
#else
    logerr("networkCanvasClientThread called on non-network build");
    closeNextTick = true;
#endif
}

void NetworkCanvasMainEditor::networkCanvasProcessCommandFromServer(std::string command)
{
    if (command == "INFO") {
        std::string inputString = networkReadString(clientSocket);
        json infoJson = json::parse(inputString);
        std::string serverName = infoJson["serverName"];
        XY canvasSize = XY{ infoJson["canvasWidth"], infoJson["canvasHeight"] };
        int tileGridWidth = infoJson["tileGridWidth"];
        int tileGridHeight = infoJson["tileGridHeight"];
        int thisUID = infoJson["yourUserIDIs"];
        u32 chatState = infoJson["chatState"];
        thisClientInfo->uid = thisUID;
        auto layerData = infoJson["layers"];
        int numLayers = layerData.size();
        if (!receivedInfo || !xyEqual(canvasSize, lastINFOSize) || lastINFONumLayers != numLayers) {
            receivedInfo = true;
            lastINFOSize = canvasSize;
            lastINFONumLayers = numLayers;
            canvas.dimensions = canvasSize;
            reallocLayers(canvasSize, numLayers);
        }

        tileDimensions = { ixmax(0, tileGridWidth), ixmax(0, tileGridHeight) };

        int i = 0;
        for (auto& l : layerData) {
            Layer* layer = layers[i++];
            layer->name = l["name"];
            layer->layerAlpha = l["opacity"];
            layer->hidden = l["hidden"];
        }

        auto userData = infoJson["clients"];
        networkClientsListMutex.lock();
        for (auto*& user : networkClients) {
            delete user;
        }
        networkClients.clear();
        for (auto& user : userData) {
            NetworkCanvasClientInfo* clientInfo = new NetworkCanvasClientInfo();
            clientInfo->uid = user["uid"];
            clientInfo->clientName = user["clientName"];
            clientInfo->cursorPosition = XY{ user["cursorX"], user["cursorY"] };
            clientInfo->lastReportTime = user["lastReportTime"];
            try {
                std::string colorString = user["clientColor"];
                clientInfo->clientColor = std::stoi(colorString, 0, 16);
            }
            catch (std::exception&) {
                clientInfo->clientColor = 0xC0E1FF;
            }
            networkClients.push_back(clientInfo);
        }
        networkClientsListMutex.unlock();

        //todo: do this only when the layers update and not every frame
        mainThreadOps.add([this]() {
            networkCanvasHostPanel->updateClientList();
            layerPicker->updateLayers();
        });

        if (chatState != networkCanvasCurrentChatState->messagesState) {
            networkSendCommand(clientSocket, "CHTS");
        }
    }
    else if (command == "LRDT") {
        u32 index;
        u64 dataSize;
        networkReadBytes(clientSocket, (u8*)&index, 4);
        networkReadBytes(clientSocket, (u8*)&dataSize, 8);
        u8* dataBuffer = (u8*)tracked_malloc(dataSize);
        if (dataBuffer != NULL) {
            networkReadBytes(clientSocket, dataBuffer, dataSize);
            Layer* l = layers[index];
            auto decompressed = decompressZlibWithoutUncompressedSize(dataBuffer, dataSize);
            if (decompressed.size() != l->w * l->h * 4) {
                logerr(frmt("Decompressed data size mismatch: expected {}, got {}", l->w * l->h * 4, decompressed.size()));
            }
            else {
                if (index != selLayer || !leftMouseHold) {
                    memcpy(l->pixels8(), decompressed.data(), 4ull * l->w * l->h);
                    l->markLayerDirty();
                }
            }
            tracked_free(dataBuffer);
        }
        else {
            logerr("Failed to allocate memory for layer pixel data update");
        }
    }
    else if (command == "UPDD") {
        u32 oldStateID = canvasStateID;
        networkReadBytes(clientSocket, (u8*)&canvasStateID, 4);
        //loginfo("Received UPDD back");
        if (oldStateID != canvasStateID || !receivedDataOnce) {
            loginfo(frmt("{:08X} != {:08X}, updating", oldStateID, canvasStateID));
            receivedDataOnce = true;
            networkCanvasSendInfoRequest();
            for (int i = 0; i < layers.size(); i++) {
                networkSendCommand(clientSocket, "LRRQ");
                networkSendBytes(clientSocket, (u8*)&i, 4);
            }
        }
    }
    else if (command == "CHTD") {
        std::string jsonStr = networkReadString(clientSocket);
        networkCanvasCurrentChatState->fromJson(jsonStr);
        mainThreadOps.add([this]() {
            if (networkCanvasChatPanel != NULL) {
                networkCanvasChatPanel->updateChat();
            }
        });
    }
    else if (command == "HSDC") {
        std::string disconnectReason = networkReadString(clientSocket);
        g_addNotificationFromThread(ErrorNotification(TL("vsp.collabeditor.error.disconnected"), disconnectReason));
    }
}

void NetworkCanvasMainEditor::networkCanvasSendInfoRequest()
{
    networkSendCommand(clientSocket, "INFO");
    json infoJson = {
        {"clientName", thisClientInfo->clientName},
        {"cursorX", mousePixelTargetPoint.x},
        {"cursorY", mousePixelTargetPoint.y},
        {"clientColor", frmt("{:06X}", thisClientInfo->clientColor&0xFFFFFF)}
    };
    networkSendString(clientSocket, infoJson.dump());

}

void NetworkCanvasMainEditor::networkCanvasSendLocalChanges()
{
    clientSideChangesMutex.lock();
    for (auto& c : clientSideChanges) {
        networkCanvasSendLRDT(clientSocket, c.first, layers[c.first]);
    }
    clientSideChanges.clear();
    clientSideChangesMutex.unlock();
}

void NetworkCanvasMainEditor::networkCanvasSendNewLayerRequest()
{
    //todo
}

void NetworkCanvasMainEditor::networkCanvasSendAUTH()
{
    networkSendCommand(clientSocket, "AUTH");
    json authJson = {
        {"password", networkCanvasPassword}
    };
    networkSendString(clientSocket, authJson.dump());
}

void NetworkCanvasMainEditor::reallocLayers(XY size, int numLayers)
{
    for (Layer*& layer : layers) {
        delete layer;
    }
    layers.clear();
    for (int i = 0; i < numLayers; i++) {
        Layer* newLayer = new Layer(size.x, size.y);
        newLayer->name = "Network layer";
        layers.push_back(newLayer);
    }
    if (selLayer >= layers.size()) {
        selLayer = ixmax(0, layers.size() - 1);
    }
    mainThreadOps.add([this]() {
        initLayers();
    });
}

NetworkCanvasMainEditor::NetworkCanvasMainEditor(std::string displayIP, PopupSetNetworkCanvasData userData, NET_StreamSocket* socket)
{
    targetIP = displayIP;
    thisClientInfo = new NetworkCanvasClientInfo();
    thisClientInfo->clientIP = "localhost";
    thisClientInfo->clientName = userData.username;
    thisClientInfo->clientColor = userData.userColor;
    networkCanvasPassword = userData.password;
    networkRunning = true;
    canvas.dimensions = { 0,0 };
    clientSocket = socket;
    clientThread = new std::thread(&NetworkCanvasMainEditor::networkCanvasClientThread, this);

    setUpWidgets();
    recenterCanvas();
    initLayers();

    networkCanvasCurrentChatState = new NetworkCanvasChatState;
    networkCanvasChatPanel = new EditorNetworkCanvasChatPanel(this);
    networkCanvasChatPanelContainer = new CollapsableDraggablePanel(TL("vsp.maineditor.panel.netcollab.chat.title"), networkCanvasChatPanel);
    networkCanvasChatPanelContainer->position.y = 64;
    networkCanvasChatPanelContainer->position.x = 730;
    wxsManager.addDrawable(networkCanvasChatPanelContainer);

    networkCanvasHostPanel = new EditorNetworkCanvasHostPanel(this, true);
    networkCanvasHostPanelContainer = new CollapsableDraggablePanel(TL("vsp.maineditor.panel.netcollab.host.title"), networkCanvasHostPanel);
    networkCanvasHostPanelContainer->position.y = 64;
    networkCanvasHostPanelContainer->position.x = 420;
    wxsManager.addDrawable(networkCanvasHostPanelContainer);
}

void NetworkCanvasMainEditor::networkCanvasStateUpdated(int whichLayer)
{
    if (whichLayer == -1) {
        whichLayer = selLayer;
    }
    clientSideChangesMutex.lock();
    clientSideChanges[whichLayer] = true;
    clientSideChangesMutex.unlock();
}

void NetworkCanvasMainEditor::networkCanvasChatSendCallback(std::string content)
{
    chatMsgQueueMutex.lock();
    chatMsgQueue.push(content);
    chatMsgQueueMutex.unlock();
}

Layer* NetworkCanvasMainEditor::newLayer()
{
    networkCanvasSendNewLayerRequest();
    return NULL;
}

void NetworkCanvasMainEditor::deleteLayer(int index)
{
    postErrorHostOnly();
}

void NetworkCanvasMainEditor::moveLayerUp(int index)
{
    postErrorHostOnly();
}

void NetworkCanvasMainEditor::moveLayerDown(int index)
{
    postErrorHostOnly();
}

void NetworkCanvasMainEditor::mergeLayerDown(int index)
{
    postErrorHostOnly();
}

void NetworkCanvasMainEditor::duplicateLayer(int index)
{
    postErrorHostOnly();
}

void NetworkCanvasMainEditor::rescaleAllLayersFromCommand(XY size)
{
    postErrorHostOnly();
}

void NetworkCanvasMainEditor::resizeAllLayersFromCommand(XY size, bool byTile)
{
    postErrorHostOnly();
}

void NetworkCanvasMainEditor::resizzeAllLayersByTilecountFromCommand(XY size)
{
    postErrorHostOnly();
}

void NetworkCanvasMainEditor::integerScaleAllLayersFromCommand(XY scale, bool downscale)
{
    postErrorHostOnly();
}

void NetworkCanvasMainEditor::promptStartNetworkSession()
{
    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Already in a network session"));
}

void NetworkCanvasMainEditor::postErrorHostOnly()
{
    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.collabeditor.error.hostonly")));
}

void NetworkCanvasMainEditor::endClientNetworkSession()
{
    networkRunning = false;
    if (clientThread != NULL) {
        clientThread->join();
        delete clientThread;
        clientThread = NULL;
    }
    if (thisClientInfo != NULL) {
        delete thisClientInfo;
        thisClientInfo = NULL;
    }
    if (networkCanvasCurrentChatState != NULL) {
        networkCanvasChatPanel = NULL;
        delete networkCanvasCurrentChatState;
        networkCanvasCurrentChatState = NULL;
    }
}
