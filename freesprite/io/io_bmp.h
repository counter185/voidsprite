#pragma once

#include "../globals.h"

#pragma pack(push, 1)
struct vsp_BITMAPFILEHEADER {
	u16 magic;
	u32 fileSize;
	u32 reserved;
	u32 dataOffset;
};
struct vsp_BITMAPINFOHEADER {
	u32 size;
	u32 width;
	u32 height;
	u16 planes;
	u16 bitCount;
	u32 compression;
	u32 imageSize;
	u32 xPixelsPerM;
	u32 yPixelsPerM;
	u32 colorsUsed;
	u32 colorsImportant;
};
#pragma pack(pop)

void bmpEncode8BitEncodedMode(u8 value, int count, std::vector<u8>& ret);
void bmpEncode8BitAbsoluteMode(std::vector<u8>& values, std::vector<u8>& ret);
std::vector<u8> bmpEncode8BitRLELine(u32* values, int n);
void bmpEncode8BitRLE(LayerPalettized* data, FILE* f);

void bmpDecode8BitRLE(Layer* ret, FILE* f);
void bmpDecode4BitRLE(Layer* ret, FILE* f);
void bmpWriteToFile(Layer* data, FILE* f, OperationProgressReport* progress = NULL, ParameterStore* params = NULL);

Layer* readBMP(PlatformNativePathString path, u64 seek = 0, OperationProgressReport* report = NULL);
bool writeBMP(PlatformNativePathString path, Layer* data, OperationProgressReport* progress = NULL, ParameterStore* params = NULL);

bool writeCaveStoryPBM(PlatformNativePathString path, Layer* data, OperationProgressReport* progress = NULL);