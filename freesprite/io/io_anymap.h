#pragma once
#include "../globals.h"

Layer* readAnymapPBM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAnymapPGM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAnymapPPM(PlatformNativePathString path, uint64_t seek, OperationProgressReport* report);

bool writeAnymapTextPBM(PlatformNativePathString path, Layer* data);
bool writeAnymapPGM(PlatformNativePathString path, Layer* data, OperationProgressReport* report, ParameterStore* params);
bool writeAnymapPPM(PlatformNativePathString path, Layer* data, OperationProgressReport* report, ParameterStore* params);