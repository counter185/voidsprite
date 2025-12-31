#pragma once
#include "../globals.h"

#pragma pack(push, 1)
struct ASEPRITEHeader {
    u32 fileSize;
    u16 magic;  //0xA5E0
    u16 numFrames;
    u16 width;
    u16 height;
    u16 colorDepth; //32: RGBA, 16: grayscale ????, 8: indexed
    u32 flags;
    u16 speedDeprecated;
    u32 reserved0[2];
    u8 paletteEntryThatIsTransparent;
    u8 ignore[3];
    u16 numColors;  //0 is 256 for old files
    u8 pixelWidth;
    u8 pixelHeight;
    s16 gridX;
    s16 gridY;
    u16 gridWidth;
    u16 gridHeight;
    u8 reserved1[84];
};

struct ASEPRITEFrameHeader {
    u32 bytesInFrame;
    u16 magic;  //0xF1FA
    u16 numChunksOld;
    u16 frameDuration;
    u8 reserved[2];
    u32 numChunksNew;
};

struct ASEPRITECelChunkFragment0 {
    u16 layerIndex;
    s16 x;
    s16 y;
    u8 opacity;
    u16 type;
    s16 zIndex;
    u8 reserved[5];
};

struct ASEPRITECelChunkFragment03 {
    u16 widthInTiles;
    u16 heightInTiles;
    u16 bitsPerTile;    //always 32 right now?
    u32 tileIDBitmask;
    u32 xFlipBitmask;
    u32 yFlipBitmask;
    u32 diagonalFlipBitmask;
    u8 reserved[10];
};

struct ASEPRITELayerChunkFragment0 {
    u16 flags;
    u16 layerType;
    u16 layerChildLevel;
    u16 defaultWidth;   //ignored
    u16 defaultHeight;  //ignored
    u16 blendMode;
    u8 opacity;
    u8 reserved[3];
};

struct ASEPRITEColorProfileChunk {
    u16 type;
    u16 flags;
    float fixedGamma;
    u8 reserved[8];
};

struct ASEPRITERGBAPixel {
    u8 r, g, b, a;
};
struct ASEPRITEIA88Pixel {
    u8 i, a;
};

struct ASEPRITEChunkHeader {
    u32 size;
    u16 type;
};
#pragma pack(pop)

struct ASELayer {
    std::string name;
    bool hidden;
    int id;
};

std::vector<int> ASELayerEvalOrder(std::vector<ASELayer>& current, std::vector<Layer*> target, int* nextLayerID);
void ASEWriteCellChunk(Layer* l, FILE* f, int layerID, u32* bytesWritten);

inline void writeASEString(std::string a, FILE* f) {
    u16 b = a.size();
    fwrite(&b, 2, 1, f);
    fwrite(a.c_str(), a.size(), 1, f);
}


MainEditor* readAsepriteASE(PlatformNativePathString path);
bool writeAsepriteASE(PlatformNativePathString path, MainEditor* editor);