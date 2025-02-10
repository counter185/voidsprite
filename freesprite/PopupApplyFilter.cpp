#include "PopupApplyFilter.h"
#include "BaseFilter.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UIButton.h"
#include "maineditor.h"
#include "background_operation.h"

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
        updateLabels();
    }
}

void PopupApplyFilter::renderFilterPopupBackground()
{
    SDL_Color bgColor = SDL_Color{ 0,0,0,0xD0 };

    int topBarY = g_windowH / 5;
    int bottomBarY = g_windowH / 5 * 4;

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0x60 * startTimer.percentElapsedTime(300)));
    //SDL_RenderFillRect(g_rd, NULL);
    
    renderGradient({ 0,0,wxWidth + 40,g_windowH }, 0x70000000, 0x70000000, 0xFF000000, 0xFF000000);
    //renderGradient({ 0,g_windowH / 2,g_windowW,g_windowH / 2 }, 0xFF000000, 0xFF000000, 0x70000000, 0x70000000);

    /*SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xA0);
    drawLine({ 0, topBarY }, { g_windowW, topBarY }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    drawLine({ g_windowW, bottomBarY }, { 0, bottomBarY }, XM1PW3P1(startTimer.percentElapsedTime(700)));

    int offset = 1;

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x60);
    drawLine({ 0, topBarY - offset }, { g_windowW, topBarY - offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    drawLine({ g_windowW, bottomBarY + offset }, { 0, bottomBarY + offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    offset++;
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x20);
    drawLine({ 0, topBarY - offset }, { g_windowW, topBarY - offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    drawLine({ g_windowW, bottomBarY + offset }, { 0, bottomBarY + offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    offset++;
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x0a);
    drawLine({ 0, topBarY - offset }, { g_windowW, topBarY - offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    drawLine({ g_windowW, bottomBarY + offset }, { 0, bottomBarY + offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));*/

    XY origin = getPopupOrigin();
    SDL_Rect bgRect = SDL_Rect{ origin.x, origin.y, wxWidth, (int)(wxHeight * XM1PW3P1(startTimer.percentElapsedTime(300))) };
    SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(g_rd, &bgRect);

    u8 alpha = 0x30;
    SDL_Rect bgRect2 = bgRect;
    for (int i = 0; i < 3; i++) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, alpha * XM1PW3P1(startTimer.percentElapsedTime(300)));
        SDL_RenderDrawRect(g_rd, &bgRect2);
        bgRect2.x--;
        bgRect2.y--;
        bgRect2.w += 2;
        bgRect2.h += 2;
        alpha /= 3;
        alpha *= 2;
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
                slider->position = XY{250, y};
                slider->sliderPos = v;
                slider->wxHeight = 25;
                slider->setCallbackListener(i, this);
                wxsManager.addDrawable(slider);

                UILabel* valueLabel = new UILabel();
                valueLabel->position = xySubtract(slider->position, { 50, 0 });
                paramLabels.push_back(valueLabel);
                wxsManager.addDrawable(valueLabel);
                break;
        }
        i++;
        y += 40;
	}
    updateLabels();

    setSize({ 550, y + 70 });

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
    auto target = this->target;
    auto session = this->session;
    auto targetFilter = this->targetFilter;
    g_startNewOperation([parameterMap, targetFilter, session, target]() {
        Layer* copy = targetFilter->run(target, parameterMap);
        session->commitStateToCurrentLayer();
        memcpy(target->pixelData, copy->pixelData, 4 * target->w * target->h);
        target->layerDirty = true;
        delete copy;
    });
    closePopup();
}

void PopupApplyFilter::updateLabels()
{
    for (int i = 0; i < paramLabels.size(); i++) {
        FilterParameter& p = params[i];
        UILabel* label = paramLabels[i];
        switch (p.paramType) {
            case PT_INT:
                label->text = std::to_string((int)p.defaultValue);
                break;
            case PT_FLOAT:
            default:
                label->text = std::to_string(p.defaultValue);
                break;
        }
    }
}