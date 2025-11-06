#include "UIDropdown.h"
#include "FontRenderer.h"
#include "UIButton.h"
#include "PopupContextMenu.h"

UIDropdown::UIDropdown(std::vector<std::string> items)
{
    this->items = items;
}

UIDropdown::UIDropdown(std::vector<std::pair<std::string, std::string>> items)
{
    this->items.resize(items.size());
    this->tooltips.resize(items.size());
    std::transform(items.begin(), items.end(), this->items.begin(), [](std::pair<std::string, std::string> p) { return p.first; });
    std::transform(items.begin(), items.end(), this->tooltips.begin(), [](std::pair<std::string, std::string> p) { return p.second; });
}

void UIDropdown::render(XY pos)
{
    lastPosOnScreen = pos;

    SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
    SDL_Color bgColor = focused ? colorBGFocused : colorBGUnfocused;
    SDL_Color textColor = focused ? colorTextFocused : colorTextUnfocused;
    SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(g_rd, &drawrect);
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    SDL_RenderDrawRect(g_rd, &drawrect);

    if (lastClick.started) {
        double lineAnimPercent = 1.0 - XM1PW3P1(lastClick.percentElapsedTime(200));
        if (lineAnimPercent > 0) {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
            drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);
            drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
            drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
            drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);
        }
    }

    int textX = pos.x + 2;
    if (icon != NULL) {
        SDL_Rect iconRect = SDL_Rect{ pos.x + 1, pos.y + 1, wxHeight - 2, wxHeight - 2 };
        SDL_RenderCopy(g_rd, icon, NULL, &iconRect);
        textX += iconRect.w;
    }

    g_fnt->RenderString(text, textX, pos.y + 2, textColor);
    renderDropdownIcon(pos);
}

void UIDropdown::handleInput(SDL_Event evt, XY gPosOffset)
{
    SDL_Event cevt = convertTouchToMouseEvent(evt);

    if (cevt.type == SDL_MOUSEBUTTONDOWN) {
        XY mousePos = xySubtract(XY{ (int)(cevt.motion.x), (int)(cevt.motion.y) }, gPosOffset);
        if (cevt.button.button == 1 && cevt.button.down) {
            if (pointInBox(mousePos, SDL_Rect{ 0,0,wxWidth,wxHeight })) {
                click();
            }
        }
    }
    else if (evt.type == SDL_KEYDOWN) {
        if (evt.key.scancode == SDL_SCANCODE_ESCAPE) {
            isOpen = false;
            openTimer.start();
        }
        else if (evt.key.scancode == SDL_SCANCODE_SPACE) {
            click();
        }
    }
}

void UIDropdown::eventButtonPressed(int evt_id)
{
    if (setTextToSelectedItem) {
        text = items[evt_id];
    }
    isOpen = false;
    if (onDropdownItemSelectedCallback != NULL) {
        onDropdownItemSelectedCallback(this, evt_id, items[evt_id]);
    }
    else if (callback != NULL) {
        callback->eventDropdownItemSelected(callback_id, evt_id, items[evt_id]);
    }
}

void UIDropdown::renderDropdownIcon(XY pos)
{
    int iconW = 20;
    int iconPad = 5;
    XY origin = { pos.x + wxWidth - iconW/2 - iconPad, pos.y + wxHeight / 2 };

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    drawLine(xyAdd(origin, { -iconW / 2 - iconPad, -wxHeight / 3 }), xyAdd(origin, { -iconW / 2 - iconPad, wxHeight / 3 }));

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0xff);

    XY cbPoint = xyAdd(origin, { 0, wxHeight / 6 });
    XY ctPoint = xyAdd(origin, { 0, -wxHeight / 6 });

    double animTime = XM1PW3P1(openTimer.started ? openTimer.percentElapsedTime(300) : 1.0f);

    if (!isOpen) {
        XY bPoint = statLineEndpoint(ctPoint, cbPoint, animTime);
        drawLine(bPoint, xyAdd(origin, { -iconW / 2, -wxHeight / 6 }), animTime);
        drawLine(bPoint, xyAdd(origin, { iconW / 2, -wxHeight / 6 }), animTime);
    }
    else {
        XY bPoint = statLineEndpoint(cbPoint, ctPoint, animTime);
        drawLine(xyAdd(origin, { -iconW / 2, wxHeight / 6 }), bPoint, animTime);
        drawLine(xyAdd(origin, { iconW / 2, wxHeight / 6 }), bPoint, animTime);
    }
}

std::vector<UIButton*> UIDropdown::genButtonsList(UIButton* (*customButtonGenFunction)(std::string name, std::string item))
{
    std::vector<UIButton*> ret;

    for (int y = 0; y < items.size(); y++) {
        if (customButtonGenFunction == NULL) {
            UIButton* btn = new UIButton();
            btn->text = items[y];
            btn->wxWidth = wxWidth;
            btn->wxHeight = wxHeight;
            btn->fill = colorBGFocused;
            btn->position = { 0, wxHeight + y * btn->wxHeight };
            btn->tooltip = tooltips.size() > y ? tooltips[y] : "";
            btn->onClickCallback = [this, y](UIButton* b) {
                this->eventButtonPressed(y);
			};
            ret.push_back(btn);
        }
        else {
            UIButton* btn = customButtonGenFunction(items[y], items[y]);
            btn->position = { 0, wxHeight + y * btn->wxHeight };
            btn->onClickCallback = [this, y](UIButton* b) {
                this->eventButtonPressed(y);
            };
            ret.push_back(btn);
        }
    }

    return ret;
}

void UIDropdown::click()
{
    lastClick.start();
    openTimer.start();
    /*isOpen = !isOpen;
    if (isOpen) {
        wxs.forceUnfocus();
    }*/
    auto ctxmenu = new PopupContextMenu(genButtonsList());
    ctxmenu->scrollable = true;
    ctxmenu->setContextMenuOrigin(xyAdd(lastPosOnScreen, XY{ 0, wxHeight }));
    ctxmenu->onExitCallback = [this](PopupContextMenu* p) {
        this->isOpen = false;
        openTimer.start();
    };
    isOpen = true;
    g_addPopup(ctxmenu);
}
