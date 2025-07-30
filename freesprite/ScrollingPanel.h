#pragma once
#include "drawable.h"
#include "EventCallbackListener.h"
#include "Panel.h"

class ScrollingPanel :
    public Panel, public EventCallbackListener
{
public:
    bool scrollVertically = true;
    bool scrollHorizontally = true;
    XY scrollOffset = XY{ 0,0 };
    int wxWidth = 200;
    int wxHeight = 200;
    Fill bgColor = Fill::Solid(0xe0000000);
    bool clipElementsToSize = true;

    ScrollingPanel() {
    }

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return pointInBox(mousePos, { thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });//|| tabButtons.mouseInAny(xyAdd(thisPositionOnScreen, scrollOffset), mousePos);// || tabs[openTab].wxs.mouseInAny(xyAdd(XY{ 0, buttonsHeight }, thisPositionOnScreen), mousePos);
    }

    void render(XY position) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;
    void mouseHoverMotion(XY mousePos, XY gPosOffset) override
    {
        if (enabled) {
            XY thisPositionOnScreen = xyAdd(gPosOffset, position);
            if (isMouseIn(thisPositionOnScreen, mousePos)) { 
                subWidgets.processHoverEvent(xyAdd(scrollOffset, thisPositionOnScreen), mousePos);
            }
        }
    }

    void mouseWheelEvent(XY mousePos, XY gPosOffset, XYf direction) override
    {
        if (enabled) {
            if (!subWidgets.processMouseWheelEvent(xyAdd(gPosOffset, position), mousePos, direction)) {
                if (scrollVertically) {
                    scrollOffset.y += direction.y * 20;
                }
                else if (scrollHorizontally) {
                    scrollOffset.x += direction.y * 20;
                }
                if (scrollHorizontally) {
                    scrollOffset.x += direction.x * 20;
                }
            }
        }
    }

    bool takesTouchEvents() override { return true; }

    void updateBounds() {
        XY insideArea = getContentBoxSize();

        if (scrollHorizontally) {
            if (insideArea.x > wxWidth) {
                if (scrollOffset.x < (wxWidth - insideArea.x)) {
                    scrollOffset.x = wxWidth - insideArea.x;
                }
                if (scrollOffset.x > 0) {
                    scrollOffset.x = 0;
                }
            }
            else {
                scrollOffset.x = 0;
            }
        }

        if (scrollVertically) {
            if (insideArea.y > wxHeight) {
                if (scrollOffset.y < (wxHeight - insideArea.y)) {
                    scrollOffset.y = wxHeight - insideArea.y;
                }
                if (scrollOffset.y > 0) {
                    scrollOffset.y = 0;
                }
            }
            else {
                scrollOffset.y = 0;
            }
        }

    }

};

