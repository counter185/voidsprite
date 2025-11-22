#pragma once
#include "../globals.h"

#pragma pack(push, 1)
struct IWIFile {
    char magic[3];
    u8 version;
};
struct IWIInfo {
    u8 format;
    u8 usage;
    u16 w;
    u16 h;
    u16 depth;
};
#pragma pack(pop)

Layer* readIWI(PlatformNativePathString path, u64 seek = 0);