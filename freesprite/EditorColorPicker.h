#pragma once
#include "UIColorPicker.h"
#include "UIButton.h"


class EditorColorPicker : public UIColorPicker
{
protected:
    EditorColorPicker() : UIColorPicker(400, 390) {}
public:
    MainEditor* caller = NULL;

    UIButton* eraserButton = NULL;
    UIButton* blendModeButton = NULL;
    UIColorSlider* alphaSlider = NULL;

    std::vector<uint32_t> lastColors;
    bool lastColorsChanged = true;

    EditorColorPicker(MainEditor* c);

    void render(XY position) override;

    void updateEraserAndAlphaBlendButtons();
    void toggleEraser();
    void toggleAlphaBlendMode();
    void updateAlphaSlider();

    void pushLastColor(uint32_t col);
    void updateLastColorButtons() override;
};

