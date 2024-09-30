#pragma once
#include "globals.h"
#include "UIButton.h"

class ButtonStartScreenSession :
    public UIButton
{
public:
    int correspondingScreen = -1;
    
    ButtonStartScreenSession(int screenIndex) {
        correspondingScreen = screenIndex;
        wxWidth = 16;
        wxHeight = 16;
        instantTooltip = true;
    }

    void render(XY pos) override;

    void click() override;
    void renderTooltip(XY pos) override;
};

