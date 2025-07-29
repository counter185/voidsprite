#pragma once
#include "../globals.h"

MainEditor* readPix2D(PlatformNativePathString path);
bool writePix2D(PlatformNativePathString path, MainEditor* editor);