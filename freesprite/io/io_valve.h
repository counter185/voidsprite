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

struct VTFHEADER
{
    char            signature[4];       // File signature ("VTF\0"). (or as little-endian integer, 0x00465456)
    unsigned int    version[2];         // version[0].version[1] (currently 7.2).
    unsigned int    headerSize;         // Size of the header struct  (16 byte aligned; currently 80 bytes) + size of the resources dictionary (7.3+).
    unsigned short  width;              // Width of the largest mipmap in pixels. Must be a power of 2.
    unsigned short  height;             // Height of the largest mipmap in pixels. Must be a power of 2.
    unsigned int    flags;              // VTF flags.
    unsigned short  frames;             // Number of frames, if animated (1 for no animation).
    unsigned short  firstFrame;         // First frame in animation (0 based). Can be -1 in environment maps older than 7.5, meaning there are 7 faces, not 6.
    unsigned char   padding0[4];        // reflectivity padding (16 byte alignment).
    float           reflectivity[3];    // reflectivity vector.
    unsigned char   padding1[4];        // reflectivity padding (8 byte packing).
    float           bumpmapScale;       // Bumpmap scale.
    int             highResImageFormat; // High resolution image format.
    unsigned char   mipmapCount;        // Number of mipmaps.
    int             lowResImageFormat;  // Low resolution image format (Usually DXT1).
    unsigned char   lowResImageWidth;   // Low resolution image width.
    unsigned char   lowResImageHeight;  // Low resolution image height.

    // 7.2+
    unsigned short  depth;              // Depth of the largest mipmap in pixels. Must be a power of 2. Is 1 for a 2D texture.

    // 7.3+
    unsigned char   padding2[3];        // depth padding (4 byte alignment).
    unsigned int    numResources;       // Number of resources this vtf has. The max appears to be 32.

    unsigned char   padding3[8];        // Necessary on certain compilers
};
struct VTF_RESOURCE_ENTRY
{
    unsigned char	tag[3]; 		// A three-byte "tag" that identifies what this resource is.
    unsigned char	flags;			// Resource entry flags. The only known flag is 0x2, which indicates that no data chunk corresponds to this resource.
    unsigned int	offset;			// The offset of this resource's data in the file. 
};

//based on https://github.com/ValveResourceFormat/ValveResourceFormat/blob/master/docs/guides/read-resource.md
struct S2_COMPILED_HEADER {
    u32 fileSize;
    u16 headerVersion;
    u16 version;
    u32 blockOffset;
    u32 blockCount;
};

struct S2_BLOCK_ENTRY {
    u32 blockType;
    u32 offset;
    u32 size;
};

struct S2_VTEX_HEADER {
    u16 version;    //should be 1
    u16 flags;
    float reflectivity[4];
    u16 w;
    u16 h;
    u16 depth;
    u8 format;
    u8 mipCount;
    u32 picmip0Res;
    u32 extDataOffset;
    u32 extDataCount;
};

enum VTFFORMAT
{
    IMAGE_FORMAT_NONE = -1,
    IMAGE_FORMAT_RGBA8888 = 0,
    IMAGE_FORMAT_ABGR8888 = 1,
    IMAGE_FORMAT_RGB888 = 2,
    IMAGE_FORMAT_BGR888 = 3,
    IMAGE_FORMAT_RGB565 = 4,
    IMAGE_FORMAT_I8 = 5,
    IMAGE_FORMAT_IA88 = 6,
    IMAGE_FORMAT_P8 = 7,
    IMAGE_FORMAT_A8 = 8,
    IMAGE_FORMAT_RGB888_BLUESCREEN = 9,
    IMAGE_FORMAT_BGR888_BLUESCREEN = 10,
    IMAGE_FORMAT_ARGB8888 = 11,
    IMAGE_FORMAT_BGRA8888 = 12,
    IMAGE_FORMAT_DXT1 = 13,
    IMAGE_FORMAT_DXT3 = 14,
    IMAGE_FORMAT_DXT5 = 15,
    IMAGE_FORMAT_BGRX8888,
    IMAGE_FORMAT_BGR565,
    IMAGE_FORMAT_BGRX5551,
    IMAGE_FORMAT_BGRA4444,
    IMAGE_FORMAT_DXT1_ONEBITALPHA,
    IMAGE_FORMAT_BGRA5551,
    IMAGE_FORMAT_UV88,
    IMAGE_FORMAT_UVWQ8888,
    IMAGE_FORMAT_RGBA16161616F,
    IMAGE_FORMAT_RGBA16161616,
    IMAGE_FORMAT_UVLX8888
};

enum S2_VTEXFORMAT {
    VTEX_UNKNOWN = 0,
    VTEX_DXT1 = 1,
    VTEX_DXT5 = 2,
    VTEX_I8 = 3,
    VTEX_RGBA8888 = 4,
    VTEX_R16 = 5,
    VTEX_RG1616 = 6,
    VTEX_RGBA16161616 = 7,
    VTEX_R16F = 8,
    VTEX_RG1616F = 9,
    VTEX_RGBA16161616F = 10,
    VTEX_R32F = 11,
    VTEX_RG3232F = 12,
    VTEX_RGB323232F = 13,
    VTEX_RGBA32323232F = 14,
    VTEX_JPEG_RGBA8888 = 15,
    VTEX_PNG_RGBA8888 = 16,
    VTEX_JPEG_DXT5 = 17,
    VTEX_PNG_DXT5 = 18,
    VTEX_BC6H = 19,
    VTEX_BC7 = 20,
    VTEX_ATI2N = 21,
    VTEX_IA88 = 22,
    VTEX_ETC2 = 23,
    VTEX_ETC2_EAC = 24,
    VTEX_R11_EAC = 25,
    VTEX_RG11_EAC = 26,
    VTEX_ATI1N = 27,
    VTEX_BGRA8888 = 28,
    VTEX_WEBP_RGBA8888 = 29,
    VTEX_WEBP_DXT5 = 30
};
#pragma pack(pop)

Layer* _VTFseekToLargestMipmapAndRead(FILE* infile, int width, int height, int mipmapCount, int frames, int imageFormat);

MainEditor* readValveSPR(PlatformNativePathString path);
Layer* readVTF(PlatformNativePathString path, uint64_t seek = 0);

bool writeVTF(PlatformNativePathString path, Layer* data, OperationProgressReport* report, ParameterStore* params);

Layer* readValveVTEX_C(PlatformNativePathString path, u64 seek = 0);