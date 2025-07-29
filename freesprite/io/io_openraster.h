#pragma once
#include "../globals.h"

MainEditor* readOpenRaster(PlatformNativePathString path);
bool writeOpenRaster(PlatformNativePathString path, MainEditor* data);