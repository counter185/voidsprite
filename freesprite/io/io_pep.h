#pragma once
#include "../globals.h"

Layer* readPEP(PlatformNativePathString path, u64 seek = 0);
bool writePEP(PlatformNativePathString path, Layer* editor);