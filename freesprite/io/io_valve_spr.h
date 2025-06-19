#pragma once
#include "../globals.h"

// below structs copied from
// https://developer.valvesoftware.com/wiki/SPR
#pragma pack(push, 1)
enum SPR_TransparencyType : u32 {
    SPR_Transparency_Normal = 0,
    SPR_Transparency_Additive = 1,
    SPR_Transparency_IndexAlpha = 2,
    SPR_Transparency_AlphaTest = 3
};

struct SPR_sprite
{
    u32         id;             // format ID, "IDSP" (0x49 0x44 0x53 0x50)
    u32         version;        // Format version number. HL1 SPRs are version 2 (0x02,0x00,0x00,0x00)
    u32         spriteType;     // Orientation method
    SPR_TransparencyType         textFormat;     // Translucency/Transparency method
    float       boundingRadius;
    u32         maxWidth;
    u32         maxHeight;
    u32         frameNum;          // number of frames the sprite contains
    float       beamLength;
    u32         synchType;

    short       paletteColorCount; // number of colors in the palette; should be 256
};
struct SPR_sprite_frame_header
{
    u32         group;
    u32         originX;        // not sure about this one, it always huge for sprites in HL1
    u32         originY;
    u32         width;
    u32         height;

    // Right after this, the paletted image data comes, each byte is a pixel.
    // The image size is given in the frame header, so the entire data for 1 frame is width*height bytes.
};
#pragma pack(pop)

Layer* readValveSPR(PlatformNativePathString path, uint64_t seek = 0);