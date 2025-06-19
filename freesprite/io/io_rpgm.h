#pragma once
#include "globals.h"

Layer* readXYZ(PlatformNativePathString path, uint64_t seek = 0);
bool writeXYZ(PlatformNativePathString path, Layer* data);
MainEditor* readLMU(PlatformNativePathString path);