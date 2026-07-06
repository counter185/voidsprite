#pragma once
#include "../globals.h"

std::vector<u8> galNextBufferPair(FILE* f);
std::vector<u32> galParsePalette(std::string s);
Layer* galDecode24bit(int w, int h, std::vector<u8>& pixelData);
Layer* galDecode8Bit(int w, int h, std::vector<u8>& pixelData);

std::string galMakeXML(MainEditor* session);
std::vector<u8> galEncodeLayerRGB(Layer* layer);
std::vector<u8> galEncodeLayerIndexed(LayerPalettized* layer);
std::vector<u8> galEncodeLayerAlphaMap(Layer* layer);
std::vector<u8> galEncodeBufferPair(std::vector<u8>& data);
std::string galEncodePalette(std::vector<u32>& palette);

MainEditor* readGAL(PlatformNativePathString path, OperationProgressReport* progress);
bool writeGAL(PlatformNativePathString path, MainEditor* session, OperationProgressReport* progress);