#pragma once
#include "../globals.h"

MainEditor* readPISKEL(PlatformNativePathString path);
bool writePISKEL(PlatformNativePathString path, MainEditor* editor);