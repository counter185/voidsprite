#include "PopupPickColor.h"
#include "UIColorInputField.h"
#include "UIButton.h"
#include "FontRenderer.h"
#include "UIColorPicker.h"

PopupPickColor::PopupPickColor(std::string tt, std::string tx, bool acceptAlpha) {
    this->acceptAlpha = acceptAlpha;
    wxWidth = 500;
    wxHeight = 510;

    //colorInput = new UIColorInputField();
    //colorInput->position = XY{ 30, wxHeight / 2 };
    //wxsManager.addDrawable(colorInput);

    colorPicker = new UIColorPicker();
    colorPicker->passThroughMouse = true;
    colorPicker->position = XY{ 30, wxHeight / 2 - colorPicker->wxHeight/2 };
    colorPicker->wxWidth = 200;
    wxsManager.addDrawable(colorPicker);

    if (acceptAlpha) {
        alphaInput = new UITextField();
        alphaInput->position = xyAdd(xyAdd(colorPicker->colorTextField->position, colorPicker->position), {colorPicker->colorTextField->wxWidth, 0});//XY{ 160, wxHeight / 2 };
        alphaInput->wxWidth = 50;
        alphaInput->wxHeight = 30;
        alphaInput->setCallbackListener(2, this);
        wxsManager.addDrawable(alphaInput);
        setAlpha(255);
    }

    actionButton(TL("vsp.cmn.apply"))->onClickCallback = [this](UIButton* btn) {
        if (onColorConfirmedCallback != NULL) {
            onColorConfirmedCallback(this, getColor());
        }
        else if (callback != NULL) {
            callback->eventColorSet(callback_id, getColor());
        }
        g_closePopup(this); 
    };

    actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](UIButton* btn) { g_closePopup(this); };

    makeTitleAndDesc(tt, tx);
}

void PopupPickColor::updateRGBTextBoxOnInputEvent(std::string data, uint8_t* value)
{
    try {
        int val;
        if (data.size() == 3 && data[0] == 'x') {
            val = std::stoi(data.substr(1), 0, 16);
        }
        else {
            val = std::stoi(data);
        }
        if (val >= 0 && val <= 255) {
            *value = val;
            alphaInput->setText(std::to_string(alpha));
        }
    }
    catch (std::exception&) {

    }
}

void PopupPickColor::setRGB(u32 rgb)
{
    colorPicker->colorUpdated(rgb);
}

void PopupPickColor::setAlpha(uint8_t a)
{
    if (acceptAlpha) {
        alpha = a;
        alphaInput->setText(std::to_string(alpha));
    }
}

uint32_t PopupPickColor::getColor()
{
    return (colorPicker->colorNowU32 & 0xFFFFFF) | (acceptAlpha ? (alpha << 24) : 0xFF000000);
}

void PopupPickColor::eventTextInput(int evt_id, std::string data)
{
    if (evt_id == 2) {
        updateRGBTextBoxOnInputEvent(data, &alpha);
    }
}

void PopupPickColor::render()
{
    BasePopup::render();

    XY origin = getPopupOrigin();
    const XY colorBoxSize = { 30,20 };

    SDL_Rect r = SDL_Rect{ origin.x + 8, origin.y + wxHeight - colorBoxSize.y - 8, colorBoxSize.x, colorBoxSize.y };
    SDL_Color colorNow = uint32ToSDLColor(getColor());
    SDL_SetRenderDrawColor(g_rd, colorNow.r, colorNow.g, colorNow.b, 255);
    SDL_RenderFillRect(g_rd, &r);
    r.x += r.w;
    SDL_RenderDrawRect(g_rd, &r);
    SDL_SetRenderDrawColor(g_rd, colorNow.r, colorNow.g, colorNow.b, colorNow.a);
    SDL_RenderFillRect(g_rd, &r);
}
