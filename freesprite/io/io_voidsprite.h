#pragma once

#include "../globals.h"

bool writeVOIDSNv1(PlatformNativePathString, XY projDimensions, std::vector<Layer*> data);
bool writeVOIDSNv2(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv3(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv4(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv5(PlatformNativePathString path, MainEditor* editor);

MainEditor* readVOIDSN(PlatformNativePathString path);

std::pair<bool, std::vector<uint32_t>> readPltVOIDPLT(PlatformNativePathString name);