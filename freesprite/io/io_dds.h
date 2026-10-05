#pragma once

#include "../globals.h"

#pragma pack(push, 1)
struct DDS_DX10_HEADER {
	u32 dxgiFormat;
	u32 resourceDimension;
	u32 miscFlag;
	u32 arraySize;
	u32 miscFlags2;
};
#pragma pack(pop)

void DeXT1(Layer* ret, int width, int height, FILE* infile);
void DeXT23(Layer* ret, int width, int height, FILE* infile);
void DeXT45(Layer* ret, int width, int height, FILE* infile);
void DeBC7(Layer* ret, int width, int height, FILE* infile);

Layer* readDDS(PlatformNativePathString path, uint64_t seek = 0);
