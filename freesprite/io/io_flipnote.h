#pragma once

#include "../globals.h"

#pragma pack(push, 1)
struct PARAHeader {
    u32 magic;
    u32 animDataSize;
    u32 audioDataSize;
    u16 numFrames;
    u16 unk1;
    u16 locked;
    u16 previewFrameNum;
    char ogAuthorName[22];
    char lastEditedBy[22];
    char userName[22];
    u64 ogAuthorID;
    u64 editAuthorID;
    char ogFileName[18];
    char fileName[18];
    u64 prevEditingAuthorID;
    u64 unk2;
    u32 date;
    u16 pad;
    u8 preview4bpp[1536];
};

struct PARAAnimSequenceHeader {
    u16 offsetTableSize;
    u16 pad;
    u32 flags;
};

struct PARAFrameData {
    u8 penPaper;
    u8 l1Encoding[48];
    u8 l2Encoding[48];
};
#pragma pack(pop)

MainEditor* readFlipnotePPM(PlatformNativePathString path);