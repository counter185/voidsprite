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

    /// <summary>
    /// Direct parent that should take over its focused state
    /// </summary>
    Panel* parent = NULL;
    bool passThroughMouse = false;

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
    void mouseWheelEvent(XY mousePos, XY gPosOffset, XY direction) override;
    bool isPanel() override { return true; }

    bool takesMouseWheelEvents() override { return true; }

    XY getDimensions() override { return XY{ wxWidth,wxHeight }; };

    bool parentFocused() { return parent != NULL && parent->focused; }
    bool thisOrParentFocused() { return parent != NULL ? parentFocused() : focused; }
    Timer64& thisOrParentFocusTimer() { return parent != NULL ? parent->focusTimer : focusTimer; }
};

