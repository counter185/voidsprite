#pragma once
#include "../globals.h"

struct PNGChunk {
    u32 length;
    char type[5];
    std::vector<u8> data;
    u32 crc;
};

#pragma pack(push, 1)
struct PNGfcTLChunk {
    u32 sequenceNumber;
    u32 width;
    u32 height;
    u32 xOffset;
    u32 yOffset;
    u16 delayNum;
    u16 delayDen;
    u8 disposeOp;
    u8 blendOp;
};
#pragma pack(pop)


std::vector<PNGChunk> loadPNGChunksFromMem(std::vector<u8>& data);
void calcCRCAndSize(PNGChunk* chunk);
void writeChunkToFile(FILE* f, PNGChunk* chunk);

Layer* readPNGFromBase64String(std::string b64);
Layer* readPNGFromMem(uint8_t* data, size_t dataSize);
std::vector<u8> writePNGToMem(Layer* l);

Layer* readPNG(PlatformNativePathString path, uint64_t seek = 0);
bool writePNG(PlatformNativePathString path, Layer* data);

bool writeAPNG(PlatformNativePathString path, MainEditor* data);

std::string getlibpngVersion();