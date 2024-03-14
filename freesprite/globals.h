#pragma once

//#include <math.h>

#include <string>
#include <map>
#include <new>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#define FONT_PATH "appfont-MPLUSRounded1c-Medium.ttf"
#define FONT_PATH_JP "appfontjp-NotoSansJP-VariableFont_wght.ttf"

class TextRenderer;
class MainEditor;
class EditorColorPicker;

extern int g_windowW, g_windowH;
extern SDL_Window* g_wd;
extern SDL_Renderer* g_rd;
extern TextRenderer* g_fnt;
extern int g_mouseX, g_mouseY;

struct XY {
	int x, y;
};