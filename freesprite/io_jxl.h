#pragma once

#include "globals.h"

Layer* readJpegXL(PlatformNativePathString path, u64 seek = 0);
bool writeJpegXL(PlatformNativePathString path, Layer* data);