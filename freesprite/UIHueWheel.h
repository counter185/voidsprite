#pragma once
#include "drawable.h"
class UIHueWheel :
    public Drawable
{
protected:
    void valueChanged(double v);
public:
    int wxWidth = 100;
    int wxHeight = 100;
    bool mouseDrag = false;
    UIColorPicker* parent;

    double value = 0;

    UIHueWheel(UIColorPicker* caller) {
        parent = caller;
    }

    void render(XY at) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    XY getDimensions() override { return { wxWidth, wxHeight }; }

    void renderWheelPosition(XY gPosOffset);
    SDL_Rect innerRect();
    double angleFromOriginAt(XY gPosOffset, XY mousePos);
    bool onScreenPosInWheelSlider(XY gPosOffset, XY mousePos);
    double outerDistance();
    double innerDistance();
};

