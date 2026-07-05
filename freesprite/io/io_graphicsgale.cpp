#include "io_base.h"
#include "io_graphicsgale.h"

#include "../FileIO.h" 

#include "../pugixml/pugixml.hpp"

std::vector<u8> galNextBufferPair(FILE* f)
{
    u32 size;
    fread(&size, 4, 1, f);
    if (size == 0) {
        return {};
    }
    std::vector<u8> data;
    data.resize(size);
    fread(data.data(), 1, size, f);
    return decompressZlibWithoutUncompressedSize(data.data(), data.size());
}

Layer* galDecode24bit(int w, int h, std::vector<u8>& pixelData)
{
    Layer* l = Layer::tryAllocLayer(w, h);
    u32* px32 = l->pixels32();
    u64 pxPos = 0;
    if (l != NULL) {
        u8* ptr = pixelData.data();
        u64 pos = 0;
        int padBytes = (4 - ((w * 3) % 4)) % 4;

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                if (pos >= pixelData.size()) {
                    logwarn("going outside data?");
                    break;
                }
                u8 b = ptr[pos++];
                u8 g = ptr[pos++];
                u8 r = ptr[pos++];
                px32[pxPos++] = PackRGBAtoARGB(r, g, b, 255);
            }
            pos += padBytes;
        }
        loginfo("parsed 24bit image");
    }
    return l;
}

void galApplyAlphaMap(Layer* rgb, Layer* alphaMap) {
    if (xyEqual({ rgb->w, rgb->h }, { alphaMap->w, alphaMap->h })) {
        u32* rgbPx = rgb->pixels32();
        u32* alphaPx = alphaMap->pixels32();
        for (u64 ptr = 0; ptr < (u64)rgb->w * rgb->h; ptr++) {
            u8 alpha = (u8)alphaPx[ptr];
            rgbPx[ptr] = modAlpha(rgbPx[ptr], alpha);
        }
    }
    else {
        logerr("[GALE] applyAlphaMap dimensions not equal");
    }
}

Layer* galDecode8Bit(int w, int h, std::vector<u8>& pixelData) {
    LayerPalettized* l = LayerPalettized::tryAllocIndexedLayer(w, h);
    u32* px32 = l->pixels32();
    u64 pxPos = 0;
    if (l != NULL) {
        u8* ptr = pixelData.data();
        u64 pos = 0;
        int padBytes = (4 - ((w * 3) % 4)) % 4;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                if (pos >= pixelData.size()) {
                    logwarn("going outside data?");
                    break;
                }
                u8 index = ptr[pos++];
                px32[pxPos++] = (u32)index;
            }
            ptr += padBytes;
        }
    }
    return l;
}

MainEditor* readGAL(PlatformNativePathString path, OperationProgressReport* progress) {
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        DoOnReturn closeFile([f]() {fclose(f); });

        char magic[9];
        fread(magic, 8, 1, f);
        magic[8] = '\0';
        if (memcmp(magic, "GaleX200", 8) != 0) {
            logerr(frmt("[GALE] Unsupported GALE header ({})", std::string(magic)));
            g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), frmt("Unsupported version: {}", std::string(magic))));
            return NULL;
        }

        std::vector<u8> xmlBuffer = galNextBufferPair(f);
        std::string xmlStr = std::string(xmlBuffer.begin(), xmlBuffer.end());
        loginfo(frmt("[GALE] xml:\n{}", xmlStr));

        pugi::xml_document doc;
        doc.load_buffer(xmlBuffer.data(), xmlBuffer.size());

        pugi::xml_node root = doc.document_element();
        int w = root.attribute("Width").as_int();
        int h = root.attribute("Height").as_int();
        int bpp = root.attribute("Bpp").as_int();
        loginfo(frmt("[GALE] {}x{} bpp:{}", w, h, bpp));
        if (bpp != 24) {
            g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), frmt("Unsupported BPP: {}", bpp)));
            logerr("[GALE] unsupported bpp");
            return NULL;
        }

        std::vector<Frame*> frames;

        int sumDelay = 0;

        for (auto& frameNode : root.children()) {
            Frame* fr = new Frame();
            frames.push_back(fr);
            sumDelay += frameNode.attribute("Delay").as_int();

            auto layersRoot = frameNode.child("Layers");
            for (auto& layerNode : layersRoot.children()) {
                auto layerBuffer = galNextBufferPair(f);
                //i can't be bothered to figure out how this padding works
                /*for (int skip = 0; skip < 4; skip++) {
                    int g = fgetc(f);
                    if (g != 0) {
                        ungetc(g, f);
                        break;
                    }
                    loginfo(frmt("[GALE] skipping padding at {}...", ftell(f)));
                }*/

                Layer* decoded = galDecode24bit(w, h, layerBuffer);

                //int alphaOn = layerNode.attribute("AlphaOn").as_int();

                auto alphaBuffer = galNextBufferPair(f);
                if (alphaBuffer.size() > 0) {
                    Layer* a = galDecode8Bit(w, h, alphaBuffer);
                    galApplyAlphaMap(decoded, a);
                    delete a;
                }
                else {
                    std::string transColor = layerNode.attribute("TransColor").as_string();
                    if (transColor != "-1") {
                        u32 transparentColor = layerNode.attribute("TransColor").as_uint() | 0xFF000000;
                        decoded->replaceColor(transparentColor, 0);
                    }
                }
                decoded->layerAlpha = (u8)layerNode.attribute("Alpha").as_int();
                decoded->hidden = layerNode.attribute("Visible").as_int() == 0;
                decoded->name = layerNode.attribute("Name").as_string();
                fr->layers.push_back(decoded);
            }
        }
        return new MainEditor(frames);
    }
    return NULL;
}