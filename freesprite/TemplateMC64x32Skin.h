#pragma once
#include "BaseTemplate.h"
class TemplateMC64x32Skin :
    public BaseTemplate
{
    std::string getName() override { return "Minecraft 64x32 Skin Template"; };
    Layer* generate() override;
};

