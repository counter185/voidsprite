#pragma once
#include "../globals.h"

//palette io that doesn't fit anywhere else goes here and io_palettes.cpp
//(so voidplt still goes into io_voidsprite.h/cpp)

std::pair<bool, std::vector<uint32_t>> readPltJASCPAL(PlatformNativePathString name);
std::pair<bool, std::vector<uint32_t>> readPltGIMPGPL(PlatformNativePathString name);
std::pair<bool, std::vector<uint32_t>> readPltHEX(PlatformNativePathString name);
std::pair<bool, std::vector<uint32_t>> readPltPDNTXT(PlatformNativePathString name);

bool writePltHEX(PlatformNativePathString path, std::vector<u32> palette);