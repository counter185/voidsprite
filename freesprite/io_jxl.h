#pragma once

#include "globals.h"

#define VOIDSPRITE_JXL_ENABLED 1

Layer* readJpegXL(PlatformNativePathString path, u64 seek = 0);
bool writeJpegXL(PlatformNativePathString path, Layer* data);