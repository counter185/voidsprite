#pragma once
#include "BaseTemplate.h"
#include "maineditor.h"

class TemplatePixelIllustration :
    public BaseTemplate
{
private:
    XY size = { 1,1 };
	std::string name = "";
public:
	TemplatePixelIllustration(XY size) : size(size) {
		name = frmt("{}x{} {}", size.x, size.y, TL("vsp.template.pixelillustration"));
	}
	std::string getName() override { return name; };
	MainEditor* generateSession() override { return new MainEditor(size); }
};

