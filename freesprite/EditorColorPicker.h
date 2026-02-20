#pragma once
#include "UIButton.h"
#include "IEditorColorPicker.h"
#include "PanelUserInteractable.h"

class EditorColorPicker : public PanelUserInteractable, public IEditorColorPicker
{
protected:
    UIColorPicker* wColorPicker = NULL;

    void _toggleEraser();
    void _toggleBlendMode();
    void updatePanelColors();
public:
    MainEditor* caller = NULL;


    UIButton* eraserButton = NULL;
    UIButton* blendModeButton = NULL;
    UIColorSlider* alphaSlider = NULL;

    std::vector<u32> lastColors;
    bool lastColorsChanged = true;

    EditorColorPicker(MainEditor* c);

    void render(XY at) override {
        updatePanelColors();
        PanelUserInteractable::render(at);
    };
    void renderAfterBG(XY position) override;

    void updateEraserAndAlphaBlendButtons();

    //void pushLastColor(uint32_t col);
    void updateLastColorButtons();

    Panel* getPanel() override { return this; };

    void toggleEraser() override;
    void toggleBlendMode() override;
    void forceFocusOnColorInputField() override;

    void setColorRGB(u32 color) override;

    void updateAlphaSlider() override;

    void pushLastColor(u32 color) override;
    void clearLastColors() override;
    void reloadColorLists() override;

    void actionCtrlScroll(float scrollAmount) override;
    void actionShiftScroll(float scrollAmount) override;
};

