#pragma once
#include "FileIO.h"
#include "mathops.h"

#define HINT_NEXT_LINE "hint:newline"
#define HINT_NEXT_LINE_HERE {HINT_NEXT_LINE,0}

//filled out at runtime in main.cpp
inline std::map<std::string, u32> g_colors;

struct NamedColorPalette {
    std::string name;
    std::vector<std::pair<std::string, u32>> colorMap;
};

inline std::vector<NamedColorPalette> g_namedColorMap = {};

void g_generateColorMap();
void g_reloadColorMap();