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

std::string galMakeXML(MainEditor* session)
{
    std::string ret = "";
    ret += frmt("<Frames Version=\"200\" Width=\"{}\" Height=\"{}\" Bpp=\"24\" Count=\"{}\" SyncPal=\"1\" Randomized=\"0\" CompType=\"0\" CompLevel=\"0\" BGColor=\"16777215\" BlockWidth=\"0\" BlockHeight=\"0\" NotFillBG=\"1\">\n", 
        session->canvas.dimensions.x, session->canvas.dimensions.y, session->frames.size());

    for (Frame*& f : session->frames) {
        ret += frmt("  <Frame Name=\"%framenumber%\" TransColor=\"-1\" Delay=\"{}\" Disposal=\"2\">\n",
            session->frameAnimMSPerFrame);
        ret += frmt("    <Layers Count=\"{}\" Width=\"{}\" Height=\"{}\" Bpp=\"24\">\n",
            f->layers.size(), session->canvas.dimensions.x, session->canvas.dimensions.y);

        for (Layer*& l : f->layers) {
            ret += frmt("      <Layer Left=\"0\" Top=\"0\" Visible=\"{}\" TransColor=\"-1\" Alpha=\"{}\" AlphaOn=\"1\" Name=\"{}\" Lock=\"0\" />\n",
                l->hidden ? "0" : "1", l->layerAlpha, l->name);
        }

        ret += frmt("    </Layers>\n");
        ret += frmt("  </Frame>\n");
    }

    ret += frmt("</Frames>");
    return ret;
}

std::vector<u8> galEncodeLayerRGB(Layer* layer)
{
    std::vector<u8> ret;
    int padBytes = (4 - ((layer->w * 3) % 4)) % 4;
    ret.resize(layer->w * layer->h * 3 + (padBytes * layer->h));
    u8* retData = ret.data();
    u32* ppx = layer->pixels32();
    for (int y = 0; y < layer->h; y++) {
        for (int x = 0; x < layer->w; x++) {
            auto c = uint32ToSDLColor(ARRAY2DPOINT(ppx, x, y, layer->w));
            *(retData++) = c.b;
            *(retData++) = c.g;
            *(retData++) = c.r;
        }
        
        for (int p = 0; p < padBytes; p++) {
            *(retData++) = 0;
        }
    }
    return ret;
}

std::vector<u8> galEncodeLayerAlphaMap(Layer* layer)
{
    std::vector<u8> ret;
    int padBytes = (4 - (layer->w % 4)) % 4;
    ret.resize(layer->w * layer->h + (padBytes * layer->h));
    u8* retData = ret.data();
    u32* ppx = layer->pixels32();
    for (int y = 0; y < layer->h; y++) {
        for (int x = 0; x < layer->w; x++) {
            auto c = uint32ToSDLColor(ARRAY2DPOINT(ppx, x, y, layer->w));
            *(retData++) = c.a;
        }

        for (int p = 0; p < padBytes; p++) {
            *(retData++) = 0;
        }
    }
    return ret;
}

std::vector<u8> galEncodeBufferPair(std::vector<u8>& data)
{
    std::vector<u8> ret = compressZlib(data.data(), data.size());
    u32 size = ret.size();
    ret.insert(ret.begin(), { (u8)size, (u8)(size >> 8), (u8)(size >> 16), (u8)(size >> 24) });
    return ret;
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

bool writeGAL(PlatformNativePathString path, MainEditor* session, OperationProgressReport* progress)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        DoOnReturn closeFile([f](){fclose(f); });
        fwrite("GaleX200", 8, 1, f);
        std::string xml = galMakeXML(session);
        std::vector<u8> xmlV = std::vector<u8>(xml.begin(), xml.end());
        std::vector<u8> xmlEncode = galEncodeBufferPair(xmlV);
        fwrite(xmlEncode.data(), 1, xmlEncode.size(), f);

        for (Frame*& frame : session->frames) {
            for (Layer*& layer : frame->layers) {
                std::vector<u8> layerRGB = galEncodeLayerRGB(layer);
                std::vector<u8> layerEncode = galEncodeBufferPair(layerRGB);
                fwrite(layerEncode.data(), 1, layerEncode.size(), f);

                std::vector<u8> layerAlpha = galEncodeLayerAlphaMap(layer);
                std::vector<u8> layerAEncode = galEncodeBufferPair(layerAlpha);
                fwrite(layerAEncode.data(), 1, layerAEncode.size(), f);
            }
        }

        return true;
    }
    return false;
}
