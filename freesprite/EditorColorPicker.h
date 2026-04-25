#pragma once
#include "UIButton.h"
#include "IEditorColorPicker.h"
#include "PanelUserInteractable.h"
#include "maineditor.h"

class SessionLocalColorList : public IPalette {
protected:
    MainEditor* target = NULL;
public:
    SessionLocalColorList(MainEditor* target) : target(target) {}

    std::string getName() override { return "Session-local palette"; }
    std::vector<std::pair<std::string, u32>>& getColorList() { return target->ssne.ineditorColorList; }

    bool canSave() override { return true; }
    bool save() override { 
        int i = 0;
        for (auto& kv : getColorList()) {
            kv.first = frmt("{:02}", i++);
        }
        target->changesSinceLastSave = HAS_UNSAVED_CHANGES; 
        return true; 
    }
};

class EditorColorPicker : public PanelUserInteractable, public IEditorColorPicker
{
protected:
    UIColorPicker* wColorPicker = NULL;

    void _toggleEraser();
    void updatePanelColors();
public:
    MainEditor* caller = NULL;
    SessionLocalColorList callerColorList;

    UIButton* eraserButton = NULL;
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
    void forceFocusOnColorInputField() override;

    void setColorRGB(u32 color) override;

    void updateAlphaSlider() override;

    void pushLastColor(u32 color) override;
    void clearLastColors() override;
    void reloadColorLists() override;

    void actionCtrlScroll(float scrollAmount) override;
    void actionShiftScroll(float scrollAmount) override;
};

