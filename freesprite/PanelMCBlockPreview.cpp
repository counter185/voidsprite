#include "PanelMCBlockPreview.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UICheckbox.h"
#include "MinecraftBlockPreviewScreen.h"

PanelMCBlockPreview::PanelMCBlockPreview(MinecraftBlockPreviewScreen* caller, bool small)
{
	this->caller = caller;

	wxWidth = small ? 180 : 320;
	wxHeight = small ? 190 : 420;
    borderColor = visualConfigHexU32("ui/panel/border");

	UILabel* titleLabel = new UILabel("Cube");
    titleLabel->position = {5, 2};
    subWidgets.addDrawable(titleLabel);

    if (!small) {
        UILabel* l = new UILabel("Angle");
        l->position = { 15, 40 };
        subWidgets.addDrawable(l);

        UISlider* isomSlider = new UISlider();
        isomSlider->position = { 70, 40 };
        isomSlider->wxWidth = 200;
        isomSlider->wxHeight = 30;
        isomSlider->sliderPos = (caller->isometricBlockScale - 2) / 9.0f;
        isomSlider->setCallbackListener(1, this);
        subWidgets.addDrawable(isomSlider);

        UICheckbox* shadeCheckbox = new UICheckbox("Shade", caller->shadeSides);
        shadeCheckbox->position = { 15, 80 };
        shadeCheckbox->setCallbackListener(2, this);
        subWidgets.addDrawable(shadeCheckbox);
    }
}

void PanelMCBlockPreview::render(XY position)
{
    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, focused ? 0xa0 : 0x90 };
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, focused ? 0xa0 : 0x90 };
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    int pad = 10;
    int size = wxWidth - pad*2;

    SDL_Rect blockRenderBounds = {
        position.x + pad, position.y + wxHeight - size - pad,
        size, size
    };

    caller->drawIsometricBlock(blockRenderBounds);

	DraggablePanel::render(position);
}

void PanelMCBlockPreview::eventSliderPosChanged(int evt_id, float value)
{
    if (evt_id == 1) {
        caller->isometricBlockScale = 2 + value * 9;
    }
}

void PanelMCBlockPreview::eventCheckboxToggled(int evt_id, bool newState)
{
    if (evt_id == 2) {
        caller->shadeSides = newState;
    }
}
