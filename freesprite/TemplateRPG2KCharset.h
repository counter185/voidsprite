#pragma once
#include "BaseTemplate.h"
class TemplateRPG2KCharset :
    public BaseTemplate
{
public:
    std::string getName() override { return "RPG2K/2K3 Charset Template"; };
    std::string getTooltip() override { return "24x32 character spritesheet for RPG Maker 2000/2003"; }
    Layer* generate() override;
    XY tileSize() override { return XY{ 24,32 }; }
    std::vector<CommentData> placeComments() override;
};

