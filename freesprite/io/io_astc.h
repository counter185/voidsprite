#pragma once

#include "../globals.h"

#pragma pack(push, 1)
struct astc_header
{
    uint8_t magic[4];
    uint8_t block_x;
    uint8_t block_y;
    uint8_t block_z;
    u16 dim_x;
    u8 pad;
    u16 dim_y;
    u8 pad2;
    uint8_t dim_z[3];
};
#pragma pack(pop)

bool DeASTCWithoutDeswizzle(Layer* ret, FILE* infile, int blockW, int blockH, OperationProgressReport* progress);
int DeASTC(Layer* ret, int width, int height, uint64_t fileLength, FILE* infile, int blockWidth = 8, int blockHeight = 8);

Layer* readASTC(PlatformNativePathString path, u64 seek = 0, OperationProgressReport* report = NULL);