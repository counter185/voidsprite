#pragma once
#include "BaseTemplate.h"
class TemplateRPG2KFaceset :
    public BaseTemplate
{
public:
    std::string getName() override { return "RPG2K/2K3 Faceset Template"; };
    Layer* generate() override;
    XY tileSize() override { return XY{ 48,48 }; }
};

