#pragma once
#include "Panel.h"

#define RESIZE_VERTICALLY 0b10
#define RESIZE_HORIZONTALLY 0b1
#define RESIZE_ALL 0b11

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
    u8 resizeFlags = RESIZE_ALL;
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
    void setupResizable(XY minDimensions = XY{ 0,0 }, u8 resizeFlags = RESIZE_ALL);
    void setupCloseButton(std::function<void()> callback);
    
    virtual bool defaultInputAction(SDL_Event evt, XY at) { return false; }
    virtual void renderAfterBG(XY at) {}
    virtual void panelResized(XY from, XY to) {}
    void renderResizeHandle(XY at);
    void repositionCloseButton();
public:
    XY anchor = XY{ 0,0 };

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
    void windowResized(XY from, XY to) override;
    
    XY getDimensions() override { return (collapsible && collapsed) ? XY{wxWidth, 30} : Panel::getDimensions(); }
    
    void toggleCollapse();
    void tryMoveOutOfOOB();
    void reanchor();
    void reanchorInnerDrawable(Drawable* d, XY oldSize, XY newSize, XY anchor);

    UILabel* addTitleText(std::string title);
};

