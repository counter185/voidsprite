#include <zlib.h>

#include "io_base.h"
#include "io_pixelstudio.h"
#include "io_png.h"
#include "../FileIO.h"

#include "../json/json.hpp"
#include "../base64/base64.hpp"

#include "../PopupMessageBox.h"

using namespace nlohmann;

MainEditor* deserializePixelStudioSession(json j)
{
    int pspversion = j["Version"].get<int>();
    logprintf("Version: %i\n", pspversion);

    if (pspversion != 2) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Unsupported Pixel Studio file version"));
        return NULL;
    }

    XY dimensions = { j["Width"].get<int>(), j["Height"].get<int>() };

    json clips = j["Clips"];
    json clip0 = clips[0];
    std::string name = clip0["Name"];
    logprintf("Clip Name: %s\n", name.c_str());
    int activeFrameIndex = clip0["ActiveFrameIndex"].get<int>();
    logprintf("Active Frame Index: %i\n", activeFrameIndex);

    bool showWarning = false;

    json frames = clip0["Frames"][0]["Layers"];
    std::vector<Layer*> layers;
    for (json& frame : frames) {
        std::string id = frame["Id"];
        std::string layerName = "Pixel Studio Layer";
        try {
            layerName = frame["Name"];
        }
        catch (std::exception&) {}
        bool hidden = frame["Hidden"];
        std::string history = frame["_historyJson"];

        json subJson = json::parse(history);
        std::string source = subJson["_source"];
        std::string base64ImageData = base64::from_base64(source);
        uint8_t* imageData = (uint8_t*)base64ImageData.c_str();
        Layer* nlayer = readPNGFromMem(imageData, base64ImageData.size());
        if (nlayer != NULL) {

            nlayer->name = layerName;
            //std::cout << subJson.dump(4) << std::endl;
            json actions = subJson["Actions"];
            int actionIndex = subJson["Index"];
            int nAction = 0;
            for (json& action : actions) {
                if (nAction++ >= actionIndex) {
                    break;
                }

                bool invalid = action["Invalid"];
                if (invalid) {
                    continue;
                }
                int tool = action["Tool"];

                std::string colorsB64 = base64::from_base64(std::string(action["Colors"]));
                std::vector<u32> colors;
                for (int c = 0; c < colorsB64.size() / 4; c++) {
                    u32 color = 0;
                    u8 r = colorsB64[c * 4];
                    u8 g = colorsB64[c * 4 + 1];
                    u8 b = colorsB64[c * 4 + 2];
                    u8 a = colorsB64[c * 4 + 3];
                    color = PackRGBAtoARGB(r, g, b, a);
                    colors.push_back(color);
                }

                std::string positionsB64 = base64::from_base64(std::string(action["Positions"]));
                std::vector<XY> positions;
                for (int p = 0; p < positionsB64.size() / 4; p++) {
                    u16 x = *(u16*)(positionsB64.c_str() + (p * 4));
                    u16 y = dimensions.y - 1 - *(u16*)(positionsB64.c_str() + (p * 4 + 2));
                    positions.push_back(XY{ x,y });
                }
#if _DEBUG
                loginfo(action.dump(4));
#endif
                switch (tool) {
                    //1px pencil
                case 0:
                {
                    json colorIndexes = action["ColorIndexes"];
                    int colIndex = 0;
                    for (XY& p : positions) {
                        nlayer->setPixel(p, colors[colorIndexes.size() > colIndex ? (int)colorIndexes[colIndex] : 0]);
                        colIndex++;
                    }
                }
                break;
                    //color picker, has no values at all attached to it
                case 1:
                    break;
                    //eraser
                case 2:
                    //"eraser pen"
                case 19:
                    for (XY& p : positions) {
                        nlayer->setPixel(p, modAlpha(colors[0], 0));
                    }
                    break;
                    //paint bucket
                case 3:
                    for (XY& p : positions) {
                        nlayer->paintBucket(p, colors[0]);
                    }
                    break;
                    //erase selection
                case 6:
                {
                    std::string metaString = action["Meta"];
                    json meta = json::parse(metaString);
                    XY from = { meta["From"]["X"], dimensions.y - 1 - (int)meta["From"]["Y"] };
                    XY to = { meta["To"]["X"],  dimensions.y - 1 - (int)meta["To"]["Y"] };

                    nlayer->fillRect(from, to, 0x00000000);
                    /*for (XY& p : positions) {
                        nlayer->setPixel(p, 0x00000000);
                    }*/
                }
                break;
                //move
                case 10:
                {
                    std::string metaString = action["Meta"];
                    json meta = json::parse(metaString);
                    XY from = { meta["From"]["X"], dimensions.y - 1 - (int)meta["From"]["Y"] };
                    XY to = { meta["To"]["X"],  dimensions.y - 1 - (int)meta["To"]["Y"] };

                    SDL_Rect rect = { ixmin(from.x, to.x),ixmin(from.y, to.y),abs(from.x - to.x) + 1,abs(from.y - to.y) + 1 };

                    XY blitAt = positions[1];
                    u32* pixelData = (u32*)tracked_malloc(rect.w * rect.h * 4);
                    for (int y = 0; y < rect.h; y++) {
                        for (int x = 0; x < rect.w; x++) {
                            pixelData[y * rect.w + x] = nlayer->getPixelAt({ rect.x + x, rect.y + y });
                        }
                    }

                    for (int y = 0; y < rect.h; y++) {
                        for (int x = 0; x < rect.w; x++) {
                            nlayer->setPixel({ rect.x + x, rect.y + y }, 0);
                        }
                    }

                    for (int y = 0; y < rect.h; y++) {
                        for (int x = 0; x < rect.w; x++) {
                            u32 srcColor = pixelData[y * rect.w + x];
                            if (srcColor >> 24 != 0) {
                                nlayer->setPixel({ blitAt.x + x, blitAt.y + y }, srcColor);
                            }
                        }
                    }
                    showWarning = true;
                    tracked_free(pixelData);
                }
                break;
                //flip x
                case 13:
                {
                    std::string metaString = action["Meta"];
                    json meta = json::parse(metaString);
                    XY from = { meta["From"]["X"], dimensions.y - 1 - (int)meta["From"]["Y"] };
                    XY to = { meta["To"]["X"],  dimensions.y - 1 - (int)meta["To"]["Y"] };

                    SDL_Rect rect = { ixmin(from.x, to.x),ixmin(from.y, to.y),abs(from.x - to.x) + 1,abs(from.y - to.y) + 1 };
                    nlayer->flipHorizontally(rect);
                }
                break;
                //flip y
                case 14:
                {
                    std::string metaString = action["Meta"];
                    json meta = json::parse(metaString);
                    XY from = { meta["From"]["X"], dimensions.y - 1 - (int)meta["From"]["Y"] };
                    XY to = { meta["To"]["X"],  dimensions.y - 1 - (int)meta["To"]["Y"] };

                    SDL_Rect rect = { ixmin(from.x, to.x),ixmin(from.y, to.y),abs(from.x - to.x) + 1,abs(from.y - to.y) + 1 };
                    nlayer->flipVertically(rect);
                }
                break;
                //replace color
                case 18:
                    for (XY& p : positions) {
                        nlayer->replaceColor(nlayer->getPixelAt(p), colors[0]);
                    }
                    break;
                    //image paste
                case 20:
                {
                    std::string subsubJson = action["Meta"];
                    json subsubJsonJ = json::parse(subsubJson);
                    //std::cout << subsubJsonJ.dump(4) << std::endl;
                    std::string pixels = subsubJsonJ["Pixels"];
                    std::string pixelsb64 = base64::from_base64(pixels);
                    uint8_t* imageData = (uint8_t*)pixelsb64.c_str();
                    Layer* nnlayer = readPNGFromMem(imageData, pixelsb64.size());

                    XY rectFrom = { subsubJsonJ["Rect"]["From"]["X"], dimensions.y - 1 - (int)subsubJsonJ["Rect"]["From"]["Y"] };
                    XY rectTo = { subsubJsonJ["Rect"]["To"]["X"], dimensions.y - 1 - (int)subsubJsonJ["Rect"]["To"]["Y"] };

                    SDL_Rect dstRect = {
                        ixmin(rectFrom.x, rectTo.x),
                        ixmin(rectFrom.y, rectTo.y),
                        abs(rectFrom.x - rectTo.x),
                        abs(rectFrom.y - rectTo.y)
                    };

                    XY rectSourceFrom = { subsubJsonJ["RectSource"]["From"]["X"], subsubJsonJ["RectSource"]["From"]["Y"] };
                    XY rectSourceTo = { subsubJsonJ["RectSource"]["To"]["X"], subsubJsonJ["RectSource"]["To"]["Y"] };

                    SDL_Rect srcRect = {
                        ixmin(rectSourceFrom.x, rectSourceTo.x),
                        ixmin(rectSourceFrom.y, rectSourceTo.y),
                        abs(rectSourceFrom.x - rectSourceTo.x),
                        abs(rectSourceFrom.y - rectSourceTo.y)
                    };
                    if (srcRect.w == 0 || srcRect.h == 0) {
                        srcRect.w = nnlayer->w;
                        srcRect.h = nnlayer->h;
                    }

                    nlayer->blit(nnlayer, { dstRect.x, dstRect.y }, srcRect);
                    showWarning = true;
                    delete nnlayer;
                }
                break;
                //case 21:
                    //rotate selection
                    //break;
                case 24:
                    //adjust HSL,
                    //data is in `"Meta": "[-15660,0,0]",`
                    //hue: max is 32400
                    //saturation, min is -10000
                {
                    loginfo(action.dump(4));
                    std::string metaStr = action["Meta"];
                    json meta = json::parse(metaStr);
                    int hue = meta[0];
                    int saturation = meta[1];
                    int lightness = meta[2];
                    hsl shift = {
                        hue / 32400.0f * 180.0f,
                        saturation / 10000.0f,
                        lightness / 10000.0f
                    };
                    logprintf("hsl shift by  h:%lf s:%lf l:%lf\n", shift.h, shift.s, shift.l);
                    u32* px32 = nlayer->pixels32();
                    for (u64 dataPtr = 0; dataPtr < nlayer->w * nlayer->h; dataPtr++) {
                        px32[dataPtr] = hslShiftPixelStudioCompat(px32[dataPtr], shift);
                    }
                    //nlayer->shiftLayerHSL(shift);

                }
                break;
                default:
                    g_addNotification(ErrorNotification("PixelStudio Error", frmt("Tool {} not implemented", tool)));
                    logprintf("[pixel studio PSP] TOOL %i NOT IMPLEMENTED\n", tool);
                    logprintf("\trelevant position data:\n");
                    for (XY& p : positions) {
                        logprintf("\t%i, %i\n", p.x, p.y);
                    }
                    logprintf("\trelevant color data:\n");
                    for (u32& c : colors) {
                        logprintf("\t%x\n", c);
                    }
                    loginfo(action.dump(4));
                    showWarning = true;
                    break;
                }
            }

            layers.push_back(nlayer);
        }

        /*FILE* tempf = platformOpenFile(convertStringOnWin32(id + "temp.bin"), PlatformFileModeWB);
        fwrite(base64ImageData.c_str(), base64ImageData.size(), 1, tempf);
        fclose(tempf);*/
    }
    MainEditor* ret = new MainEditor(layers);
    if (showWarning) {
        PopupMessageBox* warningPopup = new PopupMessageBox("Warning",
            "This is a file in a Pixel Studio Pro format.\n"
            "This format requires the whole undo history to be reenacted,\nso importing may not work directly.\n"
            "If anything looks incorrect, we suggest you do the following: \n\n"
            "  1. Load the file with Pixel Studio Pro\n"
            "  2. Open the Functions menu [F key]\n"
            "  3. Click \"Resize canvas\"\n"
            "  4. Click \"Resize\" without changing any values.\n"
            "  5. Export this file again and load it with voidsprite.", { 700, 290 });
        g_addPopup(warningPopup);
    }
    return ret;
}

json serializePixelStudioSession(MainEditor* data) {
    json o = json::object();
    o["Version"] = 2;
    o["Id"] = randomUUID();
    o["Name"] = "voidsprite Image";
    o["Width"] = data->canvas.dimensions.x;
    o["Height"] = data->canvas.dimensions.y;
    o["Type"] = 0;
    o["Background"] = true;
    o["BackgroundColor"] = { {"r", 0}, {"g", 0}, {"b", 0}, {"a", 0} };
    o["TileMode"] = false;
    o["TileFade"] = data->tileGridAlpha;
    o["ActiveClipIndex"] = 0;
    o["Clips"] = json::array();

    json clip = json::object();
    clip["Name"] = "Untitled";
    clip["LayerTypes"] = json::array();
    clip["ActiveFrameIndex"] = 0;
    clip["Frames"] = json::array();

    json frame = json::object();
    frame["Id"] = randomUUID();
    frame["Delay"] = 0.3;
    frame["ActiveLayerIndex"] = data->selLayer;
    frame["Layers"] = json::array();

    for (Layer*& l : data->layers) {
        json layer = json::object();
        layer["Id"] = randomUUID();
        layer["Name"] = l->name;
        layer["Transparency"] = l->layerAlpha / 255.0;
        layer["Hidden"] = l->hidden;
        layer["Linked"] = false;
        layer["Outline"] = 0;
        layer["Lock"] = 0;
        layer["Sx"] = 0;
        layer["Sy"] = 0;
        layer["Version"] = 1;

        json historyJson;
        historyJson["Actions"] = json::array();
        historyJson["Index"] = 0;

        std::string pixelDataPNGAsBase64 = "";
        std::vector<u8> pngData = writePNGToMem(l);
        std::string fileBuffer;
        fileBuffer.resize(pngData.size());
        memcpy(&fileBuffer[0], pngData.data(), pngData.size());
        pixelDataPNGAsBase64 = base64::to_base64(fileBuffer);
        historyJson["_source"] = pixelDataPNGAsBase64;

        layer["_historyJson"] = historyJson.dump();

        frame["Layers"].push_back(layer);
    }

    clip["Frames"].push_back(frame);

    o["Clips"].push_back(clip);
    return o;
}

MainEditor* readPixelStudioPSP(PlatformNativePathString path)
{
    std::ifstream f(path);
    if (f.is_open()) {
        try {
            json j = json::parse(f);
            f.close();

            return deserializePixelStudioSession(j);
        }
        catch (std::exception& e) {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to parse PSP JSON"));
            logerr(frmt("failed to deserialize pixel studio session:\n {}", e.what()));
            return NULL;
        }
    }
    return NULL;
}

MainEditor* readPixelStudioPSX(PlatformNativePathString path)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        DoOnReturn cl([f]() {fclose(f); });
        fseek(f, 0, SEEK_END);
        u64 fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);
        u8* fdata = (u8*)tracked_malloc(fileSize);
        if (fdata != NULL) {
            fread(fdata, 1, fileSize, f);

            std::vector<u8> decompressed = decompressZlibWithoutUncompressedSize(fdata, fileSize);
            tracked_free(fdata);
            std::string jsonString(decompressed.begin(), decompressed.end());

            try {
                json j = json::parse(jsonString);
                return deserializePixelStudioSession(j);
            }
            catch (std::exception& e) {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to parse PSX JSON"));
                logerr(frmt("failed to deserialize pixel studio session:\n {}", e.what()));
                return NULL;
            }
        }
    }
    return NULL;
}

bool writePixelStudioPSP(PlatformNativePathString path, MainEditor* data)
{
    std::ofstream outfile(path);

    if (outfile.is_open()) {
        json o = serializePixelStudioSession(data);

        outfile << o.dump();
        outfile.close();
        return true;
    }
    return false;
}

bool writePixelStudioPSX(PlatformNativePathString path, MainEditor* data)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    //std::ofstream outfile(path);

    if (f != NULL) {
        json o = serializePixelStudioSession(data);
        std::string jsonString = o.dump();

        //compress with zlib and write to file
        unsigned long maxCompressedSize = compressBound(jsonString.size());
        unsigned long compressedSize = maxCompressedSize;
        uint8_t* compressedData = (uint8_t*)tracked_malloc(maxCompressedSize);
        compress(compressedData, &compressedSize, (const Bytef*)jsonString.c_str(), jsonString.size());
        fwrite(compressedData, compressedSize, 1, f);
        fclose(f);
        tracked_free(compressedData);

        //outfile.close();
        return true;
    }
    return false;
}