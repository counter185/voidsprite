#include "PanelPreview.h"
#include "maineditor.h"
#include "UIButton.h"

PanelPreview::PanelPreview(MainEditor* t)
{
    parent = t;
    wxWidth = 400;
    wxHeight = 300;

    UIButton* closeBtn = new UIButton();
    closeBtn->wxWidth = 30;
    closeBtn->wxHeight = 20;
    closeBtn->position = { wxWidth - 5 - closeBtn->wxWidth, 5 };
    closeBtn->text = "X";
    closeBtn->onClickCallback = [&](UIButton* caller) {
        parent->removeWidget(getTopmostParent());
    };
    subWidgets.addDrawable(closeBtn);
}

void PanelPreview::handleInput(SDL_Event evt, XY gPosOffset)
{
    SDL_Event evtc = convertTouchToMouseEvent(evt);
    SDL_Rect canvasDraw = getCanvasDrawRect(gPosOffset);
    if (evtc.type == SDL_EVENT_MOUSE_BUTTON_DOWN 
        && !subWidgets.mouseInAny(gPosOffset, { (int)evtc.button.x, (int)evtc.button.y }) 
        && pointInBox({ (int)evtc.button.x, (int)evtc.button.y }, canvasDraw)) {
        dragging++;
    }
    else if (dragging > 0 && evtc.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        dragging = 0;
    }
    else if (dragging && evtc.type == SDL_EVENT_MOUSE_MOTION) {
        c.panCanvas({ (int)evtc.motion.xrel, (int)evtc.motion.yrel });
    }
    else {
        Panel::handleInput(evt, gPosOffset);
    }
}

void PanelPreview::render(XY at)
{
    c.dimensions = parent->canvas.dimensions;

    if (!enabled) {
        return;
    }

    SDL_Rect panelRect = { at.x, at.y, wxWidth, wxHeight };

    renderGradient(panelRect, 0x80000000, 0x80000000, 0x80000000, 0x80404040);

    if (thisOrParentFocused()) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ at.x, at.y }, { at.x, at.y + wxHeight }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
        drawLine({ at.x, at.y }, { at.x + wxWidth, at.y }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
    }

    SDL_Rect canvasDraw = getCanvasDrawRect(at);
    g_pushClip(canvasDraw);
    c.lockToScreenBounds(0, 0, 0, 0, { canvasDraw.w, canvasDraw.h });
    SDL_Rect texDraw = c.getCanvasOnScreenRect();
    texDraw.x += at.x;
    texDraw.y += at.y;
    for (auto*& layer : parent->layers) {
        layer->render(texDraw, layer->layerAlpha);
    }

    renderViewportBound(at);

    g_popClip();

    Panel::render(at);
}

SDL_Rect PanelPreview::getCanvasDrawRect(XY at) {
    return SDL_Rect{ at.x + 5, at.y + 35, wxWidth - 10, wxHeight - 40 };
}

bool PanelPreview::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
    SDL_Rect canvasDraw = getCanvasDrawRect(thisPositionOnScreen);
    return Panel::isMouseIn(thisPositionOnScreen, mousePos) || (enabled && pointInBox(mousePos, canvasDraw));
}

void PanelPreview::renderViewportBound(XY at)
{
    SDL_Rect texDraw = c.getCanvasOnScreenRect();

	XY topLeft = parent->canvas.screenPointToCanvasPoint({ 0, 0 });
    XY bottomRight = parent->canvas.screenPointToCanvasPoint({ g_windowW, g_windowH });

	XY scaledTopLeft = c.canvasPointToScreenPoint(topLeft);
	XY scaledBottomRight = c.canvasPointToScreenPoint(bottomRight);

    SDL_Rect scaledRect = {
        scaledTopLeft.x + at.x,
        scaledTopLeft.y + at.y,
        scaledBottomRight.x - scaledTopLeft.x,
        scaledBottomRight.y - scaledTopLeft.y
    };

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0xff, 0xd0);
	SDL_RenderDrawRect(g_rd, &scaledRect);

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0xd0);
    XY scaledMousePos = c.canvasPointToScreenPoint(parent->mousePixelTargetPoint);
    SDL_Rect mousePointerRect = {
        scaledMousePos.x + at.x,
        scaledMousePos.y + at.y,
        c.scale,
        c.scale
    };
    SDL_RenderDrawRect(g_rd, &mousePointerRect);

    SDL_Color col = uint32ToSDLColor(parent->pickedColor);
    SDL_SetRenderDrawColor(g_rd, col.r, col.g, col.b, 0xd0);
    SDL_Rect coloredRect = offsetRect(mousePointerRect, 1);
    SDL_RenderDrawRect(g_rd, &coloredRect);

    for (int i = 1; i < 4; i++) {
        SDL_Rect offsRect = offsetRect(mousePointerRect, ixpow(3, i));
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0xd0 / (i + 3));
        SDL_RenderDrawRect(g_rd, &offsRect);
    }
}