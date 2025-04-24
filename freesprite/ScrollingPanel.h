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

    void render(XY position) override {
        updateBounds();

        SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
        bgColor.fill(r);

        //DEBUG: show bounds
        /*XY endpoint = getInsideAreaWH();
        SDL_SetRenderDrawColor(g_rd, 0xff, 0, 0, 0x80);
        SDL_Rect r2 = { position.x + scrollOffset.x, position.y + scrollOffset.y, endpoint.x, endpoint.y };
        SDL_RenderDrawRect(g_rd, &r2);*/

        if (clipElementsToSize) {
            g_pushClip(r);
        }
        subWidgets.renderAll(xyAdd(position, scrollOffset));

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
        XY area = getInsideAreaWH();
        if (scrollVertically && area.y > wxHeight) {
            int lineH = (int)(ixpow(wxHeight,2) / (double)area.y);
            XY scrollbarOrigin = { position.x + wxWidth - 3,  position.y + (int)((-scrollOffset.y / (double)area.y) * wxHeight) };
            //SDL_RenderPoint(g_rd, scrollbarOrigin.x, scrollbarOrigin.y);
            SDL_RenderLine(g_rd, scrollbarOrigin.x, scrollbarOrigin.y, scrollbarOrigin.x, scrollbarOrigin.y + lineH);
        }

        if (clipElementsToSize) {
            g_popClip();
        }
        //tabs[openTab].wxs.renderAll(xyAdd(position, XY{ 0, buttonsHeight }));
    }
    void handleInput(SDL_Event evt, XY gPosOffset) override {
        DrawableManager::processHoverEventInMultiple({ subWidgets }, evt, xyAdd(gPosOffset, scrollOffset));

        if (!DrawableManager::processInputEventInMultiple({subWidgets}, evt, xyAdd(gPosOffset, scrollOffset))) {
            if (evt.type == SDL_EVENT_FINGER_MOTION) {
                XY motionPos = {(int)(evt.tfinger.x * g_windowW), (int)(evt.tfinger.y * g_windowH)};
                XY motionDir = {(int)(evt.tfinger.dx * g_windowW), (int)(evt.tfinger.dy * g_windowH)};
                if (pointInBox(motionPos, {gPosOffset.x, gPosOffset.y, wxWidth, wxHeight})) {
                    if (scrollVertically) {
                        scrollOffset.y += motionDir.y;
                    }
                    if (scrollHorizontally) {
                        scrollOffset.x += motionDir.x;
                    }
                }
            }
        }
    }
    void mouseHoverMotion(XY mousePos, XY gPosOffset) override
    {
        if (enabled) {
            subWidgets.processHoverEvent(xyAdd(scrollOffset, xyAdd(gPosOffset, position)), mousePos);
        }
    }

    void mouseWheelEvent(XY mousePos, XY gPosOffset, XY direction) override
    {
        if (enabled) {
            if (!subWidgets.processMouseWheelEvent(xyAdd(gPosOffset, position), mousePos, direction)) {
                if (scrollVertically) {
                    scrollOffset.y += direction.y * 20;
                }
                else if (scrollHorizontally) {
                    scrollOffset.x += direction.y * 20;
                }
            }
        }
    }

    bool takesTouchEvents() override { return true; }

    void updateBounds() {
        XY insideArea = getInsideAreaWH();

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

    XY getInsideAreaWH() {
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

