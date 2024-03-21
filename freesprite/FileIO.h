#pragma once
#include "globals.h"
#include "Layer.h"

Layer* readXYZ(std::wstring path);
Layer* readPNG(std::wstring path);
Layer* readTGA(std::string path);
Layer* readAETEX(std::wstring path);
MainEditor* readVOIDSN(std::wstring path);

bool writePNG(std::wstring path, Layer* data);
bool writeVOIDSNv1(std::wstring path, XY projDimensions, std::vector<Layer*> data);