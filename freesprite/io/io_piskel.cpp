#include "io_base.h"
#include "io_piskel.h"
#include "io_png.h"

#include "../json/json.hpp"
#include "../base64/base64.hpp"

using namespace nlohmann;

MainEditor* readPISKEL(PlatformNativePathString path)
{
    std::ifstream f(path);
    if (f.is_open()) {
        json j = json::parse(f);
        f.close();

        int modelVersion = j["modelVersion"];
        if (j.contains("piskel")) {
            auto root = j["piskel"];
            std::string name = root["name"];
            int fps = root["fps"];
            int width = root["width"];
            int height = root["height"];

            std::vector<Layer*> layers;
            int lastFrameCount = 0;

            for (std::string layerJson : root["layers"]) {
                json layerData = json::parse(layerJson);
                int frameCount = layerData["frameCount"];
                lastFrameCount = frameCount;

                std::string pixels = layerData["chunks"][0]["base64PNG"];
                Layer* nnlayer = readPNGFromBase64String(pixels);
                if (nnlayer != NULL) {
                    nnlayer->name = layerData.contains("name") ? layerData["name"] : "";
                    double opacity = layerData["opacity"];
                    nnlayer->layerAlpha = (u8)(255 * opacity);

                    layers.push_back(nnlayer);
                }
                else {
                    logprintf("layer load failed\n");
                }
            }

            if (layers.size() > 0) {
				MainEditor* editor = new MainEditor(layers);
                if (lastFrameCount > 1) {
                    editor->tileDimensions = { width, height };
                }
				return editor;
			}
			else {
				return NULL;
			}
        }
        else {
            return NULL;
        }
    }
    return NULL;
}

bool writePISKEL(PlatformNativePathString path, MainEditor* editor)
{
    std::ofstream outfile(path);

    if (outfile.is_open()) {
        json o = json::object();
        o["modelVersion"] = 2;
        o["piskel"] = json::object();
        o["piskel"]["name"] = "voidsprite Image";
        o["piskel"]["description"] = "exported with voidsprite";
        o["piskel"]["fps"] = 24;
        o["piskel"]["height"] = editor->canvas.dimensions.y;
        o["piskel"]["width"] = editor->canvas.dimensions.x;
        o["piskel"]["layers"] = json::array();
        for (Layer*& l : editor->getLayerStack()) {
            std::string nestedLayer = "";
            json layerObj = json::object();
            layerObj["name"] = l->name;
            layerObj["opacity"] = (double)l->layerAlpha / 255.0;
            layerObj["frameCount"] = 1;
            layerObj["chunks"] = json::array();
            json chunkObj = json::object();

            std::vector<u8> pngData = writePNGToMem(l);
            std::string fileBuffer;
            fileBuffer.resize(pngData.size());
            memcpy(fileBuffer.data(), pngData.data(), pngData.size());
            chunkObj["base64PNG"] = "data:image/png;base64," + base64::to_base64(fileBuffer);
            chunkObj["layout"] = json::array();
            chunkObj["layout"].push_back(json::array());
            chunkObj["layout"][0].push_back(0);
            
            layerObj["chunks"].push_back(chunkObj);

            nestedLayer = layerObj.dump();

            o["piskel"]["layers"].push_back(nestedLayer);

        }
        o["piskel"]["hiddenFrames"] = json::array();

        outfile << o.dump();
        outfile.close();
        return true;
    }
    return false;
}
