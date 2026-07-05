#pragma once
#include "../globals.h"

std::vector<u8> galNextBufferPair(FILE* f);
Layer* galDecode24bit(int w, int h, std::vector<u8>& pixelData);

std::string galMakeXML(MainEditor* session);
std::vector<u8> galEncodeLayerRGB(Layer* layer);
std::vector<u8> galEncodeLayerAlphaMap(Layer* layer);
std::vector<u8> galEncodeBufferPair(std::vector<u8>& data);

MainEditor* readGAL(PlatformNativePathString path, OperationProgressReport* progress);
bool writeGAL(PlatformNativePathString path, MainEditor* session, OperationProgressReport* progress);