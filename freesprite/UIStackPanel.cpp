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
    if (g_debugConfig.debugShowScrollPanelBounds) {
        SDL_Rect r = SDL_Rect{ at.x, at.y, contentBoxSize.x, contentBoxSize.y };
        SDL_SetRenderDrawColor(g_rd, 255, 0, 0, 255);
        SDL_RenderDrawRect(g_rd, &r);
    }
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