#pragma once
#include "drawable.h"
#include "EventCallbackListener.h"
#include "Panel.h"

class ScrollingPanel :
    public Panel, public EventCallbackListener
{
protected:
    XY lastPosOnScreen{};

    Timer64 verticalScrollbarHoverTimer;
    bool draggingVerticalScrollbar = false;

    Timer64 horizontalScrollbarHoverTimer;
    bool draggingHorizontalScrollbar = false;
public:
    int scrollbarThickness = 14;

    bool scrollVertically = true;
    bool scrollHorizontally = true;
    XY scrollOffset = XY{ 0,0 };
    Fill bgColor = Fill::Solid(0xe0000000);
    u32 innerBorderColor = 0x20FFFFFF;
    bool clipElementsToSize = true;

    ScrollingPanel() {
        wxWidth = 200;
        wxHeight = 200;
    }

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return enabled && pointInBox(mousePos, { thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
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

    bool takesMouseWheelEvents() override { return true; }
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

    void scrollToBottom() {
        XY insideArea = getContentBoxSize();
        if (insideArea.y > wxHeight) {
            scrollOffset.y = wxHeight - insideArea.y;
        }
        else {
            scrollOffset.y = 0;
        }
    }

    bool mouseInVerticalScrollBar();
    std::pair<XY,XY> getVerticalScrollBarFromToPos();
    double getVerticalScrollBarPixelScale();

    bool mouseInHorizontalScrollBar();
    std::pair<XY,XY> getHorizontalScrollBarFromToPos();
    double getHorizontalScrollBarPixelScale();

    void renderVerticalScrollbar(XY at);
    void renderHorizontalScrollbar(XY at);

};

