#pragma once
#include "globals.h"
#include "Layer.h"

Layer* readXYZ(std::string path);
Layer* readPNG(std::string path);
Layer* readTGA(std::string path);
Layer* readAETEX(std::string path);