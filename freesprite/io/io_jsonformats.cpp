#include "io_base.h"
#include "io_jsonformats.h"
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


MainEditor* readPIXIL(PlatformNativePathString path)
{
    std::ifstream f(path);
    if (f.is_open()) {
        try {
            json j = json::parse(f);
            f.close();

            std::string antiReverseStr1 = "/sfR5H8Fkddasdmnacvx/";
            //* there's also "p98kjasdnasd983/24kasdjasd" but it gets inserted between data:image/png and base64,
            //  which voidsprite completely skips (correct behavior is to replace this string with ; symbol)
            //* there's also "/8745jkhasdASD945kjknhj/" marked in the code as "this.importExportIsPhotoUpload"

            int canvasW = j["width"];
            int canvasH = j["height"];
            std::string name = j["name"];
            auto frameData = j["frames"];
            //todo:animations when we get there
            auto firstFrame = frameData[0];
            int selectedLayer = firstFrame["selectedLayer"];

            std::vector<Layer*> layers;
            for (auto& layer : firstFrame["layers"]) {
                std::string layerName = layer["name"];
                double opacity = 1.0;
                try {
                    double opacity = std::stod(layer["opacity"].get<std::string>());
                }
                catch (std::exception&) {
                    double opacity = layer["opacity"].get<double>();
                }
                std::string base64Data = layer["src"];

                auto findAntiReStr1 = base64Data.find(antiReverseStr1);
                while (findAntiReStr1 != std::string::npos) {
                    base64Data.erase(findAntiReStr1, antiReverseStr1.size());
                    findAntiReStr1 = base64Data.find(antiReverseStr1);
                }

                Layer* l = readPNGFromBase64String(base64Data);
                if (l != NULL) {
                    l->name = layerName;
                    l->layerAlpha = (u8)(opacity * 255);
                    layers.push_back(l);
                }
                else {
                    logerr("Failed to read layer from pixil file");
                }
            }

            return !layers.empty() ? new MainEditor(layers) : NULL;
        }
        catch (std::exception&) {
            return NULL;
        }
    }
    return NULL;
}

bool writePIXIL(PlatformNativePathString path, MainEditor* editor)
{
    std::ofstream outfile(path);

    if (outfile.is_open()) {
        json o = json::object();


        outfile << o.dump();
        outfile.close();
        return true;
    }
    return false;
}


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

MainEditor* readJPixel(PlatformNativePathString path)
{
    std::ifstream f(path);
    if (f.is_open()) {
        json j = json::parse(f);
        f.close();

        XY dimensions = { j["w"].get<int>(), j["h"].get<int>()};

        int unit = j["unit"];
        double animSpeed = j["speed"];
        int animation = j["animation"];

        std::vector<Frame*> frames;
        auto& frameData = j["frames"];
        for (auto& f : frameData) {
            Frame* nFrame = new Frame();
            frames.push_back(nFrame);

            auto& layers = f["layers"];
            for (auto& l : layers) {
                Layer* nLayer = Layer::tryAllocLayer(dimensions.x, dimensions.y);
                if (nLayer != NULL) {
                    nFrame->layers.insert(nFrame->layers.begin(), nLayer);
                    try {
                        nLayer->name = l["name"];
                        nLayer->hidden = !l["visible"];
                    }
                    catch (std::exception&) {}

                    Layer* color = readPNGFromBase64String(l["color"]);
                    Layer* alpha = readPNGFromBase64String(l["alpha"]);
                    if (color != NULL) {
                        if (color->isPalettized) {
                            Layer* conv = ((LayerPalettized*)color)->toRGB();
                            delete color;
                            color = conv;
                        }
                        nLayer->blit(color, { 0,0 }, { 0,0,dimensions.x, dimensions.y }, true);
                        delete color;
                    }

                    if (alpha != NULL) {
                        for (int x = 0; x < dimensions.x; x++) {
                            for (int y = 0; y < dimensions.y; y++) {
                                u8 alphaValue = alpha->getVisualPixelAt({ x,y }) >> 16;
                                //red channel is the alpha value

                                nLayer->setPixel({ x,y }, modAlpha(nLayer->getPixelAt({ x,y }), alphaValue));
                            }
                        }
                        delete alpha;
                    }
                }
                else {
                    g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
                }
            }
        }
        if (!frames.empty()) {
            MainEditor* editor = new MainEditor(frames);
            editor->frameAnimMSPerFrame = (int)(animSpeed * 1000);
            if (unit != 1) {
                editor->tileDimensions = { unit,unit };
            }
            return editor;
        }
    }
    return NULL;
}
