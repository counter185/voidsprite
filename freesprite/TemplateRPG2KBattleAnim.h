#pragma once
#include "BaseTemplate.h"
class TemplateRPG2KBattleAnim :
    public BaseTemplate
{
public:
    std::string getName() override { return "RPG2K/2K3 Battle Anim. Template"; };
    std::string getTooltip() override { return "96x96 battle animation spritesheet for RPG Maker 2000/2003"; }
    Layer* generate() override;
    XY tileSize() override { return XY{ 96,96 }; }
    std::vector<CommentData> placeComments() override;
};

