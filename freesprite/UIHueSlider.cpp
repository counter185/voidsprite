#include "UIHueSlider.h"
#include "mathops.h"
#include "EditorColorPicker.h"


void UIHueSlider::onSliderPosChanged()
{
    parent->editorColorHSliderChanged(this->sliderPos * 360);
}
