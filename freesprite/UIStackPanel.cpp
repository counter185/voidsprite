#include "UIStackPanel.h"


UIStackPanel* UIStackPanel::FromContent(std::vector<Drawable*> content, bool vertical)
{
    UIStackPanel* panel = new UIStackPanel();
    panel->orientationVertical = vertical;
    for (Drawable* d : content) {
        if (d != NULL) {
            panel->addWidget(d);
        }
    }
    panel->position = {0,0};
    panel->manuallyRecalculateLayout = true;
    panel->recalculateLayout();
    return panel;
}

UIStackPanel::UIStackPanel()
{
    passThroughMouse = true;
    sizeToContent = true;
}

UIStackPanel* UIStackPanel::Vertical(int spacing, std::vector<Drawable*> content, XY position)
{
    auto panel = FromContent(content, true);
    panel->position = position;
    panel->spacing = spacing;
    panel->recalculateLayout();
    return panel;
}

UIStackPanel* UIStackPanel::Horizontal(int spacing, std::vector<Drawable*> content, XY position)
{
    auto panel = FromContent(content, false);
    panel->position = position;
    panel->spacing = spacing;
    panel->recalculateLayout();
    return panel;
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
            nextPos = xyAdd(nextPos, orientationVertical ? XY{ 0, size.y + spacing } : XY{ size.x + spacing, 0 });
        }
    }
    contentBoxSize = getContentBoxSize();
}

HAlignPoint::~HAlignPoint() { if (--parent->refCount == 0) { delete parent; } }

XY HAlignPoint::getDimensions() {
    if (!xPointSet) {
        originalXPoint = position.x;
        xPointSet = true;
    }
    parent->updateAlignPoints();
    return { w, 1 };
}
