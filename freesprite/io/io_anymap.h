#pragma once
#include "../globals.h"

Layer* readAnymapPBM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAnymapPGM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAnymapPPM(PlatformNativePathString path, uint64_t seek = 0);

bool writeAnymapTextPBM(PlatformNativePathString path, Layer* data);
bool writeAnymapPGM(PlatformNativePathString path, Layer* data, OperationProgressReport* report, ParameterStore* params);
bool writeAnymapTextPPM(PlatformNativePathString path, Layer* data);