#include <map>
#include "io_base.h"
#include "io_gif.h"

std::vector<u8> GIFReadDataBlocks(FILE* f)
{
    std::vector<u8> ret;
    //read subblocks
    while (true) {
        u8 subBlockSize = 0;
        fread(&subBlockSize, 1, 1, f);
        if (subBlockSize == 0) {
            fseek(f, -1, SEEK_CUR);
            break;
        }
        u64 dataSizeNow = ret.size();
        ret.resize(dataSizeNow + subBlockSize);
        u8* targetDataPtr = ret.data() + dataSizeNow;
        fread(targetDataPtr, 1, subBlockSize, f);
    };
    return ret;
}
std::vector<u8> GIFDecodeLZW(u8 minCodeSize, std::vector<u8>& target) {
    BitReader br(target.data(), target.size());
    int codeSize = minCodeSize + 1;
    //loginfo(frmt("[GIF] decode with min code size {}", (int)minCodeSize));
    std::map<int, std::vector<u8>> dictionary;
    int dictSize = 1 << minCodeSize;
    for (int i = 0; i < dictSize; i++) {
        dictionary[i] = {(u8)i};
    }
    int clearCode = dictSize++;
    int endCode = dictSize++;
    std::vector<u8> output = {};

    int prevCode = -1;

    while (true) {
        u32 code = br.readBits(codeSize);

        if (code == clearCode) {
            //loginfo("--clear code");
            prevCode = -1;
            codeSize = minCodeSize + 1;
            for (int i = 0; i < clearCode; i++) {
                dictionary[i] = { (u8)i };
            }
            dictSize = clearCode + 2;
        }
        else if (code == endCode) {
            break;
        }
        else {
            std::vector<u8> entry;

            if (code < dictSize) {
                entry = dictionary[code];
            }
            else {
                entry = dictionary[prevCode];
                if (!entry.empty()) {
                    entry.push_back(entry.front());
                } else {
                    logerr(frmt("[GIF] LZW decode error: invalid code {} at {}", code, output.size()));
                    break;
                }
            }

            output.insert(output.end(), entry.begin(), entry.end());

            if (prevCode != -1 && dictSize < 4096) {
                auto newEntry = dictionary[prevCode];
                newEntry.push_back(entry.front());
                dictionary[dictSize++] = newEntry;

                if (dictSize == (1 << codeSize) && codeSize < 12) {
                    //loginfo(frmt("--increasing code size to {}", (int)(codeSize + 1)));
                    codeSize++;
                }
            }
            prevCode = code;
        }
    }
    return output;
}

BitWriter GIFEncodeLZW(int lzwMinCodeSize, LayerPalettized* frame) {
    BitWriter bw = BitWriter();
    u8 currentCodeSize = lzwMinCodeSize + 1;
    //loginfo(frmt("code size: {}", (int)lzwMinCodeSize));
    int dictSize = 1 << lzwMinCodeSize;
    std::vector<std::vector<u8>> codeTable;
    for (int i = 0; i < dictSize; i++) {
        codeTable.push_back({ (u8)i });
    }
    int clearCode = dictSize++;
    int endCode = dictSize++;
    codeTable.push_back({}); //clear code
    codeTable.push_back({}); //end code

    u32* pixels = frame->pixels32();
    u64 pixelCount = frame->w * frame->h;
    u64 ptr = 0;
    std::vector<u8> indexBuffer = {};

    bw.writeBits(clearCode, currentCodeSize);
    indexBuffer.push_back((u8)(pixels[ptr++] & 0xFF));
    while (ptr < pixelCount) {
        u8 k = (u8)(pixels[ptr++] & 0xFF);

        auto testBuffer = indexBuffer;
        testBuffer.push_back(k);

        auto pos = std::find_if(codeTable.begin(), codeTable.end(), [&](const std::vector<u8>& v) {
            return v == testBuffer;
        });
        if (pos != codeTable.end()) {
            //found
            indexBuffer.push_back(k);
        } else {
            //not found
            auto existingPos = std::find_if(codeTable.begin(), codeTable.end(), [&](const std::vector<u8>& v) {
                return v == indexBuffer;
            });
            int codeIndex = std::distance(codeTable.begin(), existingPos);
            bw.writeBits(codeIndex, currentCodeSize);
            indexBuffer = {k};
            if (dictSize < 4096) {
                codeTable.push_back(testBuffer);
                dictSize++;
                if (currentCodeSize < 12 && dictSize == ((1 << currentCodeSize) + 1)) {
                    currentCodeSize++;
                    //loginfo(frmt("---increasing code size to {}", (int)currentCodeSize));
                }
            } else {
                //loginfo("---dictionary full, clear code");
                bw.writeBits(clearCode, currentCodeSize);
                codeTable.clear();
                dictSize = 1 << lzwMinCodeSize;
                for (int i = 0; i < dictSize; i++) {
                    codeTable.push_back({ (u8)i });
                }
                clearCode = dictSize++;
                endCode = dictSize++;
                codeTable.push_back({}); //clear code
                codeTable.push_back({}); //end code
                currentCodeSize = lzwMinCodeSize + 1;
            }
        }
    }
    if (!indexBuffer.empty()) {
        auto existingPos = std::find_if(codeTable.begin(), codeTable.end(), [&](const std::vector<u8>& v) {
            return v == indexBuffer;
        });
        int codeIndex = std::distance(codeTable.begin(), existingPos);
        bw.writeBits(codeIndex, currentCodeSize);
    }
    bw.writeBits(endCode, currentCodeSize);
    bw.flush();
    return bw;
}

