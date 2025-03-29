#pragma once

#ifdef _MSVC_LANG
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4838)

//macro redefinition warning, remove this line after fully migrating to sdl3
#pragma warning(disable : 4005)
#endif

//#include <math.h>

#include <chrono>
#include <string>
#include <format>
#include <map>
#include <unordered_map>
#include <stack>
#include <new>
#include <ctime>
#include <vector>
#include <functional>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <regex>
#include <thread>
#include <atomic>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#if SDL_MAJOR_VERSION == 3
#include "sdl23compat.h"
#endif

extern "C" {
#include <zlib.h>
}

#ifdef __GNUC__
#define sprintf_s snprintf
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL
#endif

#define u64 uint64_t
#define s64 int64_t
#define u32 uint32_t
#define s32 int32_t
#define u16 uint16_t
#define s16 int16_t
#define u8 uint8_t
#define s8 int8_t

#ifndef _WIN32
#define _WIN32 0
#endif

#ifndef WINDOWS_XP
#define WINDOWS_XP 0
#endif

#if WINDOWS_XP
#define SDL_GetTicks64 SDL_GetTicks
#endif

#ifndef VOIDSPRITE_ASSETS_PATH
#define VOIDSPRITE_ASSETS_PATH ""
#endif

#define ARRAY2DPOINT(arr,x,y,w) (arr)[(y)*w+(x)]

#define FONT_PATH VOIDSPRITE_ASSETS_PATH "appfont-MPLUSRounded1c-Medium.ttf"
#define FONT_PATH_JP VOIDSPRITE_ASSETS_PATH "appfontjp-NotoSansJP-VariableFont_wght.ttf"

#define COLOR_INFO SDL_Color{0x63, 0xc6, 0xff, 255}
#define COLOR_ERROR SDL_Color{255,0,0,255}

#define UTF8_DIAMOND "\xE2\x97\x86"
#define UTF8_EMPTY_DIAMOND "\xE2\x97\x87"

#ifndef INT_MAX
#define INT_MAX 2147483647
#define INT_MIN (-2147483647 - 1)
#endif

//util classes
class EventCallbackListener;
class TextRenderer;
class Layer;
class LayerPalettized;
class Pattern;
class Notification;
class Gamepad;
class Timer64;
class TooltipsLayer;
class FileImporter;
class FileExporter;
struct NineSegmentPattern;
class DrawableManager;
class TextureBuffer;

//templates
class BaseTemplate;

//screens/popups
class BaseScreen;
class BasePopup;
class MainEditor;
class MainEditorPalettized;
class SpritesheetPreviewScreen;
class TilemapPreviewScreen;
class MinecraftBlockPreviewScreen;

//specialized ui elements
class EditorColorPicker;
class EditorBrushPicker;
class EditorLayerPicker;
class EditorSpritesheetPreview;
class TilemapEditorLayerPicker;
class PanelSpritesheetPreview;
class ButtonStartScreenSession;
class PanelMCBlockPreview;

//brushes/tools
class BaseBrush;
class Brush9SegmentRect;
class ToolText;

//standard ui elements
class UIColorInputField;
class UILayerButton;
class UIButton;
class UILabel;
class UISlider;
class UITextField;
class UICheckbox;
class Panel;
class ScrollingPanel;

//filters
class BaseFilter;
class RenderFilter;

template <typename T>
class ScreenWideNavBar;

extern bool g_ctrlModifier, g_shiftModifier;
extern int g_windowW, g_windowH;
extern std::string g_programDirectory;
extern SDL_Window* g_wd;
extern SDL_Renderer* g_rd;
extern TextRenderer* g_fnt;
extern TooltipsLayer* g_ttp;
extern Gamepad* g_gamepad;
extern int g_mouseX, g_mouseY;
extern std::vector<BaseBrush*> g_brushes;
extern std::vector<Pattern*> g_patterns;
extern std::vector<BaseTemplate*> g_templates;
inline double g_deltaTime = 1.0;
inline double g_frameDeltaTime = 0.001;
inline std::vector<NineSegmentPattern*> g_9spatterns;
inline std::vector<BaseFilter*> g_filters;
inline std::vector<RenderFilter*> g_renderFilters;
inline bool g_windowFocused = true;

extern std::vector<std::string> g_cmdlineArgs;

extern SDL_Texture* g_mainlogo,
   *g_iconLayerAdd,
   *g_iconLayerDelete,
   *g_iconLayerUp,
   *g_iconLayerDown,
   *g_iconLayerDownMerge,
   *g_iconLayerDuplicate,
   *g_iconEraser,
   *g_iconBlendMode,
   *g_iconColorHSV,
   *g_iconColorRGB,
   *g_iconColorVisual,
   *g_iconNavbarTabFile,
   *g_iconNavbarTabEdit,
   *g_iconNavbarTabLayer,
   *g_iconNavbarTabView,
   *g_iconComment,
   *g_iconMenuPxDim,
   *g_iconMenuSpritesheet,
   *g_iconMenuTemplates,
   *g_iconNotifTheCreature,
   *g_iconNotifError,
   *g_iconNotifSuccess,
   *g_iconNewColor;

void g_addNotification(Notification a);

void g_addScreen(BaseScreen* a, bool switchTo = true);
void g_closeLastScreen();
void g_closeScreen(BaseScreen* screen);
void g_switchScreen(int index);

void g_addPopup(BasePopup* a);
void g_popDisposeLastPopup(bool dispose = true);
void g_closePopup(BasePopup* a);

void g_pushClip(SDL_Rect r);
void g_popClip();

void g_pushRenderTarget(SDL_Texture* tex);
void g_popRenderTarget();

SDL_Surface* IMGLoadToSurface(std::string path);
SDL_Texture* IMGLoadToTexture(std::string path);

struct XY {
    int x, y;
};

template <typename T>
struct NamedOperation {
    std::string name;
    std::function<void(T)> function;
};

template <typename T>
struct NavbarSection {
    std::string name;
    std::vector<SDL_Scancode> order;
    std::map<SDL_Scancode, NamedOperation<T>> actions;
    SDL_Texture* icon = NULL;
    UIButton* button = NULL;
};

#define UNDOSTACK_LAYER_DATA_MODIFIED 0
#define UNDOSTACK_CREATE_LAYER 1
#define UNDOSTACK_DELETE_LAYER 2
#define UNDOSTACK_MOVE_LAYER 3
#define UNDOSTACK_ADD_COMMENT 4
#define UNDOSTACK_REMOVE_COMMENT 5
#define UNDOSTACK_SET_OPACITY 6
#define UNDOSTACK_RESIZE_LAYER 7
struct UndoStackElement {
    Layer* targetlayer = NULL;
    uint32_t type = 0;
    int extdata = 0;
    int extdata2 = 0;
    std::string extdata3 = "";
    void* extdata4 = NULL;
};

struct UndoStackResizeLayerElement {
    XY oldDimensions;
    uint8_t* oldData;
};

struct NineSegmentPattern {
    XY dimensions;
    u32* pixelData;
    XY point1, point2;
    std::string name = "Default pattern";
    SDL_Texture* cachedTexture = NULL;
};

#include "localization/localization.h"
#include "memory_tracker.h"
#include "platform.h"
#include "colors.h"
#include "palettes.h"
#include "settings.h"
#include "fills.h"
