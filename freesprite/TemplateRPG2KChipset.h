#pragma once
#include "BaseTemplate.h"
class TemplateRPG2KChipset :
    public BaseTemplate
{
public:
    std::string getName() override { return "RPG2K/2K3 Chipset Template"; };
    Layer* generate() override;
    XY tileSize() override { return XY{ 16,16 }; }
    std::vector<CommentData> placeComments() override;
};

