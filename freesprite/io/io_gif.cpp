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
    BitReader br(target.data());
    int codeSize = minCodeSize + 1;
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
                entry.push_back(entry.front());
            }

            output.insert(output.end(), entry.begin(), entry.end());

            if (prevCode != -1 && dictSize < 4096) {
                auto newEntry = dictionary[prevCode];
                newEntry.push_back(entry.front());
                dictionary[dictSize++] = newEntry;

                if (dictSize == (1 << codeSize) && codeSize < 12) {
                    codeSize++;
                }
            }
            prevCode = code;
        }
    }
    return output;
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
