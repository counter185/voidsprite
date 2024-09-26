#pragma once
#include "BaseTemplate.h"
class TemplateRPG2KSystem :
    public BaseTemplate
{
public:
    std::string getName() override { return "RPG2K/2K3 System Template"; };
    Layer* generate() override;
    XY tileSize() override { return XY{ 0,0 }; }
    std::vector<CommentData> placeComments() override;
};

