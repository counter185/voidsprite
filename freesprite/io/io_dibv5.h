#pragma once
#include "../globals.h"

#pragma pack(push, 1)
struct vsp_BITMAPV5HEADER {
    u32        bV5Size;
    u32        bV5Width;
    u32        bV5Height;
    u16        bV5Planes;
    u16        bV5BitCount;
    u32        bV5Compression;
    u32        bV5SizeImage;
    u32        bV5XPelsPerMeter;
    u32        bV5YPelsPerMeter;
    u32        bV5ClrUsed;
    u32        bV5ClrImportant;
    u32        bV5RedMask;
    u32        bV5GreenMask;
    u32        bV5BlueMask;
    u32        bV5AlphaMask;
    u32        bV5CSType;
    u8         bV5Endpoints[36];    //something about CIEXYZ, don't care
    u32        bV5GammaRed;
    u32        bV5GammaGreen;
    u32        bV5GammaBlue;
    u32        bV5Intent;
    u32        bV5ProfileData;
    u32        bV5ProfileSize;
    u32        bV5Reserved;
};
#pragma pack(pop)

Layer* readDIBv5FromMem(u8* mem, u64 size);
std::vector<u8> writeDIBv5ToMem(Layer* image);

Layer* readDIBV5(PlatformNativePathString path, uint64_t seek = 0);
bool writeDIBV5(PlatformNativePathString path, Layer* data);