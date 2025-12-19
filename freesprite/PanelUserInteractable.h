#pragma once
#include "Panel.h"
class PanelUserInteractable :
    public Panel
{
private:
    Panel* widgetsTarget = NULL;

    bool draggable = false;
    bool collapsible = false;
    bool resizable = false;

    bool dragging = false;
    bool wasDragged = false;

    const int resizeDistance = 15;
    bool resizing = false;
    XY minResizableSize = XY{ 0,0 };

    UIButton* collapseButton = NULL;
    Panel* collapsePanel = NULL;

    UIButton* closeButton = NULL;

    void processDrag(SDL_Event evt);
    bool processResize(SDL_Event evt);
    bool PointInResizeRange(XY pos);
protected:
    SDL_Color focusBorderColor = { 255,255,255,255 };
    double focusBorderLightup = 0.0;

    bool collapsed = false;

    DrawableManager& wxsTarget() { return widgetsTarget != NULL ? widgetsTarget->subWidgets : subWidgets; }

    virtual void drawPanelBackground(XY at);

    void setupDraggable();
    void setupCollapsible();
    void setupResizable(XY minDimensions = XY{ 0,0 });
    void setupCloseButton(std::function<void()> callback);

    UILabel* addTitleText(std::string title);

    virtual bool defaultInputAction(SDL_Event evt, XY at) { return false; }
    virtual void renderAfterBG(XY at) {}
    void renderResizeHandle(XY at);
    void repositionCloseButton();
public:
    PanelUserInteractable();

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    void render(XY at) override {
        if (enabled) {
            drawPanelBackground(at);
            if (focused) {
                renderFocusBorder(at, focusBorderColor, focusBorderLightup);
            }
            if (!collapsible || !collapsed) {
                if (resizable) {
                    renderResizeHandle(at);
                }
                renderAfterBG(at);
            }
            Panel::render(at);
        }
    }
    void handleInput(SDL_Event evt, XY gPosOffset) override;

    XY getDimensions() override { return (collapsible && collapsed) ? XY{wxWidth, 30} : Panel::getDimensions(); }

    void toggleCollapse();
    void tryMoveOutOfOOB();
};

