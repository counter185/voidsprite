#include "PopupApplyFilter.h"
#include "BaseFilter.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UIButton.h"
#include "maineditor.h"

void PopupApplyFilter::defaultInputAction(SDL_Event evt)
{
    if (evt.type == SDL_QUIT) {
        closePopup();
    }
}

void PopupApplyFilter::eventButtonPressed(int evt_id)
{
    if (evt_id == -1) {
        // apply
        applyAndClose();
    }
    else if (evt_id == -2) {
        // cancel
        closePopup();
    }
}

void PopupApplyFilter::eventSliderPosChanged(int evt_id, float value)
{
    if (evt_id < params.size()) {
        FilterParameter& p = params[evt_id];
        switch (p.paramType) {
            case PT_INT:
                p.defaultValue = (int)(p.minValue + (p.maxValue - p.minValue) * value);
                break;
            case PT_FLOAT:
                p.defaultValue = p.minValue + (p.maxValue - p.minValue) * value;
                break;
        }
    }
}

void PopupApplyFilter::setupWidgets()
{
    UILabel* title = new UILabel();
    title->text = targetFilter->name();
    title->position = XY{5, 5};
    wxsManager.addDrawable(title);

	params = targetFilter->getParameters();
    int y = 50;
    int i = 0;
	for (auto& p : params) {
        UILabel* label = new UILabel();
        label->text = p.name;
        label->position = XY{10, y + 2};
        wxsManager.addDrawable(label);

        switch (p.paramType) {
            case PT_INT:
            case PT_FLOAT:
                float v = (p.defaultValue - p.minValue) / (p.maxValue - p.minValue);
                UISlider* slider = new UISlider();
                slider->position = XY{200, y};
                slider->sliderPos = v;
                slider->wxHeight = 25;
                slider->setCallbackListener(i, this);
                wxsManager.addDrawable(slider);
                break;
        }
        i++;
        y += 40;
	}

    UIButton* btnApply = new UIButton();
    btnApply->text = "Apply";
    btnApply->wxWidth = 100;
    btnApply->position = XY{wxWidth - 10 - btnApply->wxWidth, wxHeight - 10 - btnApply->wxHeight};
    btnApply->setCallbackListener(-1, this);
    wxsManager.addDrawable(btnApply);
    
    UIButton* btnCancel = new UIButton();
    btnCancel->text = "Cancel";
    btnCancel->wxWidth = 100;
    btnCancel->position = XY{btnApply->position.x - 10 - btnCancel->wxWidth, wxHeight - 10 - btnCancel->wxHeight};
    btnCancel->setCallbackListener(-2, this);
    wxsManager.addDrawable(btnCancel);

}

void PopupApplyFilter::applyAndClose()
{
    std::map<std::string, std::string> parameterMap;
    for (auto& p : params) {
        parameterMap[p.name] = std::to_string(p.defaultValue);
    }

    Layer* copy = targetFilter->run(target, parameterMap);
    session->commitStateToCurrentLayer();
    memcpy(target->pixelData, copy->pixelData, 4 * target->w * target->h);
    target->layerDirty = true;
    delete copy;
    closePopup();
}
