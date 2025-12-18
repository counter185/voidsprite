#include "globals.h"
#include "mathops.h"
#include "UISVPicker.h"
#include "EditorColorPicker.h"
#include "FontRenderer.h"

void UISVPicker::drawPosIndicator(XY origin)
{
    //g_fnt->RenderString(std::string("vpos:") + std::to_string(vPos), origin.x + 10, origin.y + wxHeight / 4);

    SDL_Color colorNow = rgb2sdlcolor(hsv2rgb(hsv{ parent->currentH, parent->currentS, parent->currentV }));
    SDL_Color colorDesat = rgb2sdlcolor(hsv2rgb(hsv{ parent->currentH, parent->currentS /6, dxmin(1.0, parent->currentV + 0.2) }));

    XY centerPoint = xyAdd(origin, XY{ (int)(wxWidth * sPos), (int)(wxHeight * (1.0f - vPos))});
    int xdist = 8;
    SDL_Rect rectAround = SDL_Rect{ centerPoint.x - xdist, centerPoint.y - xdist, xdist * 2, xdist * 2 };
    SDL_SetRenderDrawColor(g_rd, colorNow.r, colorNow.g, colorNow.b, 0xff);
    SDL_RenderDrawRect(g_rd, &rectAround);

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x90);
    SDL_Rect rectAround2 = rectAround;
    rectAround2.x -= 1;
    rectAround2.y -= 1;
    rectAround2.w += 2;
    rectAround2.h += 2;
    SDL_RenderDrawRect(g_rd, &rectAround2);
    SDL_RenderPoint(g_rd, centerPoint.x, centerPoint.y);

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x90);
    rectAround2 = rectAround;
    rectAround2.x += 1;
    rectAround2.y += 1;
    rectAround2.w -= 2;
    rectAround2.h -= 2;
    SDL_RenderDrawRect(g_rd, &rectAround2);
    SDL_RenderPoint(g_rd, centerPoint.x - 1, centerPoint.y);
}

void UISVPicker::render(XY pos)
{	
    u32 c = sdlcolorToUint32(rgb2sdlcolor(hsv2rgb(hsv{ parent->currentH, 1.0, 1.0 })));

    SDL_Rect pickerRect = { pos.x, pos.y, wxWidth, wxHeight };
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
    SDL_RenderFillRect(g_rd, &pickerRect);
    renderGradient(pickerRect, modAlpha(c, 0), modAlpha(c, 255), modAlpha(c,0), modAlpha(c,255));
    renderGradient(pickerRect, 0x00000000, 0x00000000, 0xFF000000, 0xFF000000);

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 80);
    SDL_RenderDrawRect(g_rd, &pickerRect);

    drawPosIndicator(pos);
}

void UISVPicker::handleInput(SDL_Event evt, XY gPosOffset)
{
    switch (evt.type) {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        if (evt.button.button == 1) {
            mouseHeld = evt.button.down;
        }
        break;
    case SDL_MOUSEMOTION:
        if (mouseHeld) {
            XY mousePos = xySubtract(XY{ (int)(evt.motion.x), (int)(evt.motion.y) }, gPosOffset);
            sPos = fclamp(0.0f, mousePos.x / (float)wxWidth, 1.0f);
            vPos = 1.0f - fclamp(0.0f, mousePos.y / (float)wxHeight, 1.0f);
            this->onSVValueChanged();
        }
        break;
    }
}

void UISVPicker::onSVValueChanged()
{
    parent->editorColorSVPickerChanged(sPos, vPos);
}
