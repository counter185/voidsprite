#pragma once

//#include <math.h>

#include <string>
#include <format>
#include <map>
#include <unordered_map>
#include <new>
#include <vector>
#include <functional>
#include <algorithm>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <zlib.h>

#ifdef __GNUC__
#define sprintf_s snprintf
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL
#endif

#ifndef _WIN32
#define _WIN32 0
#endif

#define FONT_PATH "appfont-MPLUSRounded1c-Medium.ttf"
#define FONT_PATH_JP "appfontjp-NotoSansJP-VariableFont_wght.ttf"

class EventCallbackListener;
class TextRenderer;
class MainEditor;
class GlobalNavBar;
class EditorColorPicker;
class EditorBrushPicker;
class BaseScreen;
class BasePopup;
class BaseBrush;
class Layer;

extern int g_windowW, g_windowH;
extern SDL_Window* g_wd;
extern SDL_Renderer* g_rd;
extern TextRenderer* g_fnt;
extern int g_mouseX, g_mouseY;
extern std::vector<BaseBrush*> g_brushes;

extern SDL_Texture* g_mainlogo;

void g_addScreen(BaseScreen* a);
void g_closeLastScreen();
void g_closeScreen(BaseScreen* screen);

void g_addPopup(BasePopup* a);
void g_popDisposeLastPopup(bool dispose = true);

struct XY {
	int x, y;
};

struct NamedEditorOperation {
	std::string name;
	std::function<void(MainEditor*)> function;
};

struct NavbarSection {
	std::string name;
	std::vector<SDL_Keycode> order;
	std::map<SDL_Keycode, NamedEditorOperation> actions;
};

#include "platform.h"
#include "colors.h"