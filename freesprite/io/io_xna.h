#pragma once

#include "../globals.h"

#pragma pack(push,1)
struct XNBHeader {
	char magic[3];
	u8 targetPlatform;
	u8 xnbVersion;
	u8 flags;
	u32 compressedFileSize;
};

enum XNATexture2DFormat : s32 {
	XNATexture_Color = 0,
	XNATexture_BGR565 = 1,
	XNATexture_BGRA5551 = 2,
	XNATexture_BGRA4444 = 3,
	XNATexture_DXT1 = 4,
	XNATexture_DXT3 = 5,
	XNATexture_DXT5 = 6,
	XNATexture_NormalizedByte2 = 7,
	XNATexture_NormalizedByte4 = 8,
	XNATexture_R10G10B10A2 = 9,
	XNATexture_RG32 = 10,
	XNATexture_RGBA64 = 11,
	XNATexture_A8 = 12
};

struct XNATexture2DHeader {
	s32 surfaceFormat;
	s32 w;
	s32 h;
	s32 mipCount;
};

struct XNAColor {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
};
#pragma pack(pop)

s32 XNA_Read7BE(FILE* f);
std::string XNA_ReadNullTerminatedString(FILE* f);

Layer* readXNB(PlatformNativePathString path, uint64_t seek = 0);