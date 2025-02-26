#include "io_base.h"
#include "io_piskel.h"

#include "json/json.hpp"
#include "base64/base64.hpp"

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
                auto hdr = pixels.find("iVBO"); // skip to header
                if (hdr != std::string::npos) {
					pixels = pixels.substr(hdr);
				}
                std::string pixelsb64 = base64::from_base64(pixels);
                uint8_t* imageData = (uint8_t*)pixelsb64.c_str();
                Layer* nnlayer = readPNGFromMem(imageData, pixelsb64.size());
                nnlayer->name = layerData.contains("name") ? layerData["name"] : "";
                double opacity = layerData["opacity"];
                nnlayer->layerAlpha = (u8)(255 * opacity);

                layers.push_back(nnlayer);
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
	return false;
}
