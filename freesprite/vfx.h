#pragma once

enum VFXType {
    VFX_BUTTONPULSE,
    VFX_SCREENCLOSE,
    VFX_SCREENSWITCH,	//ext1:  0: both, 1: left, 2: right
    VFX_COLORPICKER,	//ext1: color, ext2: rect
    VFX_POPUPCLOSE,
    VFX_PANELOPEN,
};

void g_newVFX(VFXType type, u32 durationMS, u32 extData1 = 0, SDL_Rect extData2 = { 0,0,0,0 }, std::vector<u32> moreExtData = std::vector<u32>{0});
void g_renderVFX();
void g_removeVFXWindow(VSPWindow*);