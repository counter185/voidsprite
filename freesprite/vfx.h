#pragma once

enum VFXType {
	VFX_BUTTONPULSE,
	VFX_SCREENCLOSE,
	VFX_SCREENSWITCH,	//ext1:  0: both, 1: left, 2: right
};

void g_newVFX(VFXType type, u32 durationMS, u32 extData1 = 0, SDL_Rect extData2 = {0,0,0,0});
void g_renderVFX();