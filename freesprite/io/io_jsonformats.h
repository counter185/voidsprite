#pragma once
#include "../globals.h"

//(do not move pixel studio code here, it's complex enough for its own file)

//Lospec Pixel Editor
MainEditor* readLPE(PlatformNativePathString path);
bool writeLPE(PlatformNativePathString path, MainEditor* editor);

//Pixilart
MainEditor* readPIXIL(PlatformNativePathString path);
bool writePIXIL(PlatformNativePathString path, MainEditor* editor);

//Piskel
MainEditor* readPISKEL(PlatformNativePathString path);
bool writePISKEL(PlatformNativePathString path, MainEditor* editor);

//JPixel
MainEditor* readJPixel(PlatformNativePathString path);
//bool writeJPixel(PlatformNativePathString path, MainEditor* editor);