#pragma once
#include "globals.h"
#include "Layer.h"

Layer* readXYZ(PlatformNativePathString path);
Layer* readPNG(PlatformNativePathString path);
Layer* readTGA(std::string path);
Layer* readBMP(PlatformNativePathString path);
Layer* readAETEX(PlatformNativePathString path);
MainEditor* readVOIDSN(PlatformNativePathString path);

bool writePNG(PlatformNativePathString path, Layer* data);
bool writeVOIDSNv1(PlatformNativePathString, XY projDimensions, std::vector<Layer*> data);
bool writeXYZ(PlatformNativePathString path, Layer* data);