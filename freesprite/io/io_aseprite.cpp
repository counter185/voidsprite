#include <zlib.h>

#include "io_base.h"
#include "io_aseprite.h"

#include "../FileIO.h"


std::vector<int> ASELayerEvalOrder(std::vector<ASELayer>& current, std::vector<Layer*> target, int* nextLayerID) {
    //return: vector of layer ids for each target

    std::vector<int> ret = {};
    std::vector<ASELayer> curCopy = current;
    while (!target.empty()) {
        if (!curCopy.empty()) {
            if (target[0]->name == curCopy[0].name 
                && target[0]->hidden == curCopy[0].hidden) {
                ret.push_back(curCopy[0].id);
                target.erase(target.begin());
            }
            curCopy.erase(curCopy.begin());
        }
        else {
            current.push_back({ target[0]->name, target[0]->hidden, (*nextLayerID)++ });
            ret.push_back(current.back().id);
            target.erase(target.begin());
        }
    }
    return ret;
}

void ASEWriteCellChunk(Layer* l, FILE* f, int layerID, u32* bytesWritten)
{
    ASEPRITEChunkHeader chunkHeader{};
    chunkHeader.type = 0x2005;

    ASEPRITECelChunkFragment0 celChunk{};
    celChunk.layerIndex = layerID++;
    celChunk.x = 0;
    celChunk.y = 0;
    celChunk.opacity = l->layerAlpha;
    celChunk.type = 2;
    celChunk.zIndex = 0;

    u8* newPixelData;
    u64 dataSize;
    u32* ppx = l->pixels32();
    if (l->isPalettized) {
        newPixelData = (u8*)tracked_malloc(l->w * l->h * 4);
        std::transform(ppx, ppx + l->w * l->h, newPixelData, [](u32 a) { return a == -1 ? 0 : (u8)a; });
        dataSize = l->w * l->h;
    }
    else {
        newPixelData = (u8*)tracked_malloc(l->w * l->h * 4);
        SDL_ConvertPixels(l->w, l->h, SDL_PIXELFORMAT_ARGB8888, l->pixels32(), l->w * 4, SDL_PIXELFORMAT_ABGR8888, newPixelData, l->w * 4);
        dataSize = l->w * l->h * 4;
    }
    auto compressedData = compressZlib(newPixelData, dataSize);
    chunkHeader.size = 6 + compressedData.size() + 4 + sizeof(ASEPRITECelChunkFragment0);
    *bytesWritten += chunkHeader.size;
    fwrite(&chunkHeader, sizeof(ASEPRITEChunkHeader), 1, f);
    fwrite(&celChunk, sizeof(ASEPRITECelChunkFragment0), 1, f);
    u16 buf[2] = { l->w, l->h };
    fwrite(buf, 2, 2, f);
    fwrite(compressedData.data(), compressedData.size(), 1, f);
    tracked_free(newPixelData);
}

MainEditor* readAsepriteASE(PlatformNativePathString path)
{
    struct ASERawPixelData {
        XY position;
        XY size;
        u8* pixelData;
        u8 celOpacity = 255;
    };
    struct ASELayerData {
        ASEPRITELayerChunkFragment0 rawFrag;
        std::string name;
    };

    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        std::map<int, ASELayerData> layerData;
        struct ASEFrameData {
            std::map<int, ASERawPixelData> pixelDatas;
        };

        std::vector<ASEFrameData> framesData;

        std::vector<u32> palette;
        bool foundNewPaletteChunk = false;

        int frameLengthSum = 0;

        ASEPRITEHeader header;
        fread(&header, sizeof(ASEPRITEHeader), 1, f);
        for (int x = 0; x < header.numFrames; x++) {
            loginfo(frmt("[ASE] frame {}", x));
            framesData.push_back(ASEFrameData());
            auto& fd = framesData.back();

            int layerChunkIndexNow = 0;
            ASEPRITEFrameHeader frameHeader;
            fread(&frameHeader, sizeof(ASEPRITEFrameHeader), 1, f);
            u32 numChunks = frameHeader.numChunksOld == 0xFFFF
                ? frameHeader.numChunksNew : frameHeader.numChunksOld;

            frameLengthSum += frameHeader.frameDuration;

            //iterate over chunks
            for (u32 nch = 0; nch < numChunks; nch++) {
                u64 posRn = ftell(f);
                u32 chunkSize;
                u16 chunkType;
                fread(&chunkSize, 4, 1, f);
                fread(&chunkType, 2, 1, f);

                loginfo(frmt("[ASE] chunk {:04X}", chunkType));

                switch (chunkType) {
                case 0x2004:
                {
                    ASEPRITELayerChunkFragment0 frag0;
                    fread(&frag0, sizeof(ASEPRITELayerChunkFragment0), 1, f);
                    u16 nameLen;
                    fread(&nameLen, 2, 1, f);
                    std::string name;
                    name.resize(nameLen);
                    fread(name.data(), nameLen, 1, f);
                    layerData[layerChunkIndexNow++] = { frag0, name };
                }
                break;
                case 0x2005:
                {
                    ASEPRITECelChunkFragment0 frag0;
                    fread(&frag0, sizeof(ASEPRITECelChunkFragment0), 1, f);
                    loginfo(frmt("   of type {}", frag0.type));
                    switch (frag0.type) {
                    case 0:
                    {
                        u16 w, h;
                        fread(&w, 2, 1, f);
                        fread(&h, 2, 1, f);
                        int dataSize = w * h * (header.colorDepth / 8);
                        u8* pixelData = (u8*)tracked_malloc(dataSize);
                        fread(pixelData, dataSize, 1, f);
                        fd.pixelDatas[frag0.layerIndex] = { {frag0.x, frag0.y}, {w,h}, pixelData, frag0.opacity };
                    }
                    break;
                    case 1:
                    {
                        //WHAT IS A LINKED CEL
                        u16 framePosition;
                        fread(&framePosition, 2, 1, f);
                    }
                    break;
                    case 2:
                    {
                        u16 w, h;
                        fread(&w, 2, 1, f);
                        fread(&h, 2, 1, f);
                        int zlibDataSize = chunkSize - 6 - sizeof(ASEPRITECelChunkFragment0) - 4;
                        u8* compressedData = (u8*)tracked_malloc(zlibDataSize);
                        fread(compressedData, zlibDataSize, 1, f);
                        uLongf dstLength = w * h * (header.colorDepth / 8);
                        u8* pixelData = (u8*)tracked_malloc(dstLength);
                        int uncompressResult = uncompress(pixelData, &dstLength, compressedData, zlibDataSize);
                        if (uncompressResult != Z_OK) {
                            logerr(frmt("uncompress failed: {}", uncompressResult));
                        }
                        fd.pixelDatas[frag0.layerIndex] = { {frag0.x, frag0.y}, {w,h}, pixelData, frag0.opacity };
                        tracked_free(compressedData);
                    }
                    break;
                    case 3:
                        //what is a compressed tilemap
                        ASEPRITECelChunkFragment03 frag03;
                        fread(&frag03, sizeof(ASEPRITECelChunkFragment03), 1, f);
                        //next up is zlibbed tile data
                        break;
                    }
                }
                break;
                case 0x0004:
                {
                    if (!foundNewPaletteChunk) {
                        int paletteIndexNow = 0;
                        u16 num;
                        fread(&num, 2, 1, f);
                        for (int x = 0; x < num; x++) {
                            u8 numSkip;
                            u8 numColors;
                            fread(&numSkip, 1, 1, f);
                            fread(&numColors, 1, 1, f);
                            numColors = numColors == 0 ? 256 : numColors;
                            paletteIndexNow += numSkip;
                            for (int y = 0; y < numColors; y++) {
                                u8 rgb[3];
                                fread(&rgb, 1, 3, f);
                                if (palette.size() < paletteIndexNow + 1) {
                                    palette.resize(paletteIndexNow + 1);
                                }
                                palette[paletteIndexNow++] = PackRGBAtoARGB(rgb[0], rgb[1], rgb[2], 255);
                            }
                        }
                    }
                }
                break;
                case 0x0011:
                {
                    if (!foundNewPaletteChunk) {
                        int paletteIndexNow = 0;
                        u16 num;
                        fread(&num, 2, 1, f);
                        for (int x = 0; x < num; x++) {
                            u8 numSkip;
                            u8 numColors;
                            fread(&numSkip, 1, 1, f);
                            fread(&numColors, 1, 1, f);
                            numColors = numColors == 0 ? 256 : numColors;
                            paletteIndexNow += numSkip;
                            for (int y = 0; y < numColors; y++) {
                                u8 rgb666[3];
                                fread(&rgb666, 1, 3, f);
                                if (palette.size() < paletteIndexNow + 1) {
                                    palette.resize(paletteIndexNow + 1);
                                }
                                //WHAT DO YOU MEAN 0-63
                            }
                        }
                    }
                }
                break;
                case 0x2019:
                {
                    foundNewPaletteChunk = true;
                    u32 vals[3];
                    fread(vals, 4, 3, f);
                    u8 reserved[8];
                    fread(reserved, 1, 8, f);

                    u32 newPaletteSize = vals[0];
                    u32 firstIndex = vals[1];
                    u32 lastIndex = vals[2];
                    if (palette.size() < newPaletteSize) {
                        palette.resize(newPaletteSize);
                    }
                    for (u32 x = 0; x < lastIndex - firstIndex + 1; x++) {
                        u16 flags;
                        u8 rgba[4];
                        fread(&flags, 2, 1, f);
                        fread(rgba, 1, 4, f);
                        palette[firstIndex + x] = PackRGBAtoARGB(rgba[0], rgba[1], rgba[2], rgba[3]);

                        if (flags & 1) {
                            u16 nameLen;
                            fread(&nameLen, 2, 1, f);
                            std::string name;
                            name.resize(nameLen);
                            fread(name.data(), nameLen, 1, f);
                        }
                    }
                }
                break;
                }

                fseek(f, posRn, SEEK_SET);
                fseek(f, chunkSize, SEEK_CUR);
            }
        }

        MainEditor* retSn = NULL;
        if (header.colorDepth != 32 && header.colorDepth != 16 && header.colorDepth != 8) {
            //we are not there yet
            logprintf("[ASEPRITE] unsupported color depth: %i\n", header.colorDepth);

        }
        else {
            if (framesData.size() > 0) {
                std::vector<Frame*> frames;
                if (header.colorDepth == 8) {
                    for (auto& aseFrame : framesData) {
                        std::map<int, LayerPalettized*> layerMap;
                        std::vector<LayerPalettized*> layers;
                        for (auto& [id, ld] : layerData) {
                            LayerPalettized* l = new LayerPalettized(header.width, header.height);
                            l->name = ld.name;
                            //l->layerAlpha = ld.rawFrag.opacity;
                            l->hidden = !(ld.rawFrag.flags & 1);
                            layerMap[id] = l;
                            layers.push_back(l);
                        }

                        for (auto& [id, rawPx] : aseFrame.pixelDatas) {
                            LayerPalettized* subLayer = new LayerPalettized(rawPx.size.x, rawPx.size.y);
                            u32* px = subLayer->pixels32();
                            for (u64 pxp = 0; pxp < subLayer->w * subLayer->h; pxp++) {
                                u8 pixel = rawPx.pixelData[pxp];
                                px[pxp] = pixel;
                            }

                            layerMap[id]->blit(subLayer, rawPx.position);
                            delete subLayer;
                        }
                        Frame* f = new Frame();
                        f->layers = std::vector<Layer*>(layers.begin(), layers.end());
                        frames.push_back(f);
                    }
                    retSn = new MainEditorPalettized(frames);
                    palette[header.paletteEntryThatIsTransparent] &= 0x00FFFFFF;
                    ((MainEditorPalettized*)retSn)->setPalette(palette);
                }
                else {
                    for (auto& aseFrame : framesData) {
                        std::map<int, Layer*> layerMap;
                        std::vector<Layer*> layers;
                        for (auto& [id, ld] : layerData) {
                            Layer* l = new Layer(header.width, header.height);
                            l->name = ld.name;
                            l->layerAlpha = 255;
                            l->hidden = !(ld.rawFrag.flags & 1);
                            layerMap[id] = l;
                            layers.push_back(l);
                        }

                        for (auto& [id, rawPx] : aseFrame.pixelDatas) {
                            Layer* subLayer = new Layer(rawPx.size.x, rawPx.size.y);
                            u32* px = subLayer->pixels32();
                            if (header.colorDepth == 32) {
                                ASEPRITERGBAPixel* srcpx = (ASEPRITERGBAPixel*)rawPx.pixelData;
                                for (u64 pxp = 0; pxp < subLayer->w * subLayer->h; pxp++) {
                                    ASEPRITERGBAPixel pixel = srcpx[pxp];
                                    px[pxp] = PackRGBAtoARGB(pixel.r, pixel.g, pixel.b, pixel.a);
                                }
                            }
                            else if (header.colorDepth == 16) {
                                ASEPRITEIA88Pixel* srcpx = (ASEPRITEIA88Pixel*)rawPx.pixelData;
                                for (u64 pxp = 0; pxp < subLayer->w * subLayer->h; pxp++) {
                                    ASEPRITEIA88Pixel pixel = srcpx[pxp];
                                    px[pxp] = PackRGBAtoARGB(pixel.i, pixel.i, pixel.i, pixel.a);
                                }
                            }
                            layerMap[id]->blit(subLayer, rawPx.position);
                            layerMap[id]->layerAlpha = (u8)((layerData[id].rawFrag.opacity / 255.0f) * rawPx.celOpacity);
                            delete subLayer;
                        }
                        Frame* f = new Frame();
                        f->layers = layers;
                        frames.push_back(f);
                    }
                    retSn = new MainEditor(frames);
                }

                retSn->tileDimensions = { header.gridWidth, header.gridHeight };
                retSn->setMSPerFrame(frameLengthSum / frames.size());
            }
        }

        for (auto& frame : framesData) {
            for (auto& kv : frame.pixelDatas) {
                tracked_free(kv.second.pixelData);
            }
        }

        fclose(f);
        return retSn;
    }
    return NULL;
}

bool writeAsepriteASE(PlatformNativePathString path, MainEditor* editor)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {

        MainEditorPalettized* upcastEditor = editor->isPalettized ? (MainEditorPalettized*)editor : NULL;
        if (editor->isPalettized && upcastEditor->palette.size() > 256) {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Aseprite supports max. 256 colors"));
            fclose(f);
            return false;
        }

        std::vector<std::vector<int>> frameLayerIndices;
        std::vector<ASELayer> aseLayers;
        int nextLayerID = 0;
        for (Frame* f : editor->frames) {
            frameLayerIndices.push_back(ASELayerEvalOrder(aseLayers, f->layers, &nextLayerID));
        }


        u64 fileSizePosition = ftell(f);
        ASEPRITEHeader header{};
        header.fileSize = 0;
        header.magic = 0xA5E0;
        header.numFrames = editor->frames.size();
        header.width = editor->canvas.dimensions.x;
        header.height = editor->canvas.dimensions.y;
        header.colorDepth = editor->isPalettized ? 8 : 32;
        header.flags = 1;
        header.speedDeprecated = editor->frameAnimMSPerFrame;
        header.paletteEntryThatIsTransparent = 0;
        header.numColors = editor->isPalettized ? upcastEditor->palette.size() : 64;
        header.pixelWidth = 1;
        header.pixelHeight = 1;
        header.gridX = 0;
        header.gridY = 0;
        header.gridWidth = editor->tileDimensions.x;
        header.gridHeight = editor->tileDimensions.y;
        fwrite(&header, sizeof(ASEPRITEHeader), 1, f);

        int estimatedNumChunks = 1 + nextLayerID + editor->frames[0]->layers.size() + (editor->isPalettized ? 1 : 0);
        u64 bytesInFramePosition = ftell(f);
        u32 bytesInFrame = sizeof(ASEPRITEFrameHeader);
        ASEPRITEFrameHeader frameHeader{};
        frameHeader.bytesInFrame = 0;
        frameHeader.magic = 0xF1FA;
        frameHeader.numChunksOld = estimatedNumChunks;
        frameHeader.frameDuration = editor->frameAnimMSPerFrame;
        frameHeader.numChunksNew = estimatedNumChunks;
        fwrite(&frameHeader, sizeof(ASEPRITEFrameHeader), 1, f);

        int numChunks = 0;
        ASEPRITEChunkHeader chunkHeader{};

        //color profile chunk
        numChunks++;
        chunkHeader.size = 6 + sizeof(ASEPRITEColorProfileChunk);
        chunkHeader.type = 0x2007;
        bytesInFrame += chunkHeader.size;
        fwrite(&chunkHeader, sizeof(ASEPRITEChunkHeader), 1, f);
        ASEPRITEColorProfileChunk colorProfileChunk{};
        colorProfileChunk.type = 1;
        colorProfileChunk.flags = 0;
        fwrite(&colorProfileChunk, sizeof(ASEPRITEColorProfileChunk), 1, f);

        //palette chunk
        if (editor->isPalettized) {
            chunkHeader.size = 6 + 4 * 3 + 8 + upcastEditor->palette.size() * (2 + 4);
            chunkHeader.type = 0x2019;
            bytesInFrame += chunkHeader.size;
            fwrite(&chunkHeader, sizeof(ASEPRITEChunkHeader), 1, f);
            u32 buffer[3] = {upcastEditor->palette.size(), 0, upcastEditor->palette.size() - 1 };
            fwrite(buffer, 4, 3, f);
            u8 reservedBytes[8] = { 0 };
            fwrite(reservedBytes, 1, 8, f);
            for (u32& c : upcastEditor->palette) {
                u16 buffer2 = 0;
                fwrite(&buffer2, 2, 1, f);
                SDL_Color col = uint32ToSDLColor(c);
                u8 buffer3[4] = { col.r, col.g, col.b, col.a };
                fwrite(buffer3, 1, 4, f);
            }
        }

        //layer chunks
        for (ASELayer& l : aseLayers) {
            numChunks++;
            chunkHeader.size = 6 + sizeof(ASEPRITELayerChunkFragment0) + 2 + l.name.size();
            chunkHeader.type = 0x2004;
            bytesInFrame += chunkHeader.size;
            fwrite(&chunkHeader, sizeof(ASEPRITEChunkHeader), 1, f);
            ASEPRITELayerChunkFragment0 layerChunk{};
            layerChunk.flags = (l.hidden ? 0 : 1) + 0b10;
            layerChunk.layerType = 0;
            layerChunk.layerChildLevel = 0;
            layerChunk.blendMode = 0;
            layerChunk.opacity = 255;
            fwrite(&layerChunk, sizeof(ASEPRITELayerChunkFragment0), 1, f);
            writeASEString(l.name, f);
        }

        //cel chunks
        for (int fi = 0; fi < editor->frames.size(); fi++) {
            Frame* fr = editor->frames[fi];
            std::vector<int>& layerIndices = frameLayerIndices[fi];

            if (fi > 0) {
                int frameNumChunks = fr->layers.size();
                //new frame header
                bytesInFramePosition = ftell(f);
                bytesInFrame = sizeof(ASEPRITEFrameHeader);
                ASEPRITEFrameHeader frameHeader{};
                frameHeader.bytesInFrame = 0;
                frameHeader.magic = 0xF1FA;
                frameHeader.numChunksOld = frameNumChunks;
                frameHeader.frameDuration = editor->frameAnimMSPerFrame;
                frameHeader.numChunksNew = frameNumChunks;
                fwrite(&frameHeader, sizeof(ASEPRITEFrameHeader), 1, f);
                numChunks = 0;
            }

            int i = 0;
            for (Layer*& l : fr->layers) {
                numChunks++;
                ASEWriteCellChunk(l, f, layerIndices[i++], &bytesInFrame);
            }
            fseek(f, bytesInFramePosition, SEEK_SET);
            fwrite(&bytesInFrame, 4, 1, f);
            fseek(f, 0, SEEK_END);
        }

        u32 fileSize = ftell(f);
        fseek(f, fileSizePosition, SEEK_SET);
        fwrite(&fileSize, 4, 1, f);


        fclose(f);
        return true;
    }
    return false;
}
