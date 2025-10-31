#pragma once
#include "drawable.h"
#include "DrawableManager.h"

class Panel :
    public Drawable
{
public:
    int wxWidth = 0;
    int wxHeight = 0;
    bool enabled = true;
    DrawableManager subWidgets;
    bool sizeToContent = false;
    Fill fillFocused = Fill::None();
    Fill fillUnfocused = Fill::None();
    u32 borderColor = 0x00FFFFFF;

    static Panel* Space(int w, int h) {
        Panel* p = new Panel();
        p->wxWidth = w;
        p->wxHeight = h;
        p->passThroughMouse = true;
        return p;
    }

    ~Panel() {
        subWidgets.freeAllDrawables();
    }

    /// <summary>
    /// Direct parent that should take over its focused state
    /// </summary>
    Panel* parent = NULL;
    bool passThroughMouse = false;
    bool takeMouseWheelEvents = true;

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    void render(XY position) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;
    void focusOut() override {
        Drawable::focusOut();
        subWidgets.forceUnfocus();
    }
    void mouseHoverOut() override {
        Drawable::mouseHoverOut();
        subWidgets.forceUnhover();
    }
    void mouseHoverMotion(XY mousePos, XY gPosOffset) override;
    void mouseWheelEvent(XY mousePos, XY gPosOffset, XYf direction) override;
    bool isPanel() override { return true; }

    bool takesMouseWheelEvents() override { return takeMouseWheelEvents; }
    bool takesTouchEvents() override { return true; }

    XY getDimensions() override { return sizeToContent ? xyAdd(getContentBoxSize(), {1,1}) : XY{ wxWidth,wxHeight }; };

    Panel* getTopmostParent() { return parent != NULL ? parent->getTopmostParent() : this; }
    bool parentFocused() { return parent != NULL && parent->focused; }
    bool thisOrParentFocused() { return parent != NULL ? parentFocused() : focused; }
    Timer64& thisOrParentFocusTimer() { return parent != NULL ? parent->focusTimer : focusTimer; }

    void renderFocusBorder(XY at, SDL_Color color, double lightup = 0.0);
    void renderFocusBorderLightup(XY at, SDL_Color c, XY size, double lightup = 0.0);
    void setDefaultOpaquePanelBackground();
    void playPanelOpenVFX();
    XY getContentBoxSize() {
        XY ret = { 0,0 };
        for (Drawable*& a : subWidgets.drawablesList) {
            XY aPos = a->position;
            XY aDim = a->getRenderDimensions();
            if (aPos.x + aDim.x > ret.x) {
                ret.x = aPos.x + aDim.x;
            }
            if (aPos.y + aDim.y > ret.y) {
                ret.y = aPos.y + aDim.y;
            }
        }
        return ret;
    }
};

