#pragma once

#include "../globals.h"

#pragma pack(push, 1)
struct __AVIMAINHEADER {
    u32  dwMicroSecPerFrame;
    u32  dwMaxBytesPerSec;
    u32  dwPaddingGranularity;
    u32  dwFlags;
    u32  dwTotalFrames;
    u32  dwInitialFrames;
    u32  dwStreams;
    u32  dwSuggestedBufferSize;
    u32  dwWidth;
    u32  dwHeight;
    u32  dwReserved[4];
};

struct __BITMAPINFOHEADER {
    u32 biSize;
    int  biWidth;
    int  biHeight;
    u16  biPlanes;
    u16  biBitCount;
    u32 biCompression;
    u32 biSizeImage;
    int  biXPelsPerMeter;
    int  biYPelsPerMeter;
    u32 biClrUsed;
    u32 biClrImportant;
};

struct __AVIStreamHeader {
    u32 fccType;
    u32 fccHandler;
    u32  dwFlags;
    u16  wPriority;
    u16  wLanguage;
    u32  dwInitialFrames;
    u32  dwScale;
    u32  dwRate;
    u32  dwStart;
    u32  dwLength;
    u32  dwSuggestedBufferSize;
    u32  dwQuality;
    u32  dwSampleSize;
    u16   rcFrame[4];
};
#pragma pack(pop)

bool writeAVI(PlatformNativePathString path, MainEditor* session);