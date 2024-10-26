#include "PopupAdjustHSV.h"
#include "FontRenderer.h"
#include "UISlider.h"
#include "UIButton.h"
#include "UILabel.h"


PopupAdjustHSV::PopupAdjustHSV(std::string tt, std::string tx, bool acceptAlpha)
{
    title = tt;
    text = tx;

    UILabel* hlabel = new UILabel();
    hlabel->position = XY{ 10, 55 };
    hlabel->text = "Hue";
    wxsManager.addDrawable(hlabel);

    hSlider = new UISlider();
    hSlider->position = XY{ 115, 50 };
    hSlider->wxWidth = 400;
    hSlider->setCallbackListener(0, this);
    wxsManager.addDrawable(hSlider);


    UILabel* slabel = new UILabel();
    slabel->position = XY{ 10, 115 };
    slabel->text = "Saturation";
    wxsManager.addDrawable(slabel);

    sSlider = new UISlider();
    sSlider->position = XY{ 115, 110 };
    sSlider->wxWidth = 400;
    sSlider->setCallbackListener(1, this);
    wxsManager.addDrawable(sSlider);


    UILabel* vlabel = new UILabel();
    vlabel->position = XY{ 10, 175 };
    vlabel->text = "Value";
    wxsManager.addDrawable(vlabel);

    vSlider = new UISlider();
    vSlider->position = XY{ 115, 170 };
    vSlider->wxWidth = 400;
    vSlider->setCallbackListener(2, this);
    wxsManager.addDrawable(vSlider);

    updateSliderValues();

    UIButton* nbutton = new UIButton();
    nbutton->text = "Apply";
    nbutton->position = XY{ wxWidth - 260, wxHeight - 40 };
    nbutton->wxWidth = 120;
    nbutton->setCallbackListener(0, this);
    wxsManager.addDrawable(nbutton);

    UIButton* nbutton2 = new UIButton();
    nbutton2->text = "Cancel";
    nbutton2->position = XY{ wxWidth - 130, wxHeight - 40 };
    nbutton2->wxWidth = 120;
    nbutton2->setCallbackListener(1, this);
    wxsManager.addDrawable(nbutton2);
}

void PopupAdjustHSV::render()
{
    renderDefaultBackground();

    XY titlePos = getDefaultTitlePosition();
    XY contentPos = getDefaultContentPosition();

    g_fnt->RenderString(title, titlePos.x, titlePos.y);
    g_fnt->RenderString(text, contentPos.x, contentPos.y);

    renderDrawables();
}

void PopupAdjustHSV::eventButtonPressed(int evt_id)
{
    if (evt_id == 0) {
        if (callback != NULL) {
            callback->eventPopupClosed(callback_id, this);
        }
    }
    g_closePopup(this);
}

void PopupAdjustHSV::eventTextInput(int evt_id, std::string data)
{
}

void PopupAdjustHSV::eventSliderPosChanged(int evt_id, float value)
{
    if (evt_id == 0) {
        adjH = (value - 0.5) * 2 * 360;
    }
    else if (evt_id == 1) {
        adjS = (value - 0.5) * 2;
    }
    else if (evt_id == 2) {
        adjV = (value - 0.5) * 2;
    }
}

void PopupAdjustHSV::updateSliderValues()
{
    hSlider->sliderPos = 0.5 + adjH / 360 * 0.5;
    sSlider->sliderPos = 0.5 + adjS * 0.5;
    vSlider->sliderPos = 0.5 + adjV * 0.5;
}
