#pragma once

#include "../globals.h"

#ifndef VOIDSPRITE_JXL_ENABLED
#define VOIDSPRITE_JXL_ENABLED 1
#endif

std::string getlibjxlVersion();

MainEditor* readJpegXL(PlatformNativePathString path, OperationProgressReport* progress);
bool writeJpegXL(PlatformNativePathString path, MainEditor* data);