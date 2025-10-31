#pragma once
#include "Panel.h"
class UIStackPanel :
    public Panel
{
protected:
    static UIStackPanel* FromContent(std::vector<Drawable*> content);
public:
    bool orientationVertical = true;
    bool manuallyRecalculateLayout = false;
    XY contentBoxSize = XY{ 0,0 };
    int spacing = 0;

    UIStackPanel();

    static UIStackPanel* Vertical(int spacing, std::vector<Drawable*> content);
    static UIStackPanel* Horizontal(int spacing, std::vector<Drawable*> content);

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

