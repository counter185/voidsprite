#include "UIHueSlider.h"
#include "mathops.h"
#include "UIColorPicker.h"


void UIHueSlider::onSliderPosChanged()
{
    parent->editorColorHSliderChanged(this->getValue(0,360));
}
