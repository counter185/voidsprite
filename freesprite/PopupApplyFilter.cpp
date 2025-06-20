#include "PopupApplyFilter.h"
#include "BaseFilter.h"
#include "EventCallbackListener.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UIDoubleSlider.h"
#include "UIButton.h"
#include "UIColorInputField.h"
#include "UICheckbox.h"
#include "maineditor.h"
#include "background_operation.h"
#include "FontRenderer.h"

PopupApplyFilter::~PopupApplyFilter() {
    if (previewRenderThreadObj.joinable()) {
        previewRenderThreadShouldRun = false;
        previewRenderThreadObj.join();
    }
    if (previewPixelData != NULL) {
        tracked_free(previewPixelData);
    }
    if (previewTexture != NULL) {
        target->effectPreviewTexture = NULL;
        tracked_destroyTexture(previewTexture);
    }
}

void PopupApplyFilter::render() {
    updatePreview();
    renderFilterPopupBackground();
    renderDrawables();
    if (nowRendering) {
        g_fnt->RenderString(TL("vsp.applyfilter.renderingpreview"), 2, g_windowH - 30, {255,255,255,200});
    }
}

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
    if ((size_t) evt_id < params.size()) {
        FilterParameter& p = params[evt_id];
        switch (p.paramType) {
            case PT_INT:
                p.defaultValue = (int)(p.minValue + (p.maxValue - p.minValue) * value);
                break;
            case PT_FLOAT:
                p.defaultValue = p.minValue + (p.maxValue - p.minValue) * value;
                break;
            default: break;
        }
        updateLabels();
        threadHasNewParameters = true;
    }
}

void PopupApplyFilter::eventDoubleSliderPosChanged(int evt_id, UIDoubleSliderBounds value)
{
    if ((size_t) evt_id < params.size()) {
        FilterParameter& p = params[evt_id];
        switch (p.paramType) {
            case PT_INT_RANGE:
                p.defaultValue = (int)(p.minValue + (p.maxValue - p.minValue) * value.min);
                p.defaultValueTwo = (int)(p.minValue + (p.maxValue - p.minValue) * value.max);
                break;
            default: break;
        }
        updateLabels();
        threadHasNewParameters = true;
    }
}

void PopupApplyFilter::eventCheckboxToggled(int evt_id, bool newState)
{
    if ((size_t) evt_id < params.size()) {
        FilterParameter& p = params[evt_id];
        if (p.paramType == PT_BOOL) {
            p.defaultValue = newState ? 1 : 0;
            threadHasNewParameters = true;
        }
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
    UILabel* title = new UILabel(targetFilter->name());
    title->fontsize = 22;
    title->position = XY{5, 5};
    wxsManager.addDrawable(title);

    params = targetFilter->getParameters();
    int y = 50;
    int i = 0;
    for (auto& p : params) {
        UILabel* label = new UILabel(p.name);
        label->position = XY{10, y + 2};
        wxsManager.addDrawable(label);

        switch (p.paramType) {
            case PT_BOOL:
            {
                UICheckbox* checkbox = new UICheckbox("", p.defaultValue == 1);
                checkbox->position = XY{ 250, y };
                checkbox->setCallbackListener(i, this);
                wxsManager.addDrawable(checkbox);
            }
                break;
            case PT_INT:
            case PT_FLOAT:
            {
                float v = (p.defaultValue - p.minValue) / (p.maxValue - p.minValue);
                UISlider* slider = new UISlider();
                slider->position = XY{ 285, y };
                slider->sliderPos = v;
                slider->wxHeight = 25;
                slider->setCallbackListener(i, this);
                wxsManager.addDrawable(slider);

                UILabel* valueLabel = new UILabel();
                valueLabel->position = xySubtract(slider->position, { 84, 0 });
                paramLabels.push_back(valueLabel);
                wxsManager.addDrawable(valueLabel);
            }
                break;
            case PT_COLOR_RGB:
            {
                UIColorInputField* colorInput = new UIColorInputField();
                colorInput->position = XY{ 250, y };
                colorInput->wxHeight = 25;
                colorInput->setCallbackListener(i, this);
                wxsManager.addDrawable(colorInput);
            }
                break;
            case PT_INT_RANGE:
            {
                float vl = (p.defaultValue - p.minValue) / (p.maxValue - p.minValue);
                float vm = (p.defaultValueTwo - p.minValue) / (p.maxValue - p.minValue);
                UIDoubleSlider* slider = new UIDoubleSlider();
                slider->position = XY{ 285, y };
                slider->sliderPos.min = vl;
                slider->sliderPos.max = vm;
                slider->bodyColor = p.vU32;
                slider->wxHeight = 25;
                slider->setCallbackListener(i, this);
                wxsManager.addDrawable(slider);

                UILabel* valueLabel = new UILabel();
                valueLabel->position = xySubtract(slider->position, { 84, 0 });
                paramLabels.push_back(valueLabel);
                wxsManager.addDrawable(valueLabel);
            }
                break;
        }
        i++;
        y += 40;
    }
    updateLabels();

    setSize({ 550, y + 70 });

    UIButton* btnApply = new UIButton();
    btnApply->text = TL("vsp.cmn.apply");
    btnApply->wxWidth = 100;
    btnApply->position = XY{wxWidth - 10 - btnApply->wxWidth, wxHeight - 10 - btnApply->wxHeight};
    btnApply->setCallbackListener(-1, this);
    wxsManager.addDrawable(btnApply);
    
    UIButton* btnCancel = new UIButton();
    btnCancel->text = TL("vsp.cmn.cancel");
    btnCancel->wxWidth = 100;
    btnCancel->position = XY{btnApply->position.x - 10 - btnCancel->wxWidth, wxHeight - 10 - btnCancel->wxHeight};
    btnCancel->setCallbackListener(-2, this);
    wxsManager.addDrawable(btnCancel);

}

void PopupApplyFilter::applyAndClose()
{
    std::map<std::string, std::string> parameterMap = makeParameterMap();
    auto target = this->target;
    auto session = this->session;
    auto targetFilter = this->targetFilter;
    g_startNewOperation([parameterMap, targetFilter, session, target]() {
        Layer* copy = targetFilter->run(target, parameterMap);
        session->commitStateToCurrentLayer();
        if (session->isolateEnabled) {
            u32* srcpx = (u32*)copy->pixelData;
            u32* dstpx = (u32*)target->pixelData;
            session->isolatedFragment.forEachPoint([&](XY a) {
                if (pointInBox(a, { 0,0,target->w,target->h })) {
                    ARRAY2DPOINT(dstpx, a.x, a.y, target->w) = ARRAY2DPOINT(srcpx, a.x, a.y, target->w);
                }
            });
        }
        else {
            memcpy(target->pixelData, copy->pixelData, 4 * target->w * target->h);
        }
        target->markLayerDirty();
        delete copy;
    });
    closePopup();
}

void PopupApplyFilter::updateLabels()
{
    for (size_t i = 0; i < paramLabels.size(); i++) {
        FilterParameter& p = params[i];
        UILabel* label = paramLabels[i];
        switch (p.paramType) {
            case PT_INT:
                label->setText(std::to_string((int)p.defaultValue));
                break;
            case PT_FLOAT:
            default:
                label->setText(std::format("{:.1f}", p.defaultValue));
                break;
            case PT_INT_RANGE:
                label->setText(std::to_string((int)p.defaultValue) + ":" + std::to_string((int)p.defaultValueTwo));
                break;
        }
    }
}

void PopupApplyFilter::setupPreview()
{
    previewPixelData = (u8*)tracked_malloc(4 * target->w * target->h);
    previewTexture = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, target->w, target->h);
    SDL_SetTextureBlendMode(previewTexture, SDL_BLENDMODE_BLEND);
    target->effectPreviewTexture = previewTexture;

    previewRenderThreadObj = std::thread(&PopupApplyFilter::previewRenderThread, this);
}

void PopupApplyFilter::updatePreview()
{
    if (pixelDataDirty) {
        u8* ppx;
        int pitch;
        SDL_LockTexture(previewTexture, NULL, (void**)&ppx, &pitch);
        u32* pppx = (u32*)ppx;
        u32* previewPx = (u32*)previewPixelData;
        if (session->isolateEnabled) {
            memcpy(ppx, target->pixelData, 4 * target->w * target->h);
            session->isolatedFragment.forEachPoint([&](XY a) {
                if (pointInBox(a, { 0,0,target->w,target->h })) {
                    ARRAY2DPOINT(pppx, a.x, a.y, target->w) = ARRAY2DPOINT(previewPx, a.x, a.y, target->w);
                }
            });
        }
        else {
            memcpy(ppx, previewPixelData, 4 * target->w * target->h);
        }
        SDL_UnlockTexture(previewTexture);
        pixelDataDirty = false;
    }
}

std::map<std::string, std::string> PopupApplyFilter::makeParameterMap()
{
    std::map<std::string, std::string> parameterMap;
    for (auto& p : params) {
        switch (p.paramType) {
            case PT_INT_RANGE:
                parameterMap[p.name + ".min"] = std::to_string(p.defaultValue);
                parameterMap[p.name + ".max"] = std::to_string(p.defaultValueTwo);
            break;
            default:
                parameterMap[p.name] = std::to_string(p.defaultValue);
            break;
        }
    }
    parameterMap["!editor:activecolor"] = std::to_string(session->getActiveColor());
    return parameterMap;
}

void PopupApplyFilter::previewRenderThread()
{
    while (previewRenderThreadShouldRun) {
        if (threadHasNewParameters) {
            nowRendering = true;
            std::map<std::string, std::string> parameterMap = makeParameterMap();
            threadHasNewParameters = false;
            Layer* l = targetFilter->run(target, parameterMap);
            memcpy(previewPixelData, l->pixelData, 4 * target->w * target->h);
            pixelDataDirty = true;
            delete l;
            nowRendering = false;
        }
        SDL_Delay(1);
    }
}
