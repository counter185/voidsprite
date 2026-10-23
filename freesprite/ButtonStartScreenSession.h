#pragma once
#include "globals.h"
#include "UIButton.h"

class ButtonStartScreenSession :
    public UIButton
{
public:
    int correspondingScreen = -1;
    VSPWindow* targetWindow = NULL;
    
    ButtonStartScreenSession(VSPWindow* parent, int screenIndex) {
		targetWindow = parent;
        correspondingScreen = screenIndex;
        wxWidth = UISCALED(16);
        wxHeight = UISCALED(16);
        instantTooltip = true;
    }

    void render(XY pos) override;

    void click() override;
    void rightClick() override;
    void renderTooltip(XY pos) override;
};

