#pragma once
#include "BaseTemplate.h"
#include "Layer.h"

class TemplatePixelIllustration :
    public BaseTemplate
{
private:
    XY size = { 1,1 };
	std::string name = "";
public:
	TemplatePixelIllustration(XY size) : size(size) {
		name = std::format("{}x{} {}", size.x, size.y, TL("vsp.template.pixelillustration"));
	}
	std::string getName() override { return name; };
	Layer* generate() override {
		return Layer::tryAllocLayer(size.x, size.y);
	}
};

