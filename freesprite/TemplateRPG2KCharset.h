#pragma once
#include "BaseTemplate.h"
class TemplateRPG2KCharset :
    public BaseTemplate
{
public:
    std::string getName() override { return "RPG2K/2K3 Charset Template"; };
    Layer* generate() override;
    XY tileSize() override { return XY{ 24,32 }; }
};

