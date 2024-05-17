#include "BaseTemplate.h"
#include "Layer.h"

void BaseTemplate::drawPattern(Layer* layer, uint8_t* pattern, XY patternDimensions, XY position, uint32_t color)
{
	for (int y = 0; y < patternDimensions.y; y++) {
		for (int x = 0; x < patternDimensions.x; x++) {
			if (pattern[x + y * patternDimensions.x] != 0) {
				layer->setPixel(xyAdd(position, XY{ x,y }), color);
			}
		}
	}
}
