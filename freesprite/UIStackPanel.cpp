#include "UIStackPanel.h"


UIStackPanel::UIStackPanel()
{
    passThroughMouse = true;
    sizeToContent = true;
}

void UIStackPanel::render(XY at)
{
    if (!manuallyRecalculateLayout) {
        recalculateLayout();
    }
    Panel::render(at);
}

void UIStackPanel::addWidget(Drawable* d) {
    subWidgets.addDrawable(d);
    recalculateLayout();
}

void UIStackPanel::recalculateLayout()
{
    XY nextPos = { 0,0 };
    for (Drawable* d : subWidgets.drawablesList) {
        d->position = nextPos;
        if (!d->isPanel() || ((Panel*)d)->enabled) {
            XY size = d->getRenderDimensions();
            nextPos = xyAdd(nextPos, orientationVertical ? XY{ 0, size.y } : XY{ size.x, 0 });
        }
    }
    contentBoxSize = getContentBoxSize();
}