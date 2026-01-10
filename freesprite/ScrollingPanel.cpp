#include "ScrollingPanel.h"

void ScrollingPanel::render(XY at) 
{
    if (!enabled) {
        return;
    }

    lastPosOnScreen = at;

    updateBounds();

    SDL_Rect r = SDL_Rect{ at.x, at.y, wxWidth, wxHeight };
    bgColor.fill(r);

    SDL_Color borderColor = uint32ToSDLColor(innerBorderColor);
    SDL_SetRenderDrawColor(g_rd, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_Rect r2 = offsetRect(r, 1);
    SDL_RenderDrawRect(g_rd, &r2);

    //DEBUG: show bounds
#if _DEBUG
    if (g_debugConfig.debugShowScrollPanelBounds) {
        XY endpoint = getContentBoxSize();
        SDL_SetRenderDrawColor(g_rd, 0xff, 0, 0, 0x80);
        SDL_Rect r2 = { at.x + scrollOffset.x, at.y + scrollOffset.y, endpoint.x, endpoint.y };
        SDL_RenderDrawRect(g_rd, &r2);
    }
#endif

    if (clipElementsToSize) {
        g_pushClip(r);
    }
    subWidgets.renderAll(xyAdd(at, scrollOffset));

    renderVerticalScrollbar(at);

    if (clipElementsToSize) {
        g_popClip();
    }
    //tabs[openTab].wxs.renderAll(xyAdd(position, XY{ 0, buttonsHeight }));
}

void ScrollingPanel::handleInput(SDL_Event evt, XY gPosOffset) {
    if (!enabled) {
        return;
    }

    DrawableManager::processHoverEventInMultiple({ subWidgets }, evt, xyAdd(gPosOffset, scrollOffset));

    SDL_Event cevt = convertTouchToMouseEvent(evt);
    if (cevt.type == SDL_EVENT_MOUSE_BUTTON_DOWN && cevt.button.button == SDL_BUTTON_LEFT && mouseInVerticalScrollBar()) {
        draggingVerticalScrollbar = true;
    }
    else if (draggingVerticalScrollbar) {
        if (cevt.type == SDL_EVENT_MOUSE_MOTION) {
            double yDelta = cevt.motion.yrel * getVerticalScrollBarPixelScale();
            scrollOffset.y -= yDelta;
        } else if (cevt.type == SDL_EVENT_MOUSE_BUTTON_UP && cevt.button.button == SDL_BUTTON_LEFT) {
            draggingVerticalScrollbar = false;
        }
    } else {

        if (evt.type == SDL_EVENT_FINGER_MOTION) {
            XY motionPos = { (int)(evt.tfinger.x * g_windowW), (int)(evt.tfinger.y * g_windowH) };
            XY motionDir = { (int)(evt.tfinger.dx * g_windowW), (int)(evt.tfinger.dy * g_windowH) };
            if (pointInBox(motionPos, { gPosOffset.x, gPosOffset.y, wxWidth, wxHeight })) {
                if (scrollVertically) {
                    scrollOffset.y += motionDir.y;
                }
                if (scrollHorizontally) {
                    scrollOffset.x += motionDir.x;
                }
            }
        }

        if (!DrawableManager::processInputEventInMultiple({ subWidgets }, evt, xyAdd(gPosOffset, scrollOffset))) {
            if (evt.type == SDL_EVENT_KEY_DOWN) {
                switch (evt.key.key) {
                    case SDLK_DOWN:
                        if (scrollVertically) {
                            scrollOffset.y -= wxHeight / 4;
                        }
                        break;
                    case SDLK_UP:
                        if (scrollVertically) {
                            scrollOffset.y += wxHeight / 4;
                        }
                        break;
                    case SDLK_PAGEDOWN:
                        if (scrollVertically) {
                            scrollOffset.y -= wxHeight/4*3;
                        }
                        else if (scrollHorizontally) {
                            scrollOffset.x -= wxWidth / 4 * 3;
                        }
                        break;
                    case SDLK_PAGEUP:
                        if (scrollVertically) {
                            scrollOffset.y += wxHeight/4*3;
                        }
                        else if (scrollHorizontally) {
                            scrollOffset.x += wxWidth / 4 * 3;
                        }
                        break;
                }
            }
        }
    }
}

bool ScrollingPanel::mouseInVerticalScrollBar() {
    if (scrollVertically) {
        XY localMousePos = xySubtract({g_mouseX, g_mouseY}, lastPosOnScreen);
        return pointInBox(localMousePos, {wxWidth-scrollbarThickness,0,scrollbarThickness, wxHeight});
    }
    return false;
}

std::pair<XY,XY> ScrollingPanel::getVerticalScrollBarFromToPos() {
    XY area = getContentBoxSize();
    if (scrollVertically && area.y > wxHeight) {
        int lineH = (int)(ixpow(wxHeight, 2) / (double)area.y);
        XY scrollbarOrigin = { wxWidth - 2,  (int)((-scrollOffset.y / (double)area.y) * wxHeight) };
        return {scrollbarOrigin, xyAdd(scrollbarOrigin, {0,lineH})};
    }
    return {{},{}};
}

double ScrollingPanel::getVerticalScrollBarPixelScale() {
    XY area = getContentBoxSize();
    return area.y / (double)wxHeight;
}

void ScrollingPanel::renderVerticalScrollbar(XY at) {
    XY area = getContentBoxSize();
    if (scrollVertically) {
        auto scrollbarPositions = getVerticalScrollBarFromToPos();
        if (!xyEqual(scrollbarPositions.first, scrollbarPositions.second)) {
            bool mouseInScrollbar = mouseInVerticalScrollBar() || draggingVerticalScrollbar;

            if (mouseInScrollbar) {
                verticalScrollbarHoverTimer.startIfNotStarted();
            } else {
                verticalScrollbarHoverTimer.stop();
            }

            int height = scrollbarPositions.second.y - scrollbarPositions.first.y;
            int thickness = 1 + scrollbarThickness * 
                (mouseInScrollbar ? XM1PW3P1(verticalScrollbarHoverTimer.percentElapsedTime(200)) : 0);

            if (mouseInScrollbar) {
                SDL_Rect wholeVScrollbarAreaRect = {
                    at.x + wxWidth - thickness,
                    at.y,
                    thickness,
                    wxHeight
                };
                Fill::Solid(0xa0000000).fill(wholeVScrollbarAreaRect);

                //border line
                SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x40);
                XY p1 = {wxWidth - scrollbarThickness - 4, 0};
                XY p2 = xyAdd(p1, {0, wxHeight});
                drawLine(xyAdd(at,p1), xyAdd(at,p2), XM1PW3P1(verticalScrollbarHoverTimer.percentElapsedTime(800)));
            }

            XY handleOrigin = xySubtract(scrollbarPositions.first, {thickness, 0});
            SDL_Rect handleRect = {at.x + handleOrigin.x, at.y + handleOrigin.y, thickness, height};
            Fill::Solid(0xFFFFFFFF).fill(handleRect);
        }
    }
}