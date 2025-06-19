#pragma once

#ifdef _MSVC_LANG
#pragma warning(disable : 4018) //signed/unsigned mismatch
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4838)
#pragma warning(disable : 4477) //logprintf wrong format argument whatever
#pragma warning(disable : 4099) //liblcf link without debugging info whatever don't care

//macro redefinition warning, remove this line after fully migrating to sdl3
#pragma warning(disable : 4005)
#endif

//#include <math.h>

#include <stdarg.h>
#include <chrono>
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
#include <atomic>
//do not include these here:
//iostream: use logprintf or loginfo instead
//fstream: include it in cpp files where needed
//stack: include it in cpp files where needed
//regex: include it in cpp files where needed
//SDL3_image/SDL_image.h: include it in cpp files where needed
//SDL3_ttf/SDL_ttf.h: include it in cpp files where needed

#include <SDL3/SDL.h>

#if SDL_MAJOR_VERSION == 3
#include "sdl23compat.h"
#endif

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
#ifndef VOIDSPRITE_ASSETS_SUBDIR
    #if __ANDROID__
    #define VOIDSPRITE_ASSETS_SUBDIR ""
    #else
    #define VOIDSPRITE_ASSETS_SUBDIR "/assets/"
    #endif
#endif

#define ARRAY2DPOINT(arr,x,y,w) (arr)[(y)*w+(x)]

#define FONT_PATH "appfont-MPLUSRounded1c-Medium.ttf"
#define FONT_PATH_JP "appfontjp-NotoSansJP-Medium.ttf"

#define COLOR_INFO SDL_Color{0x63, 0xc6, 0xff, 255}
#define COLOR_ERROR SDL_Color{255,0,0,255}

#define UTF8_DIAMOND "\xE2\x97\x86"
#define UTF8_EMPTY_DIAMOND "\xE2\x97\x87"

#ifndef INT_MAX
#define INT_MAX 2147483647
#define INT_MIN (-2147483647 - 1)
#endif

//sdl
struct TTF_Font;

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
class ExtractDataScreen;

//specialized ui elements
class EditorColorPicker;
class EditorBrushPicker;
class EditorLayerPicker;
class EditorSpritesheetPreview;
class EditorTouchToggle;
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
class UIDropdown;
class UIColorPicker;
class Panel;
class ScrollingPanel;
class TabbedView;

//filters
class BaseFilter;
class RenderFilter;

template <typename T>
class ScreenWideNavBar;

extern bool g_ctrlModifier, g_shiftModifier;
extern int g_windowW, g_windowH;
inline int g_renderScale = 1;
extern std::string g_programDirectory;
extern SDL_Window* g_wd;
extern SDL_Renderer* g_rd;
extern TextRenderer* g_fnt;
extern TooltipsLayer* g_ttp;
extern Gamepad* g_gamepad;
extern int g_mouseX, g_mouseY;
extern std::vector<BaseBrush*> g_brushes;
extern std::vector<Pattern*> g_patterns;
inline std::vector<BaseTemplate*> g_templates;
inline double g_deltaTime = 1.0;
inline double g_frameDeltaTime = 0.001;
inline std::vector<NineSegmentPattern*> g_9spatterns;
inline std::vector<BaseFilter*> g_filters;
inline std::vector<BaseFilter*> g_pluginFilters;
inline std::vector<RenderFilter*> g_renderFilters;
inline bool g_windowFocused = true;
inline SDL_PropertiesID g_props;

extern std::vector<std::string> g_cmdlineArgs;

inline SDL_Texture* g_mainlogo = NULL,
   *g_iconLayerAdd = NULL,
   *g_iconLayerDelete = NULL,
   *g_iconLayerUp = NULL,
   *g_iconLayerDown = NULL,
   *g_iconLayerDownMerge = NULL,
   *g_iconLayerDuplicate = NULL,
   *g_iconLayerHide = NULL,
   *g_iconEraser = NULL,
   *g_iconBlendMode = NULL,
   *g_iconColorHSV = NULL,
   *g_iconColorRGB = NULL,
   *g_iconColorVisual = NULL,
   *g_iconNavbarTabFile = NULL,
   *g_iconNavbarTabEdit = NULL,
   *g_iconNavbarTabLayer = NULL,
   *g_iconNavbarTabView = NULL,
   *g_iconComment = NULL,
   *g_iconMenuPxDim = NULL,
   *g_iconMenuSpritesheet = NULL,
   *g_iconMenuTemplates = NULL,
   *g_iconNotifTheCreature = NULL,
   *g_iconNotifError = NULL,
   *g_iconNotifSuccess = NULL,
   *g_iconNewColor = NULL,
   *g_iconActionBarUndo = NULL,
   *g_iconActionBarRedo = NULL,
   *g_iconActionBarZoomIn = NULL,
   *g_iconActionBarZoomOut = NULL,
   *g_iconActionBarSave = NULL,
   *g_iconFilePickerDirectory = NULL,
   *g_iconFilePickerFile = NULL,
   *g_iconFilePickerSupportedFile = NULL;

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

void g_reloadFonts();

SDL_Texture* IMGLoadToTexture(std::string path);
SDL_Texture* IMGLoadAssetToTexture(std::string path);

struct XY {
    int x, y;
};
struct XYf {
    float x, y;
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
#define UNDOSTACK_ALL_LAYER_DATA_MODIFIED 8
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

#include "log.h"
#include "localization/localization.h"
#include "memory_tracker.h"
#include "platform.h"
#include "colors.h"
#include "palettes.h"
#include "settings.h"
#include "fills.h"
#include "vfx.h"
#include "visual_config.h"
