#pragma once
#include "Panel.h"
class PanelUserInteractable :
    public Panel
{
private:
    Panel* widgetsTarget = NULL;

    bool draggable = false;
    bool collapsible = false;

    bool dragging = false;
    bool wasDragged = false;

    bool collapsed = false;
    UIButton* collapseButton = NULL;
    Panel* collapsePanel = NULL;

    void processDrag(SDL_Event evt);
    void tryMoveOutOfOOB();
protected:
    DrawableManager& wxsTarget() { return widgetsTarget != NULL ? widgetsTarget->subWidgets : subWidgets; }

    virtual void drawPanelBackground(XY at);

    void setupDraggable();
    void setupCollapsible();

    UILabel* addTitleText(std::string title);

    virtual bool defaultInputAction(SDL_Event evt, XY at) { return false; }
public:

    void render(XY at) override {
        if (enabled) {
            drawPanelBackground(at);
            Panel::render(at);
        }
    }
    void handleInput(SDL_Event evt, XY gPosOffset) override;

    XY getDimensions() override { (collapsible && collapsed) ? XY{wxWidth, 30} : Panel::getDimensions(); }

    void toggleCollapse();
};

