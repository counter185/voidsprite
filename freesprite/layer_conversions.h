#pragma once
#include "globals.h"

struct LayerOpResult {
	bool success = false;
	Layer* outLayer = NULL;
	int alphaIndex = -1;
};

Layer* quantizeToNumColors(Layer* rgb, int numColors);

bool hasTransparency(Layer* rgba, u8 threshold = 127);

LayerOpResult to8BitIndexedNoAlpha(Layer* rgb);
LayerOpResult to8BitIndexed1BitAlpha(Layer* rgba);

LayerOpResult to8BitIndexedWith1BitSingleIndexAlpha(LayerPalettized* idx);