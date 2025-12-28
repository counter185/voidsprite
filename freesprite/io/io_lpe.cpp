#include "io_base.h"
#include "io_lpe.h"
#include "io_png.h"

#include "../json/json.hpp"
#include "../base64/base64.hpp"

using namespace nlohmann;

MainEditor* readLPE(PlatformNativePathString path)
{
    std::ifstream f(path);
    if (f.is_open()) {
        json j = json::parse(f);
        f.close();

        int width = j["canvasWidth"];
        int height = j["canvasHeight"];
        //auto colors = j["colors"].array();
        int selLayer = j["selectedLayer"];

        std::vector<Layer*> layers;
        for (auto& layer : j["layers"]) {
            bool visible = layer["isVisible"];
            std::string name = layer["name"];
            std::string base64image = layer["src"];
            Layer* l = readPNGFromBase64String(base64image);
            if (l != NULL) {
                l->hidden = !visible;
                l->name = name;
                layers.insert(layers.begin(), l);
            }
        }
        if (layers.size() > 0) {
            MainEditor* editor = new MainEditor(layers);
            editor->selLayer = selLayer;
            return editor;
        }

    }
    return NULL;
}

bool writeLPE(PlatformNativePathString path, MainEditor* editor)
{
    std::ofstream outfile(path);

    if (outfile.is_open()) {
        json o = json::object();
        o["canvasWidth"] = editor->canvas.dimensions.x;
        o["canvasHeight"] = editor->canvas.dimensions.y;
        o["editorMode"] = "Advanced";
        o["colors"] = json::array();
        o["colors"].push_back(frmt("#{:06X}", editor->getActiveColor() & 0xFFFFFF));
        for (int x = 0; x < ixmin(32, editor->lastColors.size()); x++) {
            o["colors"].push_back(frmt("#{:06X}", editor->lastColors[x] & 0xFFFFFF));
        }
        o["selectedLayer"] = editor->selLayer;
        o["layers"] = json::array();
        int i = 0;
        for (auto it = editor->getLayerStack().rbegin(); it != editor->getLayerStack().rend(); it++) {
            Layer* l = *it;
            json layerObj = json::object();
            layerObj["canvas"] = json::object();
            layerObj["context"] = json::object();
            layerObj["context"]["mozImageSmoothingEnabled"] = false;
            layerObj["context"]["willReadFrequently"] = true;
            layerObj["isSelected"] = editor->selLayer == i;
            layerObj["isVisible"] = !l->hidden;
            layerObj["isLocked"] = false;
            layerObj["oldLayerName"] = nullptr;
            layerObj["menuEntry"] = json::object();
            layerObj["id"] = frmt("layer{}", i);
            layerObj["name"] = l->name;

            std::vector<u8> pngBytes = writePNGToMem(l);
            std::string fileBuffer;
            fileBuffer.resize(pngBytes.size());
            memcpy(fileBuffer.data(), pngBytes.data(), pngBytes.size());
            layerObj["src"] = "data:image/png;base64," + base64::to_base64(fileBuffer);

            o["layers"].push_back(layerObj);
            i++;
        }

        outfile << o.dump();
        outfile.close();
        return true;
    }
    return false;
}
