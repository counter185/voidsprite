#pragma once

#include "globals.h"

#ifndef VOIDSPRITE_JXL_ENABLED
#define VOIDSPRITE_JXL_ENABLED 1
#endif

std::string getlibjxlVersion();

Layer* readJpegXL(PlatformNativePathString path, u64 seek = 0);
bool writeJpegXL(PlatformNativePathString path, Layer* data);