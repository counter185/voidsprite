#include "io_base.h"
#include "io_flipnote.h"

//referenced this:
//https://github.com/Flipnote-Collective/ppm-parser/blob/3e3d29a32a9fab631f650e0f34907dfceb4bda16/ppm.py#L171
u32 PARAdecodeLine2(u8* outB, u8* inB, u32 useByte)
{
    int pixel = 0;
    int offset = 0;

    if (useByte == -1) {
        while (pixel < 256) {
            u8 chunk = inB[offset++];
            for (int bit = 0; bit < 8; bit++) {
                outB[pixel++] = (chunk >> bit) & 1;
            }
        }
    }
    else {
        while (pixel < 256) {
            if (useByte & 0x80000000) {
                u8 chunk = inB[offset++];
                for (int bit = 0; bit < 8; bit++) {
                    outB[pixel++] = (chunk >> bit) & 1;
                }
            }
            else {
                pixel += 8;
            }
            useByte <<= 1;
        }
    }
    return offset;
}

MainEditor* readFlipnotePPM(PlatformNativePathString path)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        PARAHeader header;
        fread(&header, sizeof(PARAHeader), 1, f);
        header.numFrames++;
        loginfo(frmt("[Flipnote] {} frames", header.numFrames));

        PARAAnimSequenceHeader animSeqHeader;
        fread(&animSeqHeader, sizeof(PARAAnimSequenceHeader), 1, f);
        std::vector<u32> frameOffsets;
        for (int i = 0; i < header.numFrames; i++) {
            u32 frameOffset;
            fread(&frameOffset, 4, 1, f);
            frameOffsets.push_back(frameOffset);
        }
        u64 endOfOffsetList = ftell(f);
        loginfo(frmt("[Flipnote] anim list ends at {:X}", endOfOffsetList));

        std::vector<Frame*> frames;
        std::vector<u32> palette = {
            0xFF000000, //black
            0xFFFFFFFF, //white
            0xFFFF0000, //red pen
            0xFF0000FF  //blue pen
        };

        u8* prevLayer1 = NULL, * prevLayer2 = NULL;

        int frameNum = 1;
        for (auto& frameOffset : frameOffsets) {
            fseek(f, endOfOffsetList + frameOffset, SEEK_SET);
            PARAFrameData frameData;
            fread(&frameData.penPaper, 1, 1, f);

            bool isPaperBlack = (frameData.penPaper & 0b1) == 0;
            u8 layer1Color = (frameData.penPaper >> 1) & 0b11;
            u8 layer2Color = (frameData.penPaper >> 3) & 0b11;
            u8 isTranslated = (frameData.penPaper >> 5) & 0b11;
            u8 tX = 0, tY = 0;
            bool newFrame = (frameData.penPaper & 0b10000000) != 0 || prevLayer1 == NULL;

            if (!newFrame) {
                loginfo(" copied frame");
            }

            if (isTranslated) {
                loginfo(" translated frame");
                fread(&tX, 1, 1, f);
                fread(&tY, 1, 1, f);
            }

            fread(&frameData.l1Encoding, 48, 1, f);
            fread(&frameData.l2Encoding, 48, 1, f);

            loginfo(frmt(" frame {}-------------", frameNum++));
            loginfo(frmt("[Flipnote] {:08B}", frameData.penPaper));
            u8* layer1Data = (u8*)tracked_malloc(256 * 192);
            u8* layer2Data = (u8*)tracked_malloc(256 * 192);
            memset(layer1Data, 0, 256 * 192);
            memset(layer2Data, 0, 256 * 192);
            u8 decodeBuffer[32];

            struct PARADecodeTarget {
                u8* data;
                u8* lineEncodingInfo;
            };

            for (auto& decodeTarget : std::vector<PARADecodeTarget>{
                {layer1Data, frameData.l1Encoding},
                {layer2Data, frameData.l2Encoding}
            }) 
            {
                for (int y = 0; y < 192; y++) {
                    int lineByteIndex = y / 4;
                    int lineBitShift = (y % 4) * 2;
                    u8 lEncodingInfo = (decodeTarget.lineEncodingInfo[lineByteIndex] >> lineBitShift) & 0b11;

                    loginfo(frmt("Line {} encoding: {}", y, lEncodingInfo));

                    if (lEncodingInfo != 0) {
                        u32 chunkUsage;
                        if (lEncodingInfo == 3) {
                            chunkUsage = -1;
                        }
                        else {
                            fread(&chunkUsage, 4, 1, f);
                            chunkUsage = BEtoLE32(chunkUsage);
                        }
                        if (lEncodingInfo == 2) {
                            memset(decodeTarget.data + 256 * y, 1, 256);
                        }

                        u64 comeBack = ftell(f);
                        fread(decodeBuffer, 32, 1, f);
                        u32 offset = PARAdecodeLine2(decodeTarget.data + 256 * y, decodeBuffer, chunkUsage);
                        fseek(f, comeBack + offset, SEEK_SET);
                    }
                }
            }

            if (!newFrame) {
                for (int y = 0; y < 192; y++) {
                    int tty = y - tY;
                    if (tty < 0) {
                        continue;
                    }
                    else if (tty >= 192) {
                        break;
                    }
                    for (int x = 0; x < 256; x++) {
                        int ttx = x - tX;
                        if (ttx < 0) {
                            continue;
                        }
                        else if (ttx >= 256) {
                            break;
                        }
                        ARRAY2DPOINT(layer1Data, x, y, 256) ^= ARRAY2DPOINT(prevLayer1, ttx, tty, 256);
                        ARRAY2DPOINT(layer2Data, x, y, 256) ^= ARRAY2DPOINT(prevLayer2, ttx, tty, 256);
                    }
                }
            }

            LayerPalettized* l1 = LayerPalettized::tryAllocIndexedLayer(256, 192);
            LayerPalettized* l2 = LayerPalettized::tryAllocIndexedLayer(256, 192);
            l1->name = "Flipnote Layer 1";
            l2->name = "Flipnote Layer 2";
            l1->palette = l2->palette = palette;
            s32* l1px = (s32*)l1->pixels32();
            s32* l2px = (s32*)l2->pixels32();
            u8 l1PenIndex = layer1Color <= 1 ? isPaperBlack ? 1 : 0 : layer1Color;
            u8 l2PenIndex = layer2Color <= 1 ? isPaperBlack ? 1 : 0 : layer2Color;
            memset(l2px, -1, 256 * 192 * 4);
            std::fill(l1px, l1px + (256 * 192), isPaperBlack ? 0 : 1);
            for (u64 i = 0; i < 256 * 192; i++) {
                if (layer1Data[i] != 0) {
                    l1px[i] = l1PenIndex;
                };
                if (layer2Data[i] != 0) {
                    l2px[i] = l2PenIndex;
                };
            }

            Frame* nFrame = new Frame();
            nFrame->layers.push_back(l1);
            nFrame->layers.push_back(l2);
            frames.push_back(nFrame);

            if (prevLayer1 != NULL) {
                tracked_free(prevLayer1);
                tracked_free(prevLayer2);
            }
            prevLayer1 = layer1Data;
            prevLayer2 = layer2Data;
        }

        tracked_free(prevLayer1);
        tracked_free(prevLayer2);

        MainEditorPalettized* ret = NULL;

        if (!frames.empty()) {
            ret = new MainEditorPalettized(frames);
        }

        fclose(f);
        return ret;
    }

    return NULL;
}
