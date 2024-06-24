#pragma once
#include "UISlider.h"
class UIColorSlider : public UISlider
{
public:
    uint32_t colorMin = 0x000000;
    uint32_t colorMax = 0xFFFFFF;

    void render(XY pos) override;
};

