#pragma once
#include "../globals.h"

MainEditor* readLPE(PlatformNativePathString path);
bool writeLPE(PlatformNativePathString path, MainEditor* editor);