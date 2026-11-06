#include <zlib.h>

#include "io_base.h"
#include "io_pixelstudio.h"
#include "io_png.h"
#include "../FileIO.h"

#include "../json/json.hpp"
#include "../base64/base64.hpp"

#include "../PopupMessageBox.h"

using namespace nlohmann;

#define PIXELSTUDIO_DEBUG_HISTORY_REENACTMENT 0
#if PIXELSTUDIO_DEBUG_HISTORY_REENACTMENT
#include "io_avif.h"
#endif

XY psToVSPPos(XY a, int height) {
    return { a.x, height - 1 - a.y };
}

void pixelStudioDumpProblematicToolInfo(int id, std::vector<XY> positions, std::string fullJson) {
    logwarn(frmt("[PixelStudio] problematic tool id {}", id));
    for (auto& xy : positions) {
        logwarn(frmt("\t  - {}:{}", xy.x, xy.y));
    }
    logwarn(frmt("\tFull json:{}", fullJson));
}

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
    loginfo(frmt("[PixelStudio] Clip Name: {}", name));
    int activeFrameIndex = clip0["ActiveFrameIndex"].get<int>();
    loginfo(frmt("[PixelStudio] Active Frame Index: {}", activeFrameIndex));

    bool showWarning = false;

    std::vector<Frame*> retFrames;
    json frames = clip0["Frames"];

    for (json& frame : frames) {
        Frame* curFrame = new Frame();
        retFrames.push_back(curFrame);

        int n = 0;
        for (json& layer : frame["Layers"]) {

            std::string id = layer["Id"];
            std::string layerName = "Pixel Studio Layer";
            try {
                layerName = layer["Name"];
            }
            catch (std::exception&) {}
            bool hidden = layer["Hidden"];
            std::string history = layer["_historyJson"];

            json subJson = json::parse(history);
            std::string source = subJson["_source"];
            Layer* nlayer = readPNGFromBase64String(source);
            
            if (nlayer != NULL) {
                nlayer->hidden = hidden;
                nlayer->name = layerName;
                //std::cout << subJson.dump(4) << std::endl;
                json actions = subJson["Actions"];
                int actionIndex = subJson["Index"];
                int nAction = 0;

#if PIXELSTUDIO_DEBUG_HISTORY_REENACTMENT
                AVIFVideoEncoder* debugEnc = new AVIFVideoEncoder();
                debugEnc->startRecording(convertStringOnWin32(frmt("test_layer_{}.avif", n++)), 200, 100);
#endif

                for (json& action : actions) {
                    if (nAction++ >= actionIndex) {
                        break;
                    }

#if PIXELSTUDIO_DEBUG_HISTORY_REENACTMENT
                    debugEnc->submitFrame(nlayer);
#endif

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
                    s16* positionVals = (s16*)positionsB64.data();
                    std::vector<XY> positions;
                    for (int p = 0; p < positionsB64.size() / 4; p++) {
                        s16 x = *(positionVals++);
                        s16 y = *(positionVals++);
                        positions.push_back(psToVSPPos({ x,y }, dimensions.y));
                    }
#if _DEBUG
                    //loginfo(action.dump(4));
#endif
                    switch (tool) {
                    //1px pencil
                    case 0:
                    //eraser pen (erase if clicked opaque, draw if clicked transparent)
                    case 19:
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
                        XY from = psToVSPPos({ meta["From"]["X"], (int)meta["From"]["Y"] }, dimensions.y);
                        XY to = psToVSPPos({ meta["To"]["X"], (int)meta["To"]["Y"] }, dimensions.y);

                        nlayer->fillRect(from, to, 0x00000000);
                        /*for (XY& p : positions) {
                            nlayer->setPixel(p, 0x00000000);
                        }*/
                    }
                    break;
                    //move
                    case 10:
                    {
                        XY delta = xySubtract(positions[1], positions[0]);
                        if (action.contains("Meta")) {

                            std::string metaString = action["Meta"];
                            json meta = json::parse(metaString);
                            XY from = psToVSPPos({ meta["From"]["X"], (int)meta["From"]["Y"] }, dimensions.y);
                            XY to = psToVSPPos({ meta["To"]["X"], (int)meta["To"]["Y"] }, dimensions.y);

                            SDL_Rect rect = {
                                ixmin(from.x, to.x),
                                ixmin(from.y, to.y),
                                abs(from.x - to.x) + 1,
                                abs(from.y - to.y) + 1
                            };

                            //copy current pixels to temp
                            u32* pixelData = (u32*)tracked_malloc(rect.w * rect.h * 4);
                            for (int y = 0; y < rect.h; y++) {
                                for (int x = 0; x < rect.w; x++) {
                                    ARRAY2DPOINT(pixelData, x, y, rect.w) = nlayer->getPixelAt({ rect.x + x, rect.y + y });
                                }
                            }

                            //0fill selected area
                            for (int y = 0; y < rect.h; y++) {
                                for (int x = 0; x < rect.w; x++) {
                                    nlayer->setPixel({ rect.x + x, rect.y + y }, 0);
                                }
                            }

                            //paste temp pixels into dst position
                            XY blitAt = xyAdd({ rect.x, rect.y }, delta);
                            for (int y = 0; y < rect.h; y++) {
                                for (int x = 0; x < rect.w; x++) {
                                    u32 srcColor = ARRAY2DPOINT(pixelData, x, y, rect.w);
                                    if (srcColor >> 24 != 0) {
                                        nlayer->setPixel(xyAdd(blitAt, { x,y }), srcColor);
                                    }
                                }
                            }
                            tracked_free(pixelData);
                        }
                        else if (positions.size() > 2) {
                            //non-rect move.
                            //move positions from [2]-... by delta
                            std::vector<u32> colorVals;
                            colorVals.resize(positions.size() - 2);
                            u32* c = colorVals.data();

                            //store color values and erase src
                            for (int i = 2; i < positions.size(); i++) {
                                c[i-2] = nlayer->getPixelAt(positions[i]);
                                nlayer->setPixel(positions[i], 0);
                            }

                            //paste color values
                            for (int i = 2; i < positions.size(); i++) {
                                nlayer->setPixel(xyAdd(positions[i], delta), c[i - 2]);
                            }
                        }
                        //pixelStudioDumpProblematicToolInfo(tool, positions, action.dump());
                        //showWarning = true;
                    }
                    break;
                    //flip x
                    case 13:
                    {
                        std::string metaString = action["Meta"];
                        json meta = json::parse(metaString);
                        XY from = psToVSPPos({ meta["From"]["X"], (int)meta["From"]["Y"] }, dimensions.y);
                        XY to = psToVSPPos({ meta["To"]["X"], (int)meta["To"]["Y"] }, dimensions.y);

                        SDL_Rect rect = { ixmin(from.x, to.x),ixmin(from.y, to.y),abs(from.x - to.x) + 1,abs(from.y - to.y) + 1 };
                        nlayer->flipHorizontally(rect);
                    }
                    break;
                    //flip y
                    case 14:
                    {
                        std::string metaString = action["Meta"];
                        json meta = json::parse(metaString);
                        XY from = psToVSPPos({ meta["From"]["X"], (int)meta["From"]["Y"] }, dimensions.y);
                        XY to = psToVSPPos({ meta["To"]["X"], (int)meta["To"]["Y"] }, dimensions.y);

                        SDL_Rect rect = { ixmin(from.x, to.x),ixmin(from.y, to.y),abs(from.x - to.x) + 1,abs(from.y - to.y) + 1 };
                        nlayer->flipVertically(rect);
                    }
                    break;
                    //replace color
                    case 18:
                        for (XY& p : positions) {
                            nlayer->replaceColor(nlayer->getPixelAt(p), colors[0]);
                        }
                        pixelStudioDumpProblematicToolInfo(tool, positions, action.dump());
                        //showWarning = true;
                        break;
                    //image paste
                    case 20:
                    {
                        std::string subsubJson = action["Meta"];
                        json subsubJsonJ = json::parse(subsubJson);
                        std::string pixels = subsubJsonJ["Pixels"];
                        Layer* nnlayer = readPNGFromBase64String(pixels);

                        XY rectFrom = psToVSPPos({ subsubJsonJ["Rect"]["From"]["X"], (int)subsubJsonJ["Rect"]["From"]["Y"] }, dimensions.y);                    
                        XY rectTo =   psToVSPPos({ subsubJsonJ["Rect"]["To"]["X"], (int)subsubJsonJ["Rect"]["To"]["Y"] }, dimensions.y);

                        SDL_Rect dstRect = {
                            ixmin(rectFrom.x, rectTo.x),
                            ixmin(rectFrom.y, rectTo.y),
                            abs(rectFrom.x - rectTo.x),
                            abs(rectFrom.y - rectTo.y)
                        };

                        XY rectSourceFrom = { subsubJsonJ["RectSource"]["From"]["X"], subsubJsonJ["RectSource"]["From"]["Y"] };
                        XY rectSourceTo =   { subsubJsonJ["RectSource"]["To"]["X"], subsubJsonJ["RectSource"]["To"]["Y"] };

                        //XY rectSourceFrom = psToVSPPos({ subsubJsonJ["RectSource"]["From"]["X"], subsubJsonJ["RectSource"]["From"]["Y"] }, nnlayer->h);
                        //XY rectSourceTo =   psToVSPPos({ subsubJsonJ["RectSource"]["To"]["X"], subsubJsonJ["RectSource"]["To"]["Y"] }, nnlayer->h);

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
                        pixelStudioDumpProblematicToolInfo(tool, positions, action.dump());
                        //showWarning = true;
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
                        pixelStudioDumpProblematicToolInfo(tool, positions, action.dump());
                        showWarning = true;

                    }
                    break;
                    default:
                        g_addNotification(ErrorNotification("PixelStudio Error", frmt("Tool {} not implemented", tool)));
                        logerr(frmt("[pixel studio PSP] TOOL {} NOT IMPLEMENTED", tool));
                        logerr("\trelevant position data:");
                        for (XY& p : positions) {
                            logerr(frmt("\t{}, {}", p.x, p.y));
                        }
                        logerr("\trelevant color data:");
                        for (u32& c : colors) {
                            logerr(frmt("\t{:x}", c));
                        }
                        logerr(action.dump(4));
                        showWarning = true;
                        break;
                    }
                }

#if PIXELSTUDIO_DEBUG_HISTORY_REENACTMENT
                debugEnc->submitFrame(nlayer);
                debugEnc->stopRecording();
                delete debugEnc;
#endif

                curFrame->layers.push_back(nlayer);
            }
        }
    }
    MainEditor* ret = new MainEditor(retFrames);
    if (showWarning && g_interactiveContext) {
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
    o["TileFade"] = data->ssne.tileGridAlpha;
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

    for (Layer*& l : data->getLayerStack()) {
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

        std::string pixelDataPNGAsBase64 = writePNGToBase64(l);
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
    std::ifstream f = platformOpenIFStream(path);
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
    std::ofstream outfile = platformOpenOFStream(path);

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

IPalette* readPltPixelStudioPALETTE(PlatformNativePathString name)
{
    FILE* f = platformOpenFile(name, PlatformFileModeR);
    if (f != NULL) {
        DoOnReturn closeFile([f]() {fclose(f); });
        //auto fileData = readWholeFile(f);
        json j = json::parse(f);

        std::vector<u32> ret;

        std::string name = j["Name"];
        std::string author = j["Author"];

        auto colors = j["Colors"];
        for (auto& color : colors) {
            auto r = color["r"].get<int>();
            auto g = color["g"].get<int>();
            auto b = color["b"].get<int>();
            auto a = color["a"].get<int>();

            ret.push_back(PackRGBAtoARGB(r, g, b, a));
        }

        ImportedPalette* p = new ImportedPalette("", ret);
        p->name = name;
        p->description = "by " + author;
        p->formatFlags = PALETTE_FORMAT_FEATURE_NAME;
        return p;
    }
    return NULL;
}