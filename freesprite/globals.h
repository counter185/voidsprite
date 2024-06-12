#pragma once

//#include <math.h>

#include <string>
#include <format>
#include <map>
#include <unordered_map>
#include <new>
#include <ctime>
#include <vector>
#include <functional>
#include <algorithm>
#include <filesystem>

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
class GlobalNavBar;
class EditorColorPicker;
class EditorBrushPicker;
class BaseScreen;
class BasePopup;
class BaseBrush;
class Layer;
class MainEditor;
class SpritesheetPreviewScreen;

extern bool g_ctrlModifier, g_shiftModifier;
extern int g_windowW, g_windowH;
extern SDL_Window* g_wd;
extern SDL_Renderer* g_rd;
extern TextRenderer* g_fnt;
extern int g_mouseX, g_mouseY;
extern std::vector<BaseBrush*> g_brushes;

extern std::vector<std::string> g_cmdlineArgs;

extern SDL_Texture* g_mainlogo,
   *g_iconLayerAdd,
   *g_iconLayerDelete,
   *g_iconLayerUp,
   *g_iconLayerDown,
   *g_iconLayerDownMerge,
   *g_iconEraser,
   *g_iconColorHSV,
   *g_iconColorVisual,
   *g_iconNavbarTabFile,
   *g_iconNavbarTabEdit,
   *g_iconNavbarTabLayer,
   *g_iconNavbarTabView,
   *g_iconComment;

void g_addScreen(BaseScreen* a);
void g_closeLastScreen();
void g_closeScreen(BaseScreen* screen);

void g_addPopup(BasePopup* a);
void g_popDisposeLastPopup(bool dispose = true);
void g_closePopup(BasePopup* a);

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
	SDL_Texture* icon = NULL;
};

#define UNDOSTACK_LAYER_DATA_MODIFIED 0
#define UNDOSTACK_CREATE_LAYER 1
#define UNDOSTACK_DELETE_LAYER 2
#define UNDOSTACK_MOVE_LAYER 3
#define UNDOSTACK_ADD_COMMENT 4
#define UNDOSTACK_REMOVE_COMMENT 5
struct UndoStackElement {
	Layer* targetlayer;
	uint32_t type = 0;
	int extdata = 0;
	int extdata2 = 0;
	std::string extdata3 = "";
};

#include "platform.h"
#include "colors.h"