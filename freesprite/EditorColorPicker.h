#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "UIColorPicker.h"
#include "EventCallbackListener.h"
#include "UIHueSlider.h"
#include "UIColorSlider.h"
#include "UISVPicker.h"
#include "UITextField.h"
#include "UIButton.h"
#include "TabbedView.h"
#include "UILabel.h"
#include "DraggablePanel.h"
#include "colormodels.h"


class EditorColorPicker : public UIColorPicker
{
protected:
	EditorColorPicker() : UIColorPicker(400, 390) {}
public:
    MainEditor* caller = NULL;

    UIButton* eraserButton = NULL;
    UIButton* blendModeButton = NULL;

    std::vector<uint32_t> lastColors;
    bool lastColorsChanged = true;

    EditorColorPicker(MainEditor* c);

    void render(XY position) override;

    void updateEraserAndAlphaBlendButtons();
    void toggleEraser();
    void toggleAlphaBlendMode();

    void pushLastColor(uint32_t col);
    void updateLastColorButtons() override;
};