MainEditor* readGIF(PlatformNativePathString path)
{
    u8 IMAGE_SEPERATOR_MAGIC = 0x2C;
    u8 EXTENSION_INTRODUCER_MAGIC = 0x21;
    u8 GIF_TRAILER_MAGIC = 0x3B;

    const u8 LABEL_GC = 0xF9;
    const u8 LABEL_COMMENT = 0xFE;
    const u8 LABEL_APPLICATION = 0xFF;
    const u8 LABEL_PLAINTEXT = 0x01;

    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        u8 magic[3];
        u8 version[3];
        fread(magic, 1, 3, f);
        fread(version, 1, 3, f);

        bool gctColorTableSet = false;
        std::vector<u32> gctColorTable;
        u8 gctBackgroundIndex = 0;
        std::vector<u32> currentLctColorTable;

        //magic should == "GIF"
        GIFLogicalScreenDescriptor lsd;
        fread(&lsd, sizeof(GIFLogicalScreenDescriptor), 1, f);
        if (lsd.gctFlags.enabled) {
            gctColorTableSet = true;
            int nColors = pow(2, lsd.gctFlags.size + 1);
            for (int x = 0; x < nColors; x++) {
                u8 color[3];
                fread(color, 1, 3, f);
                gctColorTable.push_back(PackRGBAtoARGB(color[0], color[1], color[2], 255));
            }
        }

        std::vector<GIFGraphicControlExtension> gces;
        std::vector<GIFApplicationExtension> gaes;
        std::vector<GIFPlainTextExtension> gptes;
        std::vector<GIFImageDescriptor> gids;

        std::vector<LayerPalettized*> allFrameLayers;

        while (!feof(f)) {
            u8 blockIdentifier;
            fread(&blockIdentifier, 1, 1, f);
            loginfo(frmt("[GIF] Block {:02X} at {:08X}", blockIdentifier, ftell(f) - 1));
            if (blockIdentifier == IMAGE_SEPERATOR_MAGIC) {
                GIFImageDescriptor imgDesc;
                fread(&imgDesc, sizeof(GIFImageDescriptor), 1, f);
                gids.push_back(imgDesc);
                currentLctColorTable.clear();
                if (imgDesc.flags.lctEnable) {
                    int nColors = pow(2, imgDesc.flags.lctSize + 1);
                    for (int x = 0; x < nColors; x++) {
                        u8 color[3];
                        fread(color, 1, 3, f);
                        currentLctColorTable.push_back(PackRGBAtoARGB(color[0], color[1], color[2], 255));
                    }
                }
                u8 lzwMinCode;
                fread(&lzwMinCode, 1, 1, f);

                std::vector<u8> imageData = GIFReadDataBlocks(f);
                LayerPalettized* l = LayerPalettized::tryAllocIndexedLayer(imgDesc.imageWidth, imgDesc.imageHeight);
                l->name = "GIF Layer";
                allFrameLayers.push_back(l);
                std::vector<u8> decodedData = GIFDecodeLZW(lzwMinCode, imageData);
                u32* pxd = l->pixels32();
                l->palette = imgDesc.flags.lctEnable ? currentLctColorTable : gctColorTable;
                for (u64 ii = 0; ii < ixmin(decodedData.size(), l->w*l->h); ii++) {
                    pxd[ii] = decodedData[ii];
                }

            }
            else if (blockIdentifier == EXTENSION_INTRODUCER_MAGIC) {
                u8 label;
                fread(&label, 1, 1, f);
                switch (label) {
                    case LABEL_GC:
                        GIFGraphicControlExtension gce;
                        fread(&gce, sizeof(GIFGraphicControlExtension), 1, f);
                        gces.push_back(gce);
                        break;
                    case LABEL_COMMENT:
                    {
                        std::vector<u8> commentData = GIFReadDataBlocks(f);
                        std::string commentStr(commentData.begin(), commentData.end());
                        loginfo(frmt("[GIF] Comment: {}", commentStr));
                    }
                    break;
                    case LABEL_APPLICATION:
                    {
                        GIFApplicationExtension gae;
                        fread(&gae, sizeof(GIFApplicationExtension), 1, f);
                        std::vector<u8> applicationData = GIFReadDataBlocks(f);
                        gaes.push_back(gae);
                    }
                    break;
                    case LABEL_PLAINTEXT:
                    {
                        GIFPlainTextExtension gpe;
                        fread(&gpe, sizeof(GIFPlainTextExtension), 1, f);
                        std::vector<u8> ptd = GIFReadDataBlocks(f);
                        gptes.push_back(gpe);
                    }
                    break;
                    default:
                        logerr(frmt("[GIF] UNKNOWN EXTENSION: %i\n", label));
                        break;
                }
            }
            else if (blockIdentifier == GIF_TRAILER_MAGIC) {
                break;
            }
            u8 blockTerminator;
            fread(&blockTerminator, 1, 1, f);
        }

        int frameDelay = 200;
        if (!gces.empty()) {
            int sumFrameDelay = 0;
            for (GIFGraphicControlExtension& gce : gces) {
                sumFrameDelay += gce.delay;
            }
            frameDelay = sumFrameDelay / gces.size() * 10;
        }

        std::vector<Frame*> frames;

        if (!allFrameLayers.empty()) {
            bool convertAllToRGB = false;
            std::vector<u32> firstPalette = allFrameLayers.front()->palette;
            for (LayerPalettized* l : allFrameLayers) {
                if (l->palette != firstPalette) {
                    convertAllToRGB = true;
                    break;
                }
            }

            int i = 0;
            Layer* prevImage = NULL;
            for (LayerPalettized* l : allFrameLayers) {
                if (gces.size() > i) {
                    Frame* fr = new Frame();
                    GIFGraphicControlExtension correspondingGCE = gces[i];
                    GIFImageDescriptor correspondingGID = gids[i];

                    if (correspondingGCE.flags.transparent) {
                        u8 replaceIndex = correspondingGCE.transparentColorIndex;
                        l->replaceColor(replaceIndex, -1);
                    }
                    if (prevImage == NULL) {
                        prevImage = convertAllToRGB ? l->toRGB() : l->copyCurrentVariant();
                    }
                    else {
                        if (correspondingGCE.flags.disposalMode == 2) {
                            prevImage->fillRect({ 0,0 }, { prevImage->w, prevImage->h },
                                correspondingGCE.transparentColorIndex == gctBackgroundIndex ? (prevImage->isPalettized ? -1 : 0) 
                                : ((convertAllToRGB && l->palette.size() > gctBackgroundIndex) ? l->palette[gctBackgroundIndex] : gctBackgroundIndex));
                        }
                        Layer* newPrev = convertAllToRGB ? l->toRGB() : l->copyCurrentVariant();
                        prevImage->blit(newPrev, 
                            { correspondingGID.imageLeftPosition, correspondingGID.imageTopPosition },
                            {0,0, newPrev->w, newPrev->h});
                        delete newPrev;
                        //3 wants to revert to previous state?
                    }

                    fr->layers.push_back(prevImage->copyCurrentVariant());
                    delete l;
                    frames.push_back(fr);
                }
                else {
                    logerr(frmt("[GIF] no corresponding GCE found for frame {}", i));
                }
                i++;
            }
            if (prevImage != NULL) {
                delete prevImage;
            }
        }

        MainEditor* ret = NULL;
        if (!frames.empty()) {
            ret = frames.front()->layers.front()->isPalettized ? new MainEditorPalettized(frames) : new MainEditor(frames);
            ret->setMSPerFrame(frameDelay);
        }

        fclose(f);
        return ret;
    }
    return NULL;
}


bool writeGIF(PlatformNativePathString path, MainEditor* editor) {
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {

        std::vector<LayerPalettized*> frames;
        for (Frame* fr : editor->frames) {
            Layer* flat = editor->isPalettized ? ((MainEditorPalettized*)editor)->flattenFrameWithoutConvertingToRGB(fr) : editor->flattenFrame(fr);
            if (!editor->isPalettized) {
                //Layer* ff = flat->    //todo convert to indexed
            }
            if (flat == NULL) {
                for (LayerPalettized* l : frames) {
                    delete l;
                }
                fclose(f);
                return false;
            }
            frames.push_back((LayerPalettized*)flat);
        }

        fwrite("GIF89a", 1, 6, f);
        GIFLogicalScreenDescriptor lsd;
        lsd.width = editor->canvas.dimensions.x;
        lsd.height = editor->canvas.dimensions.y;
        lsd.gctFlags.enabled = 1;
        lsd.gctFlags.colorRes = 6;
        lsd.gctFlags.sort = 0;
        lsd.gctFlags.size = 7;
        lsd.bgColorIndex = 254;
        lsd.pixelAspect = 0;
        fwrite(&lsd, sizeof(GIFLogicalScreenDescriptor), 1, f);
        auto& palette = frames.front()->palette;
        for (int i = 0; i < 256; i++) {
            u32 color = palette.size() > i ? palette[i] : 0;
            auto cc = uint32ToSDLColor(color);
            fwrite(&cc.r, 1, 1, f);
            fwrite(&cc.g, 1, 1, f);
            fwrite(&cc.b, 1, 1, f);
        }

        GIFApplicationExtension pae;
        pae.blockSize = 11;
        memcpy(pae.identifier, "NETSCAPE", 8);
        memcpy(pae.authenticationCode, "2.0", 3);
        fwrite("\x21\xFF", 1, 2, f); //extension introducer
        fwrite(&pae, sizeof(GIFApplicationExtension), 1, f);
        u8 subBlockSize = 3;
        fwrite(&subBlockSize, 1, 1, f);
        u8 appData[3] = { 1, 0, 0 };
        fwrite(appData, 1, 3, f);
        fwrite("\x00", 1, 1, f); //block terminator

        fwrite("\x21\xFE", 1, 2, f); //comment extension
        std::string comment = "exported by voidsprite";
        u8 commentBlockSize = (u8)std::min<size_t>(255, comment.size());
        fwrite(&commentBlockSize, 1, 1, f);
        fwrite(comment.c_str(), 1, commentBlockSize, f);
        fwrite("\x00", 1, 1, f); //block terminator

        for (LayerPalettized* frame : frames) {

            GIFGraphicControlExtension gce;
            fwrite("\x21\xF9", 1, 2, f); //extension introducer
            gce.blockSize = 4;
            gce.flags.disposalMode = 2; //clear to bg
            gce.flags.userInput = 0;
            gce.flags.transparent = 1;
            gce.delay = editor->frameAnimMSPerFrame / 10;
            gce.transparentColorIndex = 255;
            fwrite(&gce, sizeof(GIFGraphicControlExtension), 1, f);
            fwrite("\x00", 1, 1, f); //block terminator

            GIFImageDescriptor gid;
            gid.imageLeftPosition = 0;
            gid.imageTopPosition = 0;
            gid.imageWidth = editor->canvas.dimensions.x;
            gid.imageHeight = editor->canvas.dimensions.y;
            gid.flags.interlaceFlag = 0;
            gid.flags.lctEnable = 0;
            gid.flags.lctSort = 0;
            gid.flags.lctSize = 0;
            fwrite("\x2C", 1, 1, f); //image separator
            fwrite(&gid, sizeof(GIFImageDescriptor), 1, f);
            u8 lzwMinCodeSize = 8;
            fwrite(&lzwMinCodeSize, 1, 1, f);
            BitWriter bw = GIFEncodeLZW(lzwMinCodeSize, frame);
            std::vector<u8>& encodedData = bw.getData();
            u8* dataPtr = encodedData.data();
            u64 dataLeft = encodedData.size();
            while (dataLeft > 0) {
                u8 blockSize = (dataLeft > 255) ? 255 : (u8)dataLeft;
                fwrite(&blockSize, 1, 1, f);
                fwrite(dataPtr, 1, blockSize, f);
                dataPtr += blockSize;
                dataLeft -= blockSize;
            }
            fwrite("\x00", 1, 1, f); //block terminator

        }
        //write trailer
        fwrite(";", 1, 1, f);

        for (LayerPalettized* l : frames) {
            delete l;
        }
        fclose(f);
        return true;
    }
    return false;
}