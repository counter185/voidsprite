#pragma once
#include "drawable.h"
#include "EventCallbackListener.h"
#include "Panel.h"

class ScrollingView :
    public Panel, public EventCallbackListener
{
public:
    bool scrollVertically = true;
    bool scrollHorizontally = true;
    XY scrollOffset = XY{ 0,0 };
    int wxWidth = 200;
    int wxHeight = 200;
    SDL_Color bgColor = { 0,0,0,0xe0 };

    ScrollingView() {
    }

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return pointInBox(mousePos, { thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });//|| tabButtons.mouseInAny(xyAdd(thisPositionOnScreen, scrollOffset), mousePos);// || tabs[openTab].wxs.mouseInAny(xyAdd(XY{ 0, buttonsHeight }, thisPositionOnScreen), mousePos);
    }

    void render(XY position) override {
        updateBounds();

        SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
        SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(g_rd, &r);

        //DEBUG: show bounds
        /*XY endpoint = getInsideAreaWH();
        SDL_SetRenderDrawColor(g_rd, 0xff, 0, 0, 0x80);
        SDL_Rect r2 = { position.x + scrollOffset.x, position.y + scrollOffset.y, endpoint.x, endpoint.y };
        SDL_RenderDrawRect(g_rd, &r2);*/

        g_pushClip(r);
        subWidgets.renderAll(xyAdd(position, scrollOffset));
        g_popClip();
        //tabs[openTab].wxs.renderAll(xyAdd(position, XY{ 0, buttonsHeight }));
    }
    void handleInput(SDL_Event evt, XY gPosOffset) override {
        DrawableManager::processHoverEventInMultiple({ subWidgets }, evt, xyAdd(gPosOffset, scrollOffset));

        if (!DrawableManager::processInputEventInMultiple({subWidgets}, evt, xyAdd(gPosOffset, scrollOffset))) {
            if (evt.type == SDL_MOUSEWHEEL) {
                if (scrollVertically) {
                    scrollOffset.y += evt.wheel.y * 20;
                }
                else if (scrollHorizontally) {
                    scrollOffset.x += evt.wheel.y * 20;
                }
            }
        }
    }

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
            XY aDim = a->getDimensions();
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

