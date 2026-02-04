#include <regex>

#include "io_base.h"
#include "io_resprite.h"

#include "io_png.h"
#include "../zip/zip.h"
#include "../json/json.hpp"

using namespace nlohmann;

MainEditor* readResprite(PlatformNativePathString path)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        DoOnReturn closeFile([f]() {fclose(f); });
        zip_t* zip = zip_cstream_open(f, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
        if (zip == NULL) {
            return NULL;
        }

        std::string uuid;

        if (zip_entry_openbyindex(zip, 0) == 0)
        {
            DoOnReturn closeEntry([zip]() { zip_entry_close(zip); });

            const char* name = zip_entry_name(zip);
            //match <uuid>.resprite at the beginning of the name
            std::string nameStr(name);
            loginfo(frmt("entry 0: {}", nameStr));
            std::regex uuidRegex("([a-fA-F0-9\\-]+)\\.resprite");
            std::smatch match;
            if (std::regex_search(nameStr, match, uuidRegex)) {
                uuid = match[1];
            }
        }

        if (uuid.empty()) {
            logerr("[Resprite] failed to get project UUID");
            return NULL;
        }

        std::string projectRoot = frmt("{}.resprite/", uuid);

        json documentJson;
        std::string documentPath = frmt("{}document.json", projectRoot);
        if (zip_entry_open(zip, documentPath.c_str()) == 0) {
            DoOnReturn closeEntry([zip]() { zip_entry_close(zip); });
            
            try {
                u8* jsonData = NULL;
                size_t jsonSize;
                zip_entry_read(zip, (void**)&jsonData, &jsonSize);
                std::string jsonString;
                jsonString.resize(jsonSize);
                memcpy(jsonString.data(), jsonData, jsonSize);
                documentJson = json::parse(jsonString);
            }
            catch (std::exception& e) {
                logerr(frmt("[Resprite] Failed to parse document.json:\n {}", e.what()));
            }
        }
        else {
            logerr("[Resprite] failed to open document.json");
            return NULL;
        }
        
        XY canvasSize = {
            documentJson["canvasSize"]["width"].get<int>(),
            documentJson["canvasSize"]["height"].get<int>()
        };
        int frameCount = documentJson["frameCount"];

        auto generatorInfo = documentJson["generatorInfo"].dump();
        loginfo(frmt("Generator info:\n {}", generatorInfo));

        auto layerDatas = documentJson["layerDatas"];

        std::vector<Frame*> frames;

        for (int i = 0; i < frameCount; i++) {
            Frame* nFrame = new Frame();

            for (auto& layer : layerDatas) {
                std::string name = layer["name"];
                double opacity = layer["opacity"];

                auto& cell = layer["cells"][i];
                std::string cellID = cell["id"];
                double cellOpacity = cell["opacity"];
                SDL_Rect bounds = {
                    cell["bounds"]["origin"]["x"].get<int>(),
                    cell["bounds"]["origin"]["y"].get<int>(),
                    cell["bounds"]["size"]["width"].get<int>(),
                    cell["bounds"]["size"]["height"].get<int>(),
                };

                if (bounds.w > 0 && bounds.h > 0) {

                    Layer* nnLayer = Layer::tryAllocLayer(canvasSize.x, canvasSize.y);
                    nnLayer->name = name;
                    nnLayer->layerAlpha = (u8)(opacity * cellOpacity * 255);
                    nFrame->layers.push_back(nnLayer);

                    std::string cellPath = frmt("{}CellData/{}.png", projectRoot, cellID);
                    if (zip_entry_open(zip, cellPath.c_str()) == 0) {
                        DoOnReturn closeCellEntry([zip]() { zip_entry_close(zip); });
                        uint8_t* pngData = NULL;
                        size_t pngSize;
                        zip_entry_read(zip, (void**)&pngData, &pngSize);
                        Layer* nLayer = readPNGFromMem(pngData, pngSize);
                        if (nLayer != NULL) {
                            
                            nnLayer->blit(nLayer, { bounds.x, bounds.y }, {0,0,bounds.w,bounds.h}, true);
                            delete nLayer;
                        }
                        else {
                            logerr(frmt("[Resprite] failed to read layer PNG data for layer '{}'", name));
                        }

                    }
                    else {
                        logerr(frmt("[Resprite] failed to open cell PNG at path: {}", cellPath));
                    }
                }
            }

            frames.push_back(nFrame);
        }

        if (!frames.empty()) {
            MainEditor* ret = new MainEditor(frames);
            ret->tileDimensions = documentJson["showGrid"].get<bool>() ? XY{
                documentJson["gridWidth"].get<int>(), documentJson["gridHeight"].get<int>()
            } : XY{0,0};
            ret->switchFrame(ixmin(documentJson["currFrameNo"].get<int>(), ret->frames.size() - 1));
            ret->switchActiveLayer(ixmin(documentJson["currLayerNo"].get<int>(), ret->getCurrentFrame()->layers.size() - 1));
            return ret;
        }
    }
    return NULL;
}
