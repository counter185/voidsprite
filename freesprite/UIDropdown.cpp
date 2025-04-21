#include "UIDropdown.h"
#include "FontRenderer.h"
#include "UIButton.h"

UIDropdown::UIDropdown(std::vector<std::string> items)
{
    this->items = items;
    genButtons();
}

UIDropdown::UIDropdown(std::vector<std::pair<std::string, std::string>> items)
{
    this->items.resize(items.size());
    this->tooltips.resize(items.size());
    std::transform(items.begin(), items.end(), this->items.begin(), [](std::pair<std::string, std::string> p) { return p.first; });
    std::transform(items.begin(), items.end(), this->tooltips.begin(), [](std::pair<std::string, std::string> p) { return p.second; });
    genButtons();
}

void UIDropdown::render(XY pos)
{
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

    g_fnt->RenderString(text + (focused ? "_" : ""), textX, pos.y + 2, textColor);

    if (isOpen) {
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0xa0 * openTimer.percentElapsedTime(100)));
        SDL_RenderFillRect(g_rd, &drawrect);
        renderDropdownIcon(pos);
        wxs.renderAll(xyAdd(xySubtract(pos, { 0, (int)((1.0f - openTimer.percentElapsedTime(100)) * wxHeight) }), XY{0, menuYOffset}));
    }
    else {
        renderDropdownIcon(pos);
    }
}

void UIDropdown::focusIn()
{
}

void UIDropdown::focusOut()
{
    isOpen = false;
    wxs.forceUnhover();
    wxs.forceUnfocus();
}

void UIDropdown::mouseHoverOut()
{
    wxs.forceUnhover();
}

void UIDropdown::mouseHoverMotion(XY mousePos, XY gPosOffset)
{
    if (isOpen) {
        wxs.processHoverEvent(xyAdd({0, menuYOffset}, xyAdd(gPosOffset, position)), mousePos);
    }
}

void UIDropdown::mouseWheelEvent(XY mousePos, XY gPosOffset, XY direction)
{
    if (isOpen) {
        if (!wxs.processMouseWheelEvent(xyAdd({ 0, menuYOffset }, xyAdd(gPosOffset, position)), mousePos, direction)) {
            menuYOffset += direction.y * 30;
            if (menuYOffset > 0) menuYOffset = 0;
            if (menuYOffset < -menuHeight + wxHeight) menuYOffset = -menuHeight + wxHeight;
        }
    }
}

void UIDropdown::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (isOpen && evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && DOWN(evt.button)) {
        wxs.tryFocusOnPoint(XY{ (int)evt.button.x, (int)evt.button.y }, xyAdd(gPosOffset, XY{0,menuYOffset}));
    }
    if (!isOpen || !wxs.anyFocused()) {
        if (evt.type == SDL_MOUSEBUTTONDOWN) {
            XY mousePos = xySubtract(XY{ (int)(evt.motion.x), (int)(evt.motion.y) }, gPosOffset);
            if (evt.button.button == 1 && DOWN(evt.button)) {
                if (pointInBox(mousePos, SDL_Rect{ 0,0,wxWidth,wxHeight })) {
                    click();
                }
            }
        }
        else if (evt.type == SDL_KEYDOWN) {
            if (KEYCODE(evt) == SDL_SCANCODE_ESCAPE) {
                isOpen = false;
                openTimer.start();
            }
            else if (KEYCODE(evt) == SDL_SCANCODE_SPACE) {
                click();
            }
        }
    }
    else {
        wxs.passInputToFocused(evt, xyAdd(gPosOffset, XY{ 0,menuYOffset }));
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

void UIDropdown::genButtons(UIButton* (*customButtonGenFunction)(std::string name, std::string item))
{
    wxs.freeAllDrawables();

    for (int y = 0; y < items.size(); y++) {
        if (customButtonGenFunction == NULL) {
            UIButton* btn = new UIButton();
            btn->text = items[y];
            btn->wxWidth = wxWidth;
            btn->wxHeight = wxHeight;
            btn->fill = colorBGFocused;
            btn->position = { 0, wxHeight + y * btn->wxHeight };
            btn->tooltip = tooltips.size() > y ? tooltips[y] : "";
            btn->setCallbackListener(y, this);
            wxs.addDrawable(btn);
        }
        else {
            UIButton* btn = customButtonGenFunction(items[y], items[y]);
            btn->position = { 0, wxHeight + y * btn->wxHeight };
            btn->setCallbackListener(y, this);
            wxs.addDrawable(btn);
        }
    }

    menuHeight = items.size() * wxHeight;
}

void UIDropdown::click()
{
    lastClick.start();
    openTimer.start();
    isOpen = !isOpen;
    if (isOpen) {
        wxs.forceUnfocus();
    }
    menuYOffset = 0;
}
