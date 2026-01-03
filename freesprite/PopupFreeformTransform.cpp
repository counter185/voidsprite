#include "PopupFreeformTransform.h"
#include "maineditor.h"
#include "Layer.h"
#include "UIButton.h"
#include "UITextField.h"
#include "UIStackPanel.h"
#include "UILabel.h"

PopupFreeformTransform::PopupFreeformTransform(MainEditor* caller, Layer* target)
{
    this->caller = caller;
    this->target = target;
    targetPasteRect = { 0,0, target->w, target->h };
    setSize({ 400, 200 });

    UITextField* txw = new UITextField(std::to_string(target->w));
    txw->isNumericField = true;
    txw->wxWidth = 120;
    txw->onTextChangedCallback = [this](UITextField* t, std::string text) {
        try {
            int v = std::stoi(text);
            targetPasteRect.w = v;
        }
        catch (std::exception&) {}
    };

    UITextField* txh = new UITextField(std::to_string(target->h));
    txh->isNumericField = true;
    txh->wxWidth = 120;
    txh->onTextChangedCallback = [this](UITextField* t, std::string text) {
        try {
            int v = std::stoi(text);
            targetPasteRect.h = v;
        }
        catch (std::exception&) {}
        };

    UIStackPanel* sp = UIStackPanel::Horizontal(12, {
        new UILabel("Size"),
        txw, txh
    });
    sp->position = { 10, 50 };
    wxsManager.addDrawable(sp);

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
    if (draggingCorner == 8) {	//drag
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
            case 4: //center top
                pasteRect.y -= diff.y;
                pasteRect.h += diff.y;
                break;
            case 5: //center right
                pasteRect.w -= diff.x;
                break;
            case 6: //center left
                pasteRect.x -= diff.x;
                pasteRect.w += diff.x;
                break;
            case 7: //center bottom 
                pasteRect.h -= diff.y;
                break;

        }
        
    }
    return pasteRect;
}

void PopupFreeformTransform::renderDefaultBackground() {
    Fill::Gradient(0xFF000000, 0x00000000, 0x00000000, 0x00000000).fill({ 0,0,g_windowW / 3*2, g_windowH / 3*2 });
}

std::pair<int, XY> PopupFreeformTransform::getMouseOverDraggablePoint()
{
    SDL_Rect rtr = getRenderTargetRect();
    std::vector<std::pair<int, XY>> points;
    const int maxDistance = 30;
    points.push_back({ 0, {rtr.x, rtr.y} });    //UL
    points.push_back({ 1, {rtr.x + rtr.w, rtr.y} });    //UR
    points.push_back({ 2, {rtr.x, rtr.y + rtr.h} });    //DL
    points.push_back({ 3, {rtr.x + rtr.w, rtr.y + rtr.h} });    //DR

    points.push_back({ 4, {rtr.x + rtr.w / 2, rtr.y} });    //center top
    points.push_back({ 5, {rtr.x + rtr.w, rtr.y + rtr.h / 2} });    //center right
    points.push_back({ 6, {rtr.x, rtr.y + rtr.h / 2} });    //center left
    points.push_back({ 7, {rtr.x + rtr.w / 2, rtr.y + rtr.h} });    //center bottom

    for (auto& [id, pos] : points) {
        if (xyDistance({ g_mouseX, g_mouseY }, pos) < maxDistance) {
            return {id, pos};
        }
    }
    if (pointInBox({ g_mouseX, g_mouseY }, rtr)) {
        return { 8, {rtr.x + rtr.w / 2, rtr.y + rtr.h / 2} };
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
    if (!xyEqual({ target->w, target->h }, { abs(targetPasteRect.w), abs(targetPasteRect.h) })) {
        pasteSrc = target->copyCurrentVariantScaled({ abs(targetPasteRect.w), abs(targetPasteRect.h) });
    }
    else {
        pasteSrc = target->copyCurrentVariant();
    }

    if (targetPasteRect.w < 0) {
        pasteSrc->flipHorizontally();
        targetPasteRect.x += targetPasteRect.w;
        targetPasteRect.w = -targetPasteRect.w;
    }
    if (targetPasteRect.h < 0) {
        pasteSrc->flipVertically();
        targetPasteRect.y += targetPasteRect.h;
        targetPasteRect.h = -targetPasteRect.h;
    }

    Layer* pasteTargetLayer = caller->getCurrentLayer();
    caller->commitStateToLayer(pasteTargetLayer);
    pasteTargetLayer->blit(pasteSrc, { targetPasteRect.x, targetPasteRect.y });
    delete pasteSrc;
}
