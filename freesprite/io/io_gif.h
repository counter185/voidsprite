#pragma once
#include "../globals.h"

#pragma pack(push, 1)
struct GIFGCTFlags {
    u8 size : 3;
    u8 sort : 1;
    u8 colorRes : 3;
    u8 enabled : 1;
};
struct GIFLogicalScreenDescriptor {
    u16 width;
    u16 height;
    GIFGCTFlags gctFlags;
    u8 bgColorIndex;
    u8 pixelAspect;
};
struct GIFImageDescriptorFlags {
    u8 lctSize : 3;
    u8 reserved : 2;
    u8 lctSort : 1;
    u8 interlaceFlag : 1;
    u8 lctEnable : 1;
};
struct GIFImageDescriptor {
    u16 imageLeftPosition;
    u16 imageTopPosition;
    u16 imageWidth;
    u16 imageHeight;
    GIFImageDescriptorFlags flags;
};

struct GCE_Flags {
    u8 transparent : 1;
    u8 userInput : 1;
    u8 disposalMode : 3;
    u8 reserved : 3;
};

struct GIFGraphicControlExtension {
    u8 blockSize;
    GCE_Flags flags;
    u16 delay;
    u8 transparentColorIndex;
};

struct GIFApplicationExtension {
    u8 blockSize;
    char identifier[8];
    char authenticationCode[3];
};

struct GIFPlainTextExtension {
    u8 blockSize;
    u16 textGridLeftPos;
    u16 textGridTopPos;
    u16 textGridWidth;
    u16 textGridHeight;
    u8 charCellWidth;
    u8 charCellHeight;
    u8 textForegroundColorIndex;
    u8 textBackgroundColorIndex;
};
#pragma pack(pop)

std::vector<u8> GIFReadDataBlocks(FILE* f);

MainEditor* readGIF(PlatformNativePathString path, OperationProgressReport* progress);

bool writeGIF(PlatformNativePathString path, MainEditor* editor);