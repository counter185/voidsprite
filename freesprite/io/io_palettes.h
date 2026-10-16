#pragma once
#include "../globals.h"

//palette io that doesn't fit anywhere else goes here and io_palettes.cpp
//(so voidplt still goes into io_voidsprite.h/cpp)

IPalette* readPltJASCPAL(PlatformNativePathString name);
IPalette* readPltGIMPGPL(PlatformNativePathString name);
IPalette* readPltHEX(PlatformNativePathString name);
IPalette* readPltPDNTXT(PlatformNativePathString name);

bool writePltHEX(PlatformNativePathString path, IPalette* palette);