#include "ScrollingPanel.h"

void ScrollingPanel::render(XY position) {
    updateBounds();

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    bgColor.fill(r);

    //DEBUG: show bounds
#if _DEBUG
    if (g_debugConfig.debugShowScrollPanelBounds) {
        XY endpoint = getInsideAreaWH();
        SDL_SetRenderDrawColor(g_rd, 0xff, 0, 0, 0x80);
        SDL_Rect r2 = { position.x + scrollOffset.x, position.y + scrollOffset.y, endpoint.x, endpoint.y };
        SDL_RenderDrawRect(g_rd, &r2);
    }
#endif

    if (clipElementsToSize) {
        g_pushClip(r);
    }
    subWidgets.renderAll(xyAdd(position, scrollOffset));

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
    XY area = getInsideAreaWH();
    if (scrollVertically && area.y > wxHeight) {
        int lineH = (int)(ixpow(wxHeight, 2) / (double)area.y);
        XY scrollbarOrigin = { position.x + wxWidth - 3,  position.y + (int)((-scrollOffset.y / (double)area.y) * wxHeight) };
        //SDL_RenderPoint(g_rd, scrollbarOrigin.x, scrollbarOrigin.y);
        SDL_RenderLine(g_rd, scrollbarOrigin.x, scrollbarOrigin.y, scrollbarOrigin.x, scrollbarOrigin.y + lineH);
    }

    if (clipElementsToSize) {
        g_popClip();
    }
    //tabs[openTab].wxs.renderAll(xyAdd(position, XY{ 0, buttonsHeight }));
}

void ScrollingPanel::handleInput(SDL_Event evt, XY gPosOffset) {
    DrawableManager::processHoverEventInMultiple({ subWidgets }, evt, xyAdd(gPosOffset, scrollOffset));

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
            switch (evt.key.scancode) {
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
