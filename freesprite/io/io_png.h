#pragma once
#include "globals.h"

Layer* readPNGFromBase64String(std::string b64);
Layer* readPNGFromMem(uint8_t* data, size_t dataSize);
std::vector<u8> writePNGToMem(Layer* l);

Layer* readPNG(PlatformNativePathString path, uint64_t seek = 0);
bool writePNG(PlatformNativePathString path, Layer* data);

std::string getlibpngVersion();