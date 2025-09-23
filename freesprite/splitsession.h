#pragma once

#include "globals.h"
#include "Timer64.h"

struct SplitSessionImage {
    std::string fileName;
    std::string originalFileName;
    XY positionInOverallImage = { 0,0 };
    XY dimensions = { 0, 0 };
    Layer* loadedLayer = NULL;
    FileExporter* exporter = NULL;

    Timer64 animTimer;
};

struct SplitSessionData {
    bool set = false;
    XY overallDimensions = {0,0};
    XY tileDimensions = { 0,0 };
    std::vector<SplitSessionImage> images;
};