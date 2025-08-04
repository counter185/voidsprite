#include "NetworkCanvasMainEditor.h"
#include "FileIO.h"
#include <thread>
#include <SDL3_net/SDL_net.h>
#include "json/json.hpp"

using namespace nlohmann;

void NetworkCanvasMainEditor::networkCanvasClientThread()
{
    networkCanvasSendInfoRequest();
    while (networkRunning) {
        std::string command = networkReadCommand(clientSocket);
        networkCanvasProcessCommandFromServer(command);
    }
    NET_DestroyStreamSocket(clientSocket);
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
        auto layerData = infoJson["layers"];
        int numLayers = layerData.size();
        if (!receivedInfo || !xyEqual(canvasSize, lastINFOSize) || lastINFONumLayers != numLayers) {
            receivedInfo = true;
            lastINFOSize = canvasSize;
            lastINFONumLayers = numLayers;
            reallocLayers(canvasSize, numLayers);
        }

        int i = 0;
        for (auto& l : layerData) {
            Layer* layer = layers[i++];
            layer->name = l["name"];
            layer->layerAlpha = l["opacity"];
            layer->hidden = l["hidden"];
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
                logerr(std::format("Decompressed data size mismatch: expected {}, got {}", l->w * l->h * 4, decompressed.size()));
            }
            else {
                memcpy(l->pixels8(), decompressed.data(), 4ull * l->w * l->h);
                l->markLayerDirty();
            }
            tracked_free(dataBuffer);
        }
        else {
            logerr("Failed to allocate memory for layer pixel data update");
        }
    }
}

void NetworkCanvasMainEditor::networkCanvasSendInfoRequest()
{
    networkSendCommand(clientSocket, "INFO");
    json infoJson = {
        {"clientName", thisClientInfo.clientName},
        {"cursorX", 0},
        {"cursorY", 0}
    };

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
    initLayers();
}

NetworkCanvasMainEditor::NetworkCanvasMainEditor(NET_StreamSocket* socket)
{
    networkRunning = true;
    canvas.dimensions = { 0,0 };
    clientSocket = socket;
    clientThread = new std::thread(&NetworkCanvasMainEditor::networkCanvasClientThread, this);

    setUpWidgets();
    recenterCanvas();
    initLayers();
}
