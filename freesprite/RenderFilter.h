#pragma once
#include "BaseFilter.h"
#include "Layer.h"

/// <summary>
/// BaseFilter that renders something instead of manipulating pixel data. May or may not use what's already in the layer.
/// </summary>
class RenderFilter : public BaseFilter
{};

class GenNoiseFilter : public RenderFilter 
{
public:
	std::string name() override { return "Random noise"; }
	Layer* run(Layer* src, std::map<std::string, std::string> options) override {
        bool grayscale = std::stod(options["grayscale"]) == 1;
		bool black_to_alpha = std::stod(options["black is alpha"]) == 1;
		Layer* c = copy(src);
		u32* px = (u32*)c->pixelData;

		for (int i = 0; i < c->w * c->h; i++) {
			if (grayscale) {
                u8 r = rand() % 256;
                px[i] = black_to_alpha ? PackRGBAtoARGB(0xff,0xff,0xff, r) : PackRGBAtoARGB(r, r, r, 0xFF);
			}
			else {
				px[i] = rand() % 2 == 0 ? (black_to_alpha ? 0x00000000 : 0xFF000000) : 0xFFFFFFFF;
			}
		}
		return c;
	}
	virtual std::vector<FilterParameter> getParameters() { return {
		BOOL_PARAM("grayscale", 1),
		BOOL_PARAM("black is alpha", 1),
	}; }

};