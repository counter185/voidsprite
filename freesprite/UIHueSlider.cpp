#include "UIHueSlider.h"
#include "mathops.h"
#include "EditorColorPicker.h"

void UIHueSlider::render(XY pos)
{
    u32 approxColors[] = {
        0xFFFF0000,
        0xFFFFFF00,
        0xFF00FF00,
        0xFF00FFFF,
        0xFF0000FF,
        0xFFFF00FF,
        0xFFFF0000
    };
    //UISlider::render(pos);
    int segW = wxWidth / 6;
    int xNow = pos.x;

    //v3
    SDL_Vertex gradientPoints[14];
    for (int x = 0; x < 6; x++) {
        gradientPoints[x].position = xytofp({ xNow, pos.y });
        gradientPoints[x + 7].position = xytofp({ xNow, pos.y + wxHeight });
        u32 c = approxColors[x];
        gradientPoints[x].color = gradientPoints[x + 7].color = toFColor({(u8)((c & 0xFF0000) >> 16), (u8)((c & 0x00FF00) >> 8), (u8)(c & 0x0000FF), 0xff});
        xNow += segW;
    }
    gradientPoints[6].color = gradientPoints[13].color = gradientPoints[0].color;
    gradientPoints[6].position = xytofp({pos.x + wxWidth, pos.y});
    gradientPoints[13].position = xytofp({pos.x + wxWidth, pos.y + wxHeight});

    int indices[] = {
        0,1,7, 1,7,8,
        1,2,8, 2,8,9,
        2,3,9, 3,9,10,
        3,4,10, 4,10,11,
        4,5,11, 5,11,12,
        5,6,12, 6,12,13
    };
    SDL_RenderGeometry(g_rd, NULL, gradientPoints, 14, indices, 36);

    //v2
    /*for (int s = 0; s < 6; s++) {
        SDL_Rect gradientR = {xNow, pos.y, segW, wxHeight};
        renderGradient(gradientR, approxColors[s], approxColors[s + 1], approxColors[s], approxColors[s + 1]);
        xNow += segW;
    }*/

    //old method
    /*for (int x = 0; x < 360; x++) {
        rgb rgbcolor = hsv2rgb(hsv{ (double)x, 1.0, 1.0 });
        SDL_SetRenderDrawColor(g_rd, rgbcolor.r * 255, rgbcolor.g * 255, rgbcolor.b * 255, 0xff);//parent->focused ? 0xff : 0x30);
        SDL_RenderDrawLine(g_rd, pos.x + x, pos.y, pos.x + x, pos.y + wxHeight);
    }*/
	drawPosIndicator(pos);
}

void UIHueSlider::onSliderPosChanged()
{
    parent->editorColorHSliderChanged(this->sliderPos * 360);
}
