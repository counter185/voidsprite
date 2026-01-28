#include "globals.h"

Layer* quantizeToNumColors(Layer* rgb, int numColors);

LayerPalettized* to8BitIndexedNoAlpha(Layer* rgb);
LayerPalettized* to8BitIndexed1BitAlpha(Layer* rgba);