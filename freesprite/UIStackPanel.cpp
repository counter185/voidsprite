#include "UIStackPanel.h"


UIStackPanel* UIStackPanel::FromContent(std::vector<Drawable*> content)
{
    UIStackPanel* panel = new UIStackPanel();
    for (Drawable* d : content) {
        panel->addWidget(d);
    }
    panel->manuallyRecalculateLayout = true;
    panel->recalculateLayout();
    return panel;
}

UIStackPanel::UIStackPanel()
{
    passThroughMouse = true;
    sizeToContent = true;
}

UIStackPanel* UIStackPanel::Vertical(int spacing, std::vector<Drawable*> content)
{
    auto panel = FromContent(content);
    panel->orientationVertical = true;
    panel->spacing = spacing;
    panel->recalculateLayout();
    return panel;
}

UIStackPanel* UIStackPanel::Horizontal(int spacing, std::vector<Drawable*> content)
{
    auto panel = FromContent(content);
    panel->orientationVertical = false;
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