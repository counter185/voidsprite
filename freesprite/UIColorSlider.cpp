#include "UIColorSlider.h"
#include "mathops.h"
#include "EditorColorPicker.h"

void UIColorSlider::render(XY pos)
{
    for (int x = 0; x < 360; x++) {
        rgb rgbcolor = hsv2rgb(hsv{ (double)x, 1.0, 1.0 });
        SDL_SetRenderDrawColor(g_rd, rgbcolor.r * 255, rgbcolor.g * 255, rgbcolor.b * 255, parent->focused ? 0xff : 0x30);
        SDL_RenderDrawLine(g_rd, pos.x + x, pos.y, pos.x + x, pos.y + wxHeight);
    }
	drawPosIndicator(pos);
}

void UIColorSlider::onSliderPosChanged()
{
    parent->editorColorHSliderChanged(this->sliderPos * 360);
}
