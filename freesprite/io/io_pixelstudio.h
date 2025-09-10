#pragma once
#include "../globals.h"

MainEditor* readPixelStudioPSP(PlatformNativePathString path);
MainEditor* readPixelStudioPSX(PlatformNativePathString path);

bool writePixelStudioPSP(PlatformNativePathString path, MainEditor* data);
bool writePixelStudioPSX(PlatformNativePathString path, MainEditor* data);