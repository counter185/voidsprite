#include "io_base.h"
#include "io_aseprite.h"


MainEditor* readAsepriteASE(PlatformNativePathString path)
{
    struct ASERawPixelData {
        XY position;
        XY size;
        u8* pixelData;
    };
    struct ASELayerData {
        ASEPRITELayerChunkFragment0 rawFrag;
        std::string name;
    };

    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        std::map<int, ASERawPixelData> pixelDatas;
        std::map<int, ASELayerData> layerData;
        std::vector<u32> palette;
        bool foundNewPaletteChunk = false;

        ASEPRITEHeader header;
        fread(&header, sizeof(ASEPRITEHeader), 1, f);
        for (int x = 0; x < ixmin(1, header.numFrames); x++) {
            int layerChunkIndexNow = 0;
            ASEPRITEFrameHeader frameHeader;
            fread(&frameHeader, sizeof(ASEPRITEFrameHeader), 1, f);
            u32 numChunks = frameHeader.numChunksOld == 0xFFFF
                ? frameHeader.numChunksNew : frameHeader.numChunksOld;

            for (u32 nch = 0; nch < numChunks; nch++) {
                u64 posRn = ftell(f);
                u32 chunkSize;
                u16 chunkType;
                fread(&chunkSize, 4, 1, f);
                fread(&chunkType, 2, 1, f);

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
                    switch (frag0.type) {
                    case 0:
                    {
                        u16 w, h;
                        fread(&w, 2, 1, f);
                        fread(&h, 2, 1, f);
                        int dataSize = w * h * (header.colorDepth / 8);
                        u8* pixelData = (u8*)tracked_malloc(dataSize);
                        fread(pixelData, dataSize, 1, f);
                        pixelDatas[frag0.layerIndex] = { {frag0.x, frag0.y}, {w,h}, pixelData };
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
                            printf("uncompress failed: %i\n", uncompressResult);
                        }
                        pixelDatas[frag0.layerIndex] = { {frag0.x, frag0.y}, {w,h}, pixelData };
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
                    for (int x = 0; x < lastIndex - firstIndex + 1; x++) {
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
            printf("[ASEPRITE] unsupported color depth: %i\n", header.colorDepth);

        }
        else {
            if (pixelDatas.size() > 0) {
                if (header.colorDepth == 8) {
                    std::vector<LayerPalettized*> layers;
                    for (auto& kv : pixelDatas) {
                        LayerPalettized* l = new LayerPalettized(header.width, header.height);
                        l->name = "Aseprite layer";
                        if (layerData.contains(kv.first)) {
                            l->name = layerData[kv.first].name;
                            //l->layerAlpha = layerData[kv.first].rawFrag.opacity;
                            l->hidden = !(layerData[kv.first].rawFrag.flags & 1);
                        }

                        LayerPalettized* subLayer = new LayerPalettized(kv.second.size.x, kv.second.size.y);
                        u32* px = (u32*)subLayer->pixelData;
                        for (u64 pxp = 0; pxp < subLayer->w * subLayer->h; pxp++) {
                            u8 pixel = kv.second.pixelData[pxp];
                            px[pxp] = pixel;
                        }

                        l->blit(subLayer, kv.second.position);
                        delete subLayer;
                        layers.push_back(l);
                    }
                    retSn = new MainEditorPalettized(layers);
                    palette[header.paletteEntryThatIsTransparent] &= 0x00FFFFFF;
                    ((MainEditorPalettized*)retSn)->setPalette(palette);
                }
                else {
                    std::vector<Layer*> layers;
                    for (auto& kv : pixelDatas) {
                        Layer* l = new Layer(header.width, header.height);
                        l->name = "Aseprite layer";
                        if (layerData.contains(kv.first)) {
                            l->name = layerData[kv.first].name;
                            l->layerAlpha = layerData[kv.first].rawFrag.opacity;
                            l->hidden = !(layerData[kv.first].rawFrag.flags & 1);
                        }

                        Layer* subLayer = new Layer(kv.second.size.x, kv.second.size.y);
                        u32* px = (u32*)subLayer->pixelData;
                        if (header.colorDepth == 32) {
                            ASEPRITERGBAPixel* srcpx = (ASEPRITERGBAPixel*)kv.second.pixelData;
                            for (u64 pxp = 0; pxp < subLayer->w * subLayer->h; pxp++) {
                                ASEPRITERGBAPixel pixel = srcpx[pxp];
                                px[pxp] = PackRGBAtoARGB(pixel.r, pixel.g, pixel.b, pixel.a);
                            }
                        }
                        else if (header.colorDepth == 16) {
                            ASEPRITEIA88Pixel* srcpx = (ASEPRITEIA88Pixel*)kv.second.pixelData;
                            for (u64 pxp = 0; pxp < subLayer->w * subLayer->h; pxp++) {
                                ASEPRITEIA88Pixel pixel = srcpx[pxp];
                                px[pxp] = PackRGBAtoARGB(pixel.i, pixel.i, pixel.i, pixel.a);
                            }
                        }
                        l->blit(subLayer, kv.second.position);
                        delete subLayer;
                        layers.push_back(l);
                    }
                    retSn = new MainEditor(layers);
                }

                retSn->tileDimensions = { header.gridWidth, header.gridHeight };
            }
        }

        for (auto& kv : pixelDatas) {
            tracked_free(kv.second.pixelData);
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
            g_addNotification(ErrorNotification("Error", "Aseprite supports max. 256 colors"));
            fclose(f);
            return false;
        }

        u64 fileSizePosition = ftell(f);
        ASEPRITEHeader header{};
        header.fileSize = 0;
        header.magic = 0xA5E0;
        header.numFrames = 1;
        header.width = editor->canvas.dimensions.x;
        header.height = editor->canvas.dimensions.y;
        header.colorDepth = editor->isPalettized ? 8 : 32;
        header.flags = 1;
        header.speedDeprecated = 100;
        header.paletteEntryThatIsTransparent = 0;
        header.numColors = editor->isPalettized ? upcastEditor->palette.size() : 64;
        header.pixelWidth = 1;
        header.pixelHeight = 1;
        header.gridX = 0;
        header.gridY = 0;
        header.gridWidth = editor->tileDimensions.x;
        header.gridHeight = editor->tileDimensions.y;
        fwrite(&header, sizeof(ASEPRITEHeader), 1, f);

        int estimatedNumChunks = 1 + editor->layers.size() * 2 + (editor->isPalettized ? 1 : 0);
        u64 bytesInFramePosition = ftell(f);
        u32 bytesInFrame = sizeof(ASEPRITEFrameHeader);
        ASEPRITEFrameHeader frameHeader{};
        frameHeader.bytesInFrame = 0;
        frameHeader.magic = 0xF1FA;
        frameHeader.numChunksOld = estimatedNumChunks;
        frameHeader.frameDuration = 100;
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
        for (Layer*& l : editor->layers) {
            numChunks++;
            chunkHeader.size = 6 + sizeof(ASEPRITELayerChunkFragment0) + 2 + l->name.size();
            chunkHeader.type = 0x2004;
            bytesInFrame += chunkHeader.size;
            fwrite(&chunkHeader, sizeof(ASEPRITEChunkHeader), 1, f);
            ASEPRITELayerChunkFragment0 layerChunk{};
            layerChunk.flags = (l->hidden ? 0 : 1) + 0b10;
            layerChunk.layerType = 0;
            layerChunk.layerChildLevel = 0;
            layerChunk.blendMode = 0;
            layerChunk.opacity = l->layerAlpha;
            fwrite(&layerChunk, sizeof(ASEPRITELayerChunkFragment0), 1, f);
            writeASEString(l->name, f);
        }

        //cel chunks
        int i = 0;
        for (Layer*& l : editor->layers) {
            numChunks++;
            chunkHeader.type = 0x2005;

            ASEPRITECelChunkFragment0 celChunk{};
            celChunk.layerIndex = i++;
            celChunk.x = 0;
            celChunk.y = 0;
            celChunk.opacity = l->layerAlpha;
            celChunk.type = 2;
            celChunk.zIndex = 0;

            u8* newPixelData;
            u64 dataSize;
            u32* ppx = (u32*)l->pixelData;
            if (editor->isPalettized) {
                newPixelData = (u8*)tracked_malloc(l->w * l->h * 4);
                std::transform(ppx, ppx + l->w * l->h, newPixelData, [](u32 a) { return a == -1 ? 0 : (u8)a; });
                dataSize = l->w * l->h;
            }
            else {
                newPixelData = (u8*)tracked_malloc(l->w * l->h * 4);
                SDL_ConvertPixels(l->w, l->h, SDL_PIXELFORMAT_ARGB8888, l->pixelData, l->w * 4, SDL_PIXELFORMAT_ABGR8888, newPixelData, l->w * 4);
                dataSize = l->w * l->h * 4;
            }
            auto compressedData = compressZlib(newPixelData, dataSize);
            chunkHeader.size = 6 + compressedData.size() + 4 + sizeof(ASEPRITECelChunkFragment0);
            bytesInFrame += chunkHeader.size;
            fwrite(&chunkHeader, sizeof(ASEPRITEChunkHeader), 1, f);
            fwrite(&celChunk, sizeof(ASEPRITECelChunkFragment0), 1, f);
            u16 buf[2] = { l->w, l->h };
            fwrite(buf, 2, 2, f);
            fwrite(compressedData.data(), compressedData.size(), 1, f);
            tracked_free(newPixelData);
        }

        u32 fileSize = ftell(f);
        fseek(f, fileSizePosition, SEEK_SET);
        fwrite(&fileSize, 4, 1, f);
        fseek(f, bytesInFramePosition, SEEK_SET);
        fwrite(&bytesInFrame, 4, 1, f);


        fclose(f);
        return true;
    }
    return false;
}
