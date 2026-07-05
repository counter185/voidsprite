#pragma once
#include "../globals.h"

std::vector<u8> galNextBufferPair(FILE* f);
Layer* galDecode24bit(int w, int h, std::vector<u8>& pixelData);

MainEditor* readGAL(PlatformNativePathString path, OperationProgressReport* progress);