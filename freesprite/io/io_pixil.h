#pragma once
#include "../globals.h"

MainEditor* readPIXIL(PlatformNativePathString path);
bool writePIXIL(PlatformNativePathString path, MainEditor* editor);