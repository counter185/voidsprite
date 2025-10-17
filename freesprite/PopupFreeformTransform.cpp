#include "PopupFreeformTransform.h"
#include "maineditor.h"
#include "Layer.h"
#include "UIButton.h"

PopupFreeformTransform::PopupFreeformTransform(MainEditor* caller, Layer* target)
{
    this->caller = caller;
    this->target = target;
    targetPasteRect = { 0,0, target->w, target->h };
    setSize({ 400, 200 });

    makeTitleAndDesc(TL("vsp.freeformtransform.title"));

    actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](UIButton*) {
        closePopup();
    };
    actionButton(TL("vsp.cmn.confirm"))->onClickCallback = [this](UIButton*) {
        paste();
        closePopup();
    };
}

PopupFreeformTransform::~PopupFreeformTransform()
{
    if (target != NULL) {
        delete target;
    }
}

void PopupFreeformTransform::render()
{
    SDL_Rect renderTargetRect = getRenderTargetRect();
    target->render(renderTargetRect);
    SDL_Rect rtr1 = offsetRect(renderTargetRect, 1);
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
    SDL_RenderDrawRect(g_rd, &rtr1);

    if (draggingCorner != -1) {
        SDL_Rect dragRect = getCallerCanvas().canvasRectToScreenRect(evalPasteRectChange());
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
        SDL_RenderDrawRect(g_rd, &dragRect);
    }

    auto hoverPoint = getMouseOverDraggablePoint();
    if (hoverPoint.first != -1) {
        XY p = hoverPoint.second;
        SDL_Rect selectRect = offsetRect({ p.x,p.y,1,1 }, 8);
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
        SDL_RenderDrawRect(g_rd, &selectRect);
    }

    BasePopup::render();
}

void PopupFreeformTransform::defaultInputAction(SDL_Event evt)
{
    bool handledMouseEvent = false;
    if (evt.type == SDL_EVENT_KEY_DOWN) {
        if (evt.key.scancode == SDL_SCANCODE_RETURN) {
            handledMouseEvent = true;
            paste();
            closePopup();
        }
        else if (evt.key.scancode == SDL_SCANCODE_ESCAPE) {
            handledMouseEvent = true;
            closePopup();
        }
    }
    else if (evt.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (evt.button.button == SDL_BUTTON_LEFT) {
            if (draggingCorner != -1) {
                handledMouseEvent = true;
                targetPasteRect = evalPasteRectChange();
                draggingCorner = -1;
            }
        }
    }
    else if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (evt.button.button == SDL_BUTTON_LEFT) {
            auto whichPoint = getMouseOverDraggablePoint();
            if (whichPoint.first != -1) {
                handledMouseEvent = true;
                draggingCorner = whichPoint.first;
                dragStart = getCallerCanvas().screenPointToCanvasPoint({ g_mouseX, g_mouseY });
            }
        }
    }

    if (!handledMouseEvent) {
        getCallerCanvas().takeInput(evt);
    }
}

SDL_Rect PopupFreeformTransform::evalPasteRectChange()
{
    SDL_Rect pasteRect = targetPasteRect;
    XY dragPoint = getCallerCanvas().screenPointToCanvasPoint({ g_mouseX, g_mouseY });
    XY diff = xySubtract(dragStart, dragPoint);
    if (draggingCorner == 4) {	//drag
        pasteRect.x -= diff.x;
        pasteRect.y -= diff.y;
    }
    else {	//resize
        switch (draggingCorner) {
        case 0:	//top left
            pasteRect.x -= diff.x;
            pasteRect.y -= diff.y;
            pasteRect.w += diff.x;
            pasteRect.h += diff.y;
            break;
        case 1:	//top right
            pasteRect.y -= diff.y;
            pasteRect.w -= diff.x;
            pasteRect.h += diff.y;
            break;
        case 2:	//bottom left
            pasteRect.x -= diff.x;
            pasteRect.w += diff.x;
            pasteRect.h -= diff.y;
            break;
        case 3:	//bottom right
            pasteRect.w -= diff.x;
            pasteRect.h -= diff.y;
            break;
        }
        
    }
    return pasteRect;
}

void PopupFreeformTransform::renderDefaultBackground() {
    Fill::Gradient(0xFF000000, 0x00000000, 0x00000000, 0x00000000).fill({ 0,0,g_windowW / 3*2, g_windowH / 3*2 });
    renderPopupWindow();
}

std::pair<int, XY> PopupFreeformTransform::getMouseOverDraggablePoint()
{
    SDL_Rect rtr = getRenderTargetRect();
    std::vector<std::pair<int, XY>> points;
    const int maxDistance = 30;
    points.push_back({ 0, {rtr.x, rtr.y} });
    points.push_back({ 1, {rtr.x + rtr.w, rtr.y} });
    points.push_back({ 2, {rtr.x, rtr.y + rtr.h} });
    points.push_back({ 3, {rtr.x + rtr.w, rtr.y + rtr.h} });

    for (auto& [id, pos] : points) {
        if (xyDistance({ g_mouseX, g_mouseY }, pos) < maxDistance) {
            return {id, pos};
        }
    }
    if (pointInBox({ g_mouseX, g_mouseY }, rtr)) {
        return { 4, {rtr.x + rtr.w / 2, rtr.y + rtr.h / 2} };
    }
    return { -1,{-1,-1} };
}

SDL_Rect PopupFreeformTransform::getRenderTargetRect()
{
    Canvas& c = getCallerCanvas();
    return c.canvasRectToScreenRect(targetPasteRect);
}

Canvas& PopupFreeformTransform::getCallerCanvas() { return caller->canvas; }

void PopupFreeformTransform::paste()
{
    Layer* pasteSrc;
    if (!xyEqual({ target->w, target->h }, { targetPasteRect.w, targetPasteRect.h })) {
        pasteSrc = target->copyCurrentVariantScaled({ targetPasteRect.w, targetPasteRect.h });
    }
    else {
        pasteSrc = target->copyCurrentVariant();
    }
    Layer* pasteTargetLayer = caller->getCurrentLayer();
    caller->commitStateToLayer(pasteTargetLayer);
    pasteTargetLayer->blit(pasteSrc, { targetPasteRect.x, targetPasteRect.y });
    delete pasteSrc;
}
