#pragma once
#include "Panel.h"
class UIStackPanel :
    public Panel
{
public:
    bool orientationVertical = true;
    bool manuallyRecalculateLayout = false;
    XY contentBoxSize = XY{ 0,0 };

    UIStackPanel();

    void render(XY at) override;

    void addWidget(Drawable* d);
    void recalculateLayout();

    XY getDimensions() override {
        if (manuallyRecalculateLayout) {
            return contentBoxSize;
        }
        return Panel::getDimensions();
    }
};

