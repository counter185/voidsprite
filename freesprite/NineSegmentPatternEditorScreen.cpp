#include "NineSegmentPatternEditorScreen.h"
#include "maineditor.h"
#include "FileIO.h"
#include "Notification.h"

NineSegmentPatternEditorScreen::NineSegmentPatternEditorScreen(MainEditor* parent) {
    caller = parent;

    pointUL = { caller->canvas.dimensions.x / 3, caller->canvas.dimensions.y / 3 };
    pointUR = { caller->canvas.dimensions.x / 3 * 2, caller->canvas.dimensions.y / 3 * 2 };

    navbar = new ScreenWideNavBar<NineSegmentPatternEditorScreen*>(this,
        {
            {
                SDL_SCANCODE_F,
                {
                    "File",
                    {},
                    {
                        {SDL_SCANCODE_C, { "Close",
                                [](NineSegmentPatternEditorScreen* screen) {
                                    screen->closeNextTick = true;
                                }
                            }
                        },
                        {SDL_SCANCODE_S, { "Export to 9-segment pattern",
                                [](NineSegmentPatternEditorScreen* screen) {
                                    platformTrySaveOtherFile(screen, { {".void9sp", "9-segment pattern file"} }, "save 9-segment pattern", EVENT_9SPEDITOR_SAVE);
                                }
                            }
                        },
                    },
                    g_iconNavbarTabFile
                }
            },
        }, { SDL_SCANCODE_F });
    wxsManager.addDrawable(navbar);
}

void NineSegmentPatternEditorScreen::render()
{
    drawBackground();

    SDL_Rect canvasRenderRect = { canvasDrawOrigin.x, canvasDrawOrigin.y, caller->canvas.dimensions.x * canvasZoom, caller->canvas.dimensions.y * canvasZoom };
    for (Layer*& l : caller->layers) {
        SDL_RenderCopy(g_rd, l->tex, NULL, &canvasRenderRect);
    }
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xD0);
    SDL_Rect r2 = canvasRenderRect;
    r2.x -= 1;
    r2.y -= 1;
    r2.w += 2;
    r2.h += 2;
    SDL_RenderDrawRect(g_rd, &r2);

    XY dragHover = { -1,-1 };
    if (abs(mousePixelPos.x - pointUL.x) < 2) {
        dragHover.x = 0;
    }
    else if (abs(mousePixelPos.x - pointUR.x) < 2) {
        dragHover.x = 1;
    }
    if (abs(mousePixelPos.y - pointUL.y) < 2) {
        dragHover.y = 0;
    }
    else if (abs(mousePixelPos.y - pointUR.y) < 2) {
        dragHover.y = 1;
    }

    SDL_Color norm = {255,255,255,0x7f};
    SDL_Color drag = { 0xff, 0x6f, 0x28, 0x7f };

    //vlines
    SDL_Color c;
    c = dragHover.x == 0 ? drag : norm;
    SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, dragging.x == 0 ? 0xD0 : c.a);
    SDL_RenderDrawLine(g_rd, canvasRenderRect.x + pointUL.x * canvasZoom, canvasRenderRect.y,
        canvasRenderRect.x + pointUL.x * canvasZoom, canvasRenderRect.y + caller->canvas.dimensions.x * canvasZoom);

    c = dragHover.x == 1 ? drag : norm;
    SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, dragging.x == 1 ? 0xD0 : c.a);
    SDL_RenderDrawLine(g_rd, canvasRenderRect.x + pointUR.x * canvasZoom, canvasRenderRect.y,
        canvasRenderRect.x + pointUR.x * canvasZoom, canvasRenderRect.y + caller->canvas.dimensions.y * canvasZoom);


    //hlines
    c = dragHover.y == 0 ? drag : norm;
    SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, dragging.y == 0 ? 0xD0 : c.a);
    SDL_RenderDrawLine(g_rd, canvasRenderRect.x, canvasRenderRect.y + pointUL.y * canvasZoom,
        canvasRenderRect.x + caller->canvas.dimensions.x * canvasZoom, canvasRenderRect.y + pointUL.y * canvasZoom);

    c = dragHover.y == 1 ? drag : norm;
    SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, dragging.y == 1 ? 0xD0 : c.a);
    SDL_RenderDrawLine(g_rd, canvasRenderRect.x, canvasRenderRect.y + pointUR.y * canvasZoom,
        canvasRenderRect.x + caller->canvas.dimensions.y * canvasZoom, canvasRenderRect.y + pointUR.y * canvasZoom);

    drawBottomBar();
    BaseScreen::render();
}

void NineSegmentPatternEditorScreen::tick()
{
    if (closeNextTick) {
        g_closeScreen(this);
        return;
    }

    canvasDrawOrigin = XY{
        iclamp(-caller->canvas.dimensions.x * canvasZoom + 4, canvasDrawOrigin.x, g_windowW - 4),
        iclamp(-caller->canvas.dimensions.y * canvasZoom + 4, canvasDrawOrigin.y, g_windowH - 4)
    };
}

void NineSegmentPatternEditorScreen::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }

    LALT_TO_SUMMON_NAVBAR;

    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {
        switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                scrollingCanvas = true;
            }
            else if (evt.button.button == SDL_BUTTON_LEFT) {
                if (abs(mousePixelPos.x - pointUL.x) < 2) {
                    dragging.x = 0;
                }
                else if (abs(mousePixelPos.x - pointUR.x) < 2) {
                    dragging.x = 1;
                }
                if (abs(mousePixelPos.y - pointUL.y) < 2) {
                    dragging.y = 0;
                }
                else if (abs(mousePixelPos.y - pointUR.y) < 2) {
                    dragging.y = 1;
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                scrollingCanvas = false;
            }
            else if (evt.button.button == SDL_BUTTON_LEFT) {
                dragging = { -1,-1 };
            }
            break;
        case SDL_MOUSEMOTION:
            calcMousePixelPos({(int)(evt.motion.x), (int)(evt.motion.y)});
            if (scrollingCanvas) {
                canvasDrawOrigin = xyAdd(canvasDrawOrigin, XY{ (int)(evt.motion.xrel), (int)(evt.motion.yrel) });
            }
            if (dragging.x != -1) {
                int* targets[] = {&pointUL.x, &pointUR.x};
                *targets[dragging.x] = mousePixelPos.x;
            }
            if (dragging.y != -1) {
                int* targets[] = {&pointUL.y, &pointUR.y};
                *targets[dragging.y] = mousePixelPos.y;
            }
            break;
        case SDL_MOUSEWHEEL:
            canvasZoom += evt.wheel.y;
            canvasZoom = ixmax(1, canvasZoom);
            break;
        }
    }
}

BaseScreen* NineSegmentPatternEditorScreen::isSubscreenOf()
{
    return caller;
}

void NineSegmentPatternEditorScreen::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex)
{
    if (evt_id == EVENT_9SPEDITOR_SAVE) {
        Layer* l = caller->flattenImage();
        if (l != NULL) {
            if (write9SegmentPattern(name, l, pointUL, xySubtract(caller->canvas.dimensions, pointUR))) {
                g_addNotification(SuccessNotification("Success", "9-segment pattern saved"));
            }
            else {
                g_addNotification(ErrorNotification("Error", "Failed to export pattern"));
            }
            delete l;
        }
        else {
            g_addNotification(ErrorNotification("Error", "Failed to flatten image"));
        }
    }
}

void NineSegmentPatternEditorScreen::drawBackground()
{
    if (g_config.animatedBackground) {
        uint64_t now = g_config.animatedBackground >= 3 ? 0 : SDL_GetTicks64();
        uint64_t progress = now % 120000;
        for (int y = -(1.0 - progress / 120000.0) * g_windowH; y < g_windowH; y += 50) {
            if (y >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x22);
                SDL_RenderDrawLine(g_rd, 0, y, g_windowW, y);
            }
        }

        for (int x = -(1.0 - (now % 100000) / 100000.0) * g_windowW; x < g_windowW; x += 30) {
            if (x >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x19);
                SDL_RenderDrawLine(g_rd, x, 0, x, g_windowH);
            }
        }
    }
}

void NineSegmentPatternEditorScreen::calcMousePixelPos(XY onScreenPos)
{
    XY posInCanvas = xySubtract(onScreenPos, canvasDrawOrigin);
    mousePixelPos = XY{ posInCanvas.x / canvasZoom, posInCanvas.y / canvasZoom };
}
