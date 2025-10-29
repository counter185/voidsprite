#include "PanelMCBlockPreview.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UICheckbox.h"
#include "MinecraftBlockPreviewScreen.h"

PanelMCBlockPreview::PanelMCBlockPreview(MinecraftBlockPreviewScreen* caller, bool small)
{
    this->caller = caller;
    this->small = small;
    wxWidth = small ? 180 : 320;
    wxHeight = small ? 200 : 470;

    setupDraggable();
    if (small) {
        setupCollapsible();
    }
    addTitleText("Cube");
    

    if (!small) {
        wxsTarget().addDrawable(new UILabel("Angle", XY{ 15, 40 }));


        wxsTarget().addDrawable(new UILabel("\xCE\xB1", XY{ 80, 40 }));
        UISlider* isomSlider = new UISlider();
        isomSlider->position = { 100, 40 };
        isomSlider->wxWidth = 200;
        isomSlider->wxHeight = 25;
        isomSlider->setValue(0, 90, caller->isomAlpha);
        isomSlider->onChangeValueCallback = [caller](UISlider* s, double) {
            caller->isomAlpha = s->getValue(0, 90);
        };
        wxsTarget().addDrawable(isomSlider);


        wxsTarget().addDrawable(new UILabel("\xCE\xB2", XY{ 80, 80 }));
        UISlider* betaSlider = new UISlider();
        betaSlider->position = { 100, 80 };
        betaSlider->wxWidth = 200;
        betaSlider->wxHeight = 25;
        betaSlider->setValue(0, 90, caller->isomBeta);
        betaSlider->onChangeValueCallback = [caller](UISlider* s, double) {
            caller->isomBeta = s->getValue(0, 90);
        };
        wxsTarget().addDrawable(betaSlider);

        UICheckbox* shadeCheckbox = new UICheckbox("Shade", &caller->shadeSides);
        shadeCheckbox->position = { 15, 120 };
        wxsTarget().addDrawable(shadeCheckbox);
    }
}

void PanelMCBlockPreview::renderAfterBG(XY position)
{
    int pad = small ? 5 : 10;
    int size = wxWidth - pad*2;

    SDL_Rect blockRenderBounds = {
        position.x + pad, position.y + wxHeight - size - pad,
        size, size
    };

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x40);
    SDL_RenderDrawRect(g_rd, &blockRenderBounds);

    caller->drawIsometricBlockV2(blockRenderBounds);
}

