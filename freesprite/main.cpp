
#include "globals.h"

#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "FontRenderer.h"
#include "maineditor.h"
#include "BaseScreen.h"
#include "StartScreen.h"
#include "BasePopup.h"
#include "brush/BaseBrush.h"
#include "Pattern.h"
#include "Notification.h"
#include "Gamepad.h"
#include "FileIO.h"
#include "TooltipsLayer.h"
#include "ButtonStartScreenSession.h"
#include "CustomTemplate.h"
#include "ninesegmentpatterns.h"
#include "background_operation.h"
#include "discord_rpc.h"
#include "colormodels.h"
#include "keybinds.h"
#include "sdk_pluginloader.h"
#include "multiwindow.h"

#include "TemplateMC64x32Skin.h"
#include "TemplateRPG2KBattleAnim.h"
#include "TemplateRPG2KCharset.h"
#include "TemplateRPG2KChipset.h"
#include "TemplateRPG2KFaceset.h"
#include "TemplateRPG2KSystem.h"
#include "TemplatePixelIllustration.h"

#include "BaseFilter.h"

#include "ee_creature.h"

#include "main.h"

int g_windowW = 1280;
int g_windowH = 720;
XY unscaledWindowSize = {g_windowW, g_windowH};
std::string g_programDirectory = "";

SDL_Window* g_wd;
SDL_Renderer* g_rd;
int g_mouseX = 0, g_mouseY = 0;
TextRenderer* g_fnt = NULL;
TooltipsLayer* g_ttp;
Gamepad* g_gamepad = NULL;
std::vector<std::string> g_cmdlineArgs;
bool fullscreen = false;
bool g_ctrlModifier = false;
bool g_shiftModifier = false;

std::vector<BaseBrush*> g_brushes;
std::vector<Pattern*> g_patterns;

Timer64 lastPenEvent;

void g_addPopup(BasePopup* a) {
    g_currentWindow->popupStack.push_back(a);
    a->startTimer.start();
}
void g_popDisposeLastPopup(bool dispose) {
    if (dispose) {
        delete g_currentWindow->popupStack[g_currentWindow->popupStack.size() - 1];
    }
    g_currentWindow->popupStack.pop_back();
}
void g_closePopup(BasePopup* a) {
    for (int x = 0; x < g_currentWindow->popupStack.size(); x++) {
        if (g_currentWindow->popupStack[x] == a) {
            delete g_currentWindow->popupStack[x];
            g_currentWindow->popupStack.erase(g_currentWindow->popupStack.begin() + x);
        }
    }
}


void g_addScreen(BaseScreen* a, bool switchTo)
{
    g_currentWindow->addScreen(a, switchTo);
}

void g_closeScreen(BaseScreen* screen) {
    for (auto& [id, wd] : g_windows) {
        //we have to iterate over all windows to close all subscreens too
        wd->closeScreen(screen);
    }
}

void g_switchScreen(int index)
{
    g_currentWindow->switchScreen(index);
}

void g_reloadFonts() {
    VSPWindow* currentWd = g_currentWindow;
    for (auto& [id, wd] : g_windows) {
        wd->thisWindowsTurn();
        wd->initFonts();
    }
    currentWd->thisWindowsTurn();
}

std::vector<SDL_Rect> clips;
void g_pushClip(SDL_Rect r) {
    clips.push_back(r);
    SDL_RenderSetClipRect(g_rd, &r);
}
void g_popClip() {
    clips.pop_back();
    SDL_RenderSetClipRect(g_rd, clips.size() == 0 ? NULL : &clips[clips.size() - 1]);
}

std::vector<SDL_Texture*> rts;
void g_pushRenderTarget(SDL_Texture* rt) {
    rts.push_back(rt);
    SDL_SetRenderTarget(g_rd, rt);
}
void g_popRenderTarget() {
    rts.pop_back();
    SDL_SetRenderTarget(g_rd, rts.size() == 0 ? NULL : rts[rts.size() - 1]);
}


SDL_Texture* IMGLoadToTexture(std::string path) {
    SDL_Surface* srf = IMG_Load(pathInProgramDirectory(path).c_str());
    srf = srf == NULL ? IMG_Load(path.c_str()) : srf;
    if (srf == NULL) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Can't load: " + path));
        return NULL;
    }
    SDL_Texture* ret = tracked_createTextureFromSurface(g_rd, srf);
    SDL_FreeSurface(srf);
    return ret;
}

SDL_Texture* IMGLoadAssetToTexture(std::string path, SDL_Renderer* rd) {
    rd = rd == NULL ? g_rd : rd;
    std::vector<PlatformNativePathString> pathList;
    if (!g_usingDefaultVisualConfig()) {
        PlatformNativePathString vcRoot = g_getCustomVisualConfigRoot();
        pathList.push_back(vcRoot + convertStringOnWin32(VOIDSPRITE_ASSETS_SUBDIR + path));
        pathList.push_back(vcRoot + convertStringOnWin32("/" + path));
    }
    pathList.push_back(convertStringOnWin32(pathInProgramDirectory(VOIDSPRITE_ASSETS_SUBDIR + path)));
    SDL_Surface* srf = NULL;
    for (auto& path : pathList) {
        std::string pathUTF8 = convertStringToUTF8OnWin32(path);
        srf = IMG_Load(pathUTF8.c_str());
        if (srf != NULL) {
            break;
        }
    }
    if (srf == NULL) {
        logerr(std::format("Failed to load asset: {}", convertStringToUTF8OnWin32(pathList[pathList.size() - 1])));
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Can't load: " + path));
        return NULL;
    }
    SDL_Texture* ret = tracked_createTextureFromSurface(rd, srf);
    SDL_FreeSurface(srf);
    return ret;
}

void renderbgOpInProgressScreen() {

    SDL_Rect r = { 0, 0, g_windowW, g_windowH };
    
    u32 colFB = PackRGBAtoARGB(0, 0, 0, (u8)(0xd0 * g_bgOpStartTimer.percentElapsedTime(600)));
    u32 colHB = PackRGBAtoARGB(0, 0, 0, (u8)(0x80 * g_bgOpStartTimer.percentElapsedTime(600)));

    renderGradient({ 0,0,g_windowW,g_windowH / 2 }, colFB, colFB, colHB, colHB);
    renderGradient({ 0,g_windowH/2,g_windowW,g_windowH / 2 + 1 }, colHB, colHB, colFB, colFB);

    TooltipsLayer localttp;

    static std::string bgOpInProgressText = TL("vsp.bgop.inprogress");

    localttp.addTooltip(Tooltip{ {0,g_windowH - 30 }, bgOpInProgressText, {255,255,255,255}, g_bgOpStartTimer.percentElapsedTime(600) });
    localttp.renderAll();
}

void main_toggleFullscreen()
{
    fullscreen = !fullscreen;
    SDL_SetWindowFullscreen(g_wd, fullscreen);
    g_newVFX(VFX_SCREENSWITCH, 800);
}

void main_newWindow(std::string title) {
    VSPWindow* newWindow = VSPWindow::tryCreateWindow(title, { g_windowW, g_windowH }, SDL_WINDOW_RESIZABLE);
    if (newWindow != NULL) {
        newWindow->addToWindowList();
        VSPWindow* oldWindow = g_currentWindow;
        newWindow->thisWindowsTurn();
        newWindow->initFonts();
        newWindow->updateViewportScaler();
        g_currentWindow = newWindow;
        g_newVFX(VFX_SCREENSWITCH, 800);
        newWindow->addScreen(new StartScreen(), true);
        loginfo(std::format("Opening window ID {}", newWindow->windowID));
        if (oldWindow != NULL) {
           oldWindow->thisWindowsTurn();
        }
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.windowcreationfailed")));
    }
}

void main_currentWorkspaceToNewWindow(std::string title)
{
    if (g_currentWindow != NULL && !g_currentWindow->screenStack.empty() && g_currentWindow->popupStack.empty()) {
        BaseScreen* currentScreen = g_currentWindow->screenStack[g_currentWindow->currentScreen];
        VSPWindow* newWindow = VSPWindow::tryCreateWindow(title, { g_windowW, g_windowH }, SDL_WINDOW_RESIZABLE);
        if (newWindow != NULL) {
            g_currentWindow->detachScreen(currentScreen);
            newWindow->addToWindowList();
            VSPWindow* oldWindow = g_currentWindow;
            newWindow->thisWindowsTurn();
            newWindow->initFonts();
            newWindow->updateViewportScaler();
            g_currentWindow = newWindow;
            g_newVFX(VFX_SCREENSWITCH, 800);
            newWindow->addScreen(currentScreen, true);
                        loginfo(std::format("Opening window ID {}", newWindow->windowID));
            if (oldWindow != NULL) {
                oldWindow->thisWindowsTurn();
            }
        }
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.windowcreationfailed")));
    }
}

void main_attachCurrentWorkspaceToMainWindow() {
    if (g_mainWindow != NULL && g_currentWindow != g_mainWindow) {
        BaseScreen* currentScreen = g_currentWindow->screenStack[g_currentWindow->currentScreen];
        g_currentWindow->detachScreen(currentScreen);
        g_mainWindow->addScreen(currentScreen, true);
        g_newVFX(VFX_SCREENSWITCH, 800);
        loginfo(std::format("Attaching workspace to main window ID {}", g_mainWindow->windowID));
    }
    else {
        logerr("attachCurrentWorkspaceToMainWindow failed");
    }
}

void main_renderScaleUp()
{
    g_renderScale++;
    for (auto& [id, wd] : g_windows) {
        wd->renderScale = g_renderScale;
        wd->updateViewportScaler();
    }
}

void main_renderScaleDown()
{
    if (g_renderScale-- <= 1) {
        g_renderScale = 1;
    }
    for (auto& [id, wd] : g_windows) {
        wd->renderScale = g_renderScale;
        wd->updateViewportScaler();
    }
}

//todo fix these
void main_switchToFavScreen()
{
    /*
    if (popupStack.empty() && favourite && fav_screen < screenStack.size()) {
        g_switchScreen(fav_screen);
    }
    */
}

void main_assignFavScreen()
{
    /*
    fav_screen = currentScreen;
    favourite = !favourite;
    */
}

int main(int argc, char** argv)
{
    try {
        std::chrono::time_point<std::chrono::system_clock> startupTime = std::chrono::system_clock::now();

        log_init();
        bool convert = false, convertReadingSrc = false;
        std::vector<std::pair<std::string, std::string>> convertTargets;

        for (int arg = 1; arg < argc; arg++) {
            std::string a = std::string(argv[arg]);
            if (a == "--convert") {
                convert = true;
                convertReadingSrc = true;
            }
            else if (convert) {
                if (convertReadingSrc) {
                    convertTargets.push_back({ a, "" });
                    convertReadingSrc = false;
                }
                else {
                    convertTargets[convertTargets.size() - 1].second = a;
                    convertReadingSrc = false;
                    convert = false;
                }
            }
            else {
                g_cmdlineArgs.push_back(a);
            }
        }
#if __ANDROID__
        g_programDirectory = "";
#else
        g_programDirectory = std::string(argv[0]);
        g_programDirectory = g_programDirectory.substr(0, g_programDirectory.find_last_of("/\\"));
        g_programDirectory += _WIN32 ? "\\" : "/";
#endif
        loginfo(std::format("Program directory: {}", g_programDirectory));
        //g_addNotification(Notification("", g_programDirectory));

        srand(time(NULL));

        loginfo("Library versions:\n" + getAllLibsVersions());

        platformPreInit();

        loginfo("System information:\n" + platformGetSystemInfo());

        g_loadConfig();
        loginfo("Config loaded");

        SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "candidates");
        int canInit = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD);
        //IMG_Init(-1);   //😈time to get evil
        std::string windowTitle = "void" UTF8_DIAMOND "sprite"
#if _DEBUG
            " " UTF8_DIAMOND " DEBUG"
#endif
            ;
        u32 windowFlags =
#if __ANDROID__
            SDL_WINDOW_MAXIMIZED |
#endif
#if _WIN32
            SDL_WINDOW_HIDDEN |
#endif
            SDL_WINDOW_RESIZABLE;
        g_mainWindow = VSPWindow::tryCreateWindow(windowTitle, { g_windowW, g_windowH }, windowFlags);
        g_mainWindow->isMainWindow = true;
        g_mainWindow->thisWindowsTurn();

        g_mainWindow->addToWindowList();

        loginfo("Passed SDL_Init");
        platformInit();
        SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
        SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
        //SDL_SetHint(SDL_HINT_PEN_MOUSE_EVENTS, "0");
        SDL_SetHint(SDL_HINT_PEN_TOUCH_EVENTS, "0");

        g_props = SDL_CreateProperties();

        g_mainWindow->unscaledWindowSize = { g_windowW, g_windowH };
        g_mainWindow->updateViewportScaler();

        if (g_config.customVisualConfigPath != "") {
            if (!g_loadVisualConfig(convertStringOnWin32(g_config.customVisualConfigPath))) {
                logerr("Failed to load custom visual config");
            }
        }
        if (!std::filesystem::exists(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/visualconfigs/sample_config.json"))) {
            serializeVisualConfig(getDefaultVisualConf(), convertStringToUTF8OnWin32(platformEnsureDirAndGetConfigFilePath()) + "/visualconfigs/sample_config.json");
        }

        if (g_config.overrideCursor) {
            std::string cursorPath = pathInProgramDirectory(VOIDSPRITE_ASSETS_SUBDIR "app_cursor.png");
            SDL_Surface* cursorSrf = IMG_Load(cursorPath.c_str());
            if (cursorSrf != NULL) {
                SDL_Cursor* cur = SDL_CreateColorCursor(cursorSrf, 0, 0);
                SDL_SetCursor(cur);
                SDL_FreeSurface(cursorSrf);
            }
        }

        g_createVSPSDK();
        g_loadPlugins();
        g_setupColorModels();
        g_setupIO();
        g_reloadColorMap();

        loginfo("Loading assets");

        g_mainlogo = new ReldTex( [](SDL_Renderer* rd) { return IMGLoadAssetToTexture("mainlogo.png", rd); } );
        g_iconLayerAdd = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_layer_add.png", rd); } );
        g_iconLayerDelete = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_layer_delete.png", rd); } );
        g_iconLayerUp = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_layer_up.png", rd); } );
        g_iconLayerDown = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_layer_down.png", rd); } );
        g_iconLayerDownMerge = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_layer_downmerge.png", rd); } );
        g_iconLayerDuplicate = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_layer_duplicate.png", rd); } );
        g_iconLayerHide = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_layer_hide.png", rd); } );
        g_iconEraser = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_eraser.png", rd); } );
        g_iconBlendMode = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_blendmode.png", rd); } );
        g_iconColorRGB = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_color_rgb.png", rd); } );
        g_iconColorHSV = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_color_hsv.png", rd); } );
        g_iconColorVisual = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_color_visual.png", rd); } );
        g_iconNavbarTabFile = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("tab_file.png", rd); } );
        g_iconNavbarTabEdit = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("tab_edit.png", rd); } );
        g_iconNavbarTabLayer = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("tab_layer.png", rd); } );
        g_iconNavbarTabView = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("tab_view.png", rd); } );
        g_iconComment = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_message.png", rd); } );
        g_iconMenuPxDim = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("menu_pxdim.png", rd); } );
        g_iconMenuSpritesheet = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("menu_sptl.png", rd); } );
        g_iconMenuTemplates = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("menu_templates.png", rd); } );
        g_iconNotifError = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("notif_error.png", rd); } );
        g_iconNotifSuccess = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("notif_success.png", rd); } );
        g_iconNewColor = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_newcolor.png", rd); } );
        g_iconActionBarUndo = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("actionbar_undo.png", rd); } );
        g_iconActionBarRedo = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("actionbar_redo.png", rd); } );
        g_iconActionBarZoomIn = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("actionbar_zoomin.png", rd); } );
        g_iconActionBarZoomOut = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("actionbar_zoomout.png", rd); } );
        g_iconActionBarSave = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("actionbar_save.png", rd); } );
        g_iconFilePickerDirectory = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_filepicker_directory.png", rd); } );
        //SDL_SetTextureColorMod(g_iconFilePickerDirectory, 0xFF, 0xFC, 0x7B);
        g_iconFilePickerFile = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_filepicker_file.png", rd); } );
        //SDL_SetTextureColorMod(g_iconFilePickerFile, 0x80, 0x80, 0x80);
        g_iconFilePickerSupportedFile = new ReldTex([](SDL_Renderer* rd) { return IMGLoadAssetToTexture("icon_filepicker_supportedfile.png", rd); } );

        g_iconNotifTheCreature = new ReldTex([](SDL_Renderer* rd) { 
            SDL_Surface* srf = SDL_CreateSurface(50, 50, SDL_PIXELFORMAT_ARGB8888);
            memcpy(srf->pixels, the_creature, 50 * 50 * 4);
            SDL_Texture* ret = tracked_createTextureFromSurface(g_rd, srf); 
            SDL_FreeSurface(srf);
            return ret;
        });

        g_gamepad = new Gamepad();
        g_gamepad->TryCaptureGamepad();

        loginfo("Loading tools");

        //load brushes
        g_loadBrushes();
        int i = 0;
        for (BaseBrush*& brush : g_brushes) {
            brush->cachedIcon = new ReldTex([brush](SDL_Renderer* rd) { return IMGLoadAssetToTexture(brush->getIconPath()); } );
        }

        loginfo("Loading patterns");

        //load patterns
        g_patterns.push_back(new PatternFull());
        g_patterns.push_back(new PatternGrid());
        g_patterns.push_back(new PatternGridReverse());
        g_patterns.push_back(new PatternDiag2px());
        g_patterns.push_back(new PatternDiag3px());
        g_patterns.push_back(new PatternDiag4px());
        g_patterns.push_back(new PatternDiag2pxReverse());
        g_patterns.push_back(new PatternDiag3pxReverse());
        g_patterns.push_back(new PatternDiag4pxReverse());
        g_patterns.push_back(new PatternHorizontal1px());
        g_patterns.push_back(new PatternHorizontal2px());
        g_patterns.push_back(new PatternHorizontal3px());
        g_patterns.push_back(new PatternHorizontal4px());
        g_patterns.push_back(new PatternVertical1px());
        g_patterns.push_back(new PatternVertical2px());
        g_patterns.push_back(new PatternVertical3px());
        g_patterns.push_back(new PatternVertical4px());
        g_patterns.push_back(new PatternSquares1px());
        g_patterns.push_back(new PatternSquares2px());
        g_patterns.push_back(new PatternSquares3px());
        g_patterns.push_back(new PatternSquares4px());
        g_patterns.push_back(new PatternRandom(2));
        g_patterns.push_back(new PatternRandom(4));
        g_patterns.push_back(new PatternRandom(8));
        g_patterns.push_back(new PatternRandom(16));
        int customPatterns = 0;
        auto customPatternPaths = joinVectors({
            platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("patterns/"), ".pbm"),
            platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("patterns/"), ".xbm")
            });
        for (auto& cpattern : customPatternPaths) {
            CustomPattern* p = CustomPattern::load(cpattern);
            if (p != NULL) {
                g_patterns.push_back(p);
                customPatterns++;
            }
        }
        for (Pattern*& pattern : g_patterns) {
            pattern->tryLoadIcon();
        }

        loginfo("Loading templates");

        // load templates
        g_templates = {
            new TemplatePixelIllustration({256, 144}),
            new TemplatePixelIllustration({426, 240}),
            new TemplatePixelIllustration({640, 360}),
            new TemplatePixelIllustration({853, 480}),
            new TemplateRPG2KCharset(),
            new TemplateRPG2KChipset(),
            new TemplateRPG2KFaceset(),
            new TemplateRPG2KSystem(),
            new TemplateRPG2KBattleAnim(),
            new TemplateMC64x32Skin(),
        };
        int customTemplates = 0;
        auto customTemplatePaths = joinVectors({
            platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("templates/"), ".png"),
            platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("templates/"), ".voidsn")
            });
        for (PlatformNativePathString& t : customTemplatePaths) {
            CustomTemplate* tt = new CustomTemplate(t);
            if (tt != NULL) {
                g_templates.push_back(tt);
                customTemplates++;
            }
        }

        loginfo("Loading 9S patterns");

        // load 9segment patterns
        g_9spatterns = {
            new NineSegmentPattern(nspattern1),
        };
        auto nineSegmentPatternPaths = joinVectors({
            platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("9segmentpatterns/"), ".void9sp")
            });
        int custom9SPatterns = 0;
        for (auto& t : nineSegmentPatternPaths) {
            auto result = read9SegmentPattern(t);
            if (result.first) {
                g_9spatterns.push_back(new NineSegmentPattern(result.second));
                custom9SPatterns++;
            }
        }
        for (NineSegmentPattern*& pattern : g_9spatterns) {
            cacheTexture(*pattern);
        }

        //load filters
        g_loadFilters();

        g_initKeybinds();

        loginfo("Loading fonts");
        //load fonts
        TTF_Init();
        g_reloadFonts();

        /*SDL_Surface* rasterCP437FontImg = IMG_Load(pathInProgramDirectory("assets/codepage437-8x8-voidfont.png").c_str());
        if (rasterCP437FontImg != NULL) {
            BitmapFontObject* fnt = new BitmapFontObject(rasterCP437FontImg, 437);
            g_fnt->AddFont(fnt);
        }*/


        g_ttp = new TooltipsLayer();

        loginfo("Starting launchpad");
        StartScreen* launchpad = new StartScreen();
        g_mainWindow->addScreen(launchpad, g_mainWindow->screenStack.empty());

        //run command line args
        bool closeLaunchpad = false;
        for (std::string& arg : g_cmdlineArgs) {
            if (arg.substr(0, 2) == "--") {
                std::string option = arg.substr(2);
                if (option == "no-launchpad") {
                    closeLaunchpad = true;
                }
            }
            else {
                if (std::filesystem::exists(convertStringOnWin32(arg))) {
                    launchpad->tryLoadFile(arg);
                }
                else {
                    //todo: this notification never fits the whole file name
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), std::format("Could not find file:\n {}", arg)));
                }
            }
        }
        if (closeLaunchpad) {
            g_closeScreen(launchpad);
        }

        if (g_config.useDiscordRPC) {
            g_initRPC();
        }

        platformPostInit();
        loginfo("Init passed");

        //run conversions
        for (auto& c : convertTargets) {
            MainEditor* sn = loadAnyIntoSession(c.first);
            PopupQuickConvert::doQuickConvert(sn, convertStringOnWin32(c.second));
        }

        if (customPatterns > 0) {
            g_addNotification(Notification(std::format("Loaded {} custom patterns", customPatterns), "", 4000, NULL, COLOR_INFO));
        }
        if (customTemplates > 0) {
            g_addNotification(Notification(std::format("Loaded {} custom templates", customTemplates), "", 4000, NULL, COLOR_INFO));
        }
        if (custom9SPatterns > 0) {
            g_addNotification(Notification(std::format("Loaded {} custom 9seg. patterns", custom9SPatterns), "", 4000, NULL, COLOR_INFO));
        }

        auto startupFinish = std::chrono::system_clock::now() - startupTime;
        auto startupDuration = std::chrono::duration_cast<std::chrono::milliseconds>(startupFinish).count();
        loginfo(std::format("Startup took {}ms", startupDuration));

#if __ANDROID__
        //sometimes it really really doesn't feel like maximizing
        SDL_MaximizeWindow(g_wd);
#endif

        uint64_t ticksBegin = SDL_GetTicks64();

        int lastFrameCount = 0;
        int rtFrameCount = 0;
        u64 frameCountTimestamp = 0;

        SDL_Event evt;
        while (!g_windows.empty()) {
            bool firedQuitEventThisFrame = false;
            bool anyPopupsOpen = false;
            for (auto& [id, wd] : g_windows) {
                anyPopupsOpen |= wd->hasPopupsOpen();
            }

            while (SDL_PollEvent(&evt)) {
                VSPWindow* wdTarget = VSPWindow::windowEventTarget(evt);
                if (wdTarget == NULL) {
                    continue;
                }
                wdTarget->thisWindowsTurn();

                evt = scaleScreenPositionsInEvent(evt);
                DrawableManager::processHoverEventInMultiple({ g_currentWindow->overlayWidgets }, evt);

                //events that can fire during bg operation
                switch (evt.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    evt.type = SDL_EVENT_QUIT;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (evt.key.scancode == SDL_SCANCODE_AC_BACK) {
                        evt.type = SDL_EVENT_QUIT;
                    }
                    else if (evt.key.scancode == SDL_SCANCODE_LCTRL) {
                        g_ctrlModifier = true;
                    }
                    else if (evt.key.scancode == SDL_SCANCODE_LSHIFT) {
                        g_shiftModifier = true;
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    if (evt.key.scancode == SDL_SCANCODE_LCTRL) {
                        g_ctrlModifier = false;
                    }
                    else if (evt.key.scancode == SDL_SCANCODE_LSHIFT) {
                        g_shiftModifier = false;
                    }
                    break;
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
#if __ANDROID__
                    if (!SDL_IsDeXMode()) {
                        SDL_MaximizeWindow(g_wd);
                    }
#endif
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    wdTarget->unscaledWindowSize = { evt.window.data1, evt.window.data2 };
#if __ANDROID__
                    wdTarget->autoViewportScale();
#else
                    wdTarget->updateViewportScaler();
#endif
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    if (!lastPenEvent.started || lastPenEvent.elapsedTime() > 100) {
                        wdTarget->mouseX = (int)(evt.motion.x);
                        wdTarget->mouseY = (int)(evt.motion.y);
                    }
                    break;
                case SDL_EVENT_FINGER_DOWN:
                case SDL_EVENT_FINGER_UP:
                case SDL_EVENT_FINGER_MOTION:
                    if (!lastPenEvent.started || lastPenEvent.elapsedTime() > 100) {
                        wdTarget->mouseX = evt.tfinger.x * g_windowW;
                        wdTarget->mouseY = evt.tfinger.y * g_windowH;
                    }
                    break;
                case SDL_EVENT_PEN_MOTION:
                    lastPenEvent.start();
                    wdTarget->mouseX = (int)(evt.pmotion.x);
                    wdTarget->mouseY = (int)(evt.pmotion.y);
                    break;
                case SDL_EVENT_PEN_DOWN:
                case SDL_EVENT_PEN_UP:
                case SDL_EVENT_PEN_BUTTON_DOWN:
                case SDL_EVENT_PEN_BUTTON_UP:
                    lastPenEvent.start();
                    break;
                }

                //ensure only one quit/close event per frame
                if (evt.type == SDL_EVENT_QUIT || evt.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                    if (!firedQuitEventThisFrame) {
                        firedQuitEventThisFrame = true;
                    }
                    else {
                        continue;
                    }
                }

                //focus on the window that has popups open
                if (anyPopupsOpen && !wdTarget->hasPopupsOpen() && evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    for (auto& [id, w] : g_windows) {
                        if (w->hasPopupsOpen()) {
                            SDL_RaiseWindow(w->wd);
                            break;
                        }
                    }
                    
                }

                if (g_bgOpRunning || (anyPopupsOpen && !wdTarget->hasPopupsOpen())) {
                    continue;
                }

                g_keybindManager.processKeybinds(evt, "global", NULL);

                //events that can't fire during bg operation
                /*switch (evt.type) {
                case SDL_QUIT:
                    //return 0;
                    break;
                }*/

                g_gamepad->TakeEvent(evt);
                if (!g_bgOpRunning && !DrawableManager::processInputEventInMultiple({ wdTarget->overlayWidgets }, evt)) {
                    if (!wdTarget->popupStack.empty() && wdTarget->popupStack[wdTarget->popupStack.size() - 1]->takesInput()) {
                        BasePopup* popup = wdTarget->popupStack[wdTarget->popupStack.size() - 1];
                        popup->takeInput(popup->takesTouchEvents() ? evt : convertTouchToMouseEvent(evt));
                    }
                    else {
                        if (!wdTarget->screenStack.empty()) {
                            wdTarget->screenStack[wdTarget->currentScreen]->takeInput(wdTarget->screenStack[wdTarget->currentScreen]->takesTouchEvents() ? evt : convertTouchToMouseEvent(evt));
                        }
                    }
                }
            }

            g_windowFocused = false;
            for (auto& [id, w] : g_windows) {
                g_windowFocused |= ((SDL_GetWindowFlags(w->wd) & SDL_WINDOW_INPUT_FOCUS) != 0);
            }

            for (auto wit = g_windows.cbegin(); wit != g_windows.cend(); ) {
                VSPWindow* wd = (*wit).second;

                //close window if it has no screens left
                if (wd->screenStack.empty()){
                    loginfo(std::format("Closing window ID {}", wd->windowID));
                    if (wd->isMainWindow) {
                        loginfo("   -the main window was closed");
                        g_mainWindow = NULL;
                    }
                    delete wd;
                    g_windows.erase(wit++);
                    continue;
                }
                ++wit;

                wd->thisWindowsTurn();

                if (!wd->popupStack.empty()) {
                    wd->popupStack[wd->popupStack.size() - 1]->tick();
                }
                if (!wd->screenStack.empty()) {
                    wd->screenStack[wd->currentScreen]->tick();
                }

                if (wd->viewport != NULL) {
                    g_pushRenderTarget(wd->viewport);
                }

                SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 255);
                SDL_RenderClear(g_rd);

                if (!wd->screenStack.empty()) {
                    wd->screenStack[wd->currentScreen]->render();
                }

                for (BasePopup*& popup : wd->popupStack) {
                    if (popup != wd->popupStack[wd->popupStack.size() - 1]) {
                        g_ttp->takeTooltips = false;
                        popup->render();
                        g_ttp->takeTooltips = true;
                    }
                    else {
                        popup->render();
                    }
                }

                /*if (!popupStack.empty()) {
                    popupStack[popupStack.size() - 1]->render();
                }*/


    #if _DEBUG
                XY origin = { g_windowW - 240, g_windowH - 90 };
                for (auto& mem : g_named_memmap) {
                    g_fnt->RenderString(std::format("{} | {}", mem.first, bytesToFriendlyString(mem.second)), origin.x,
                        origin.y, { 255, 255, 255, 100 }, 14);
                    origin.y -= 16;
                }
                g_fnt->RenderString(std::format("Textures created: {}", g_allocated_textures), origin.x, origin.y,
                    { 255, 255, 255, 100 }, 14);
                origin.y -= 16;
                //g_fnt->RenderString(std::format("{} FPS", lastFrameCount), origin.x, origin.y, { 255,255,255,100 }, 14);
                //origin.y -= 16;
    #endif

                XY nextStatusBarOrigin = { g_windowW, g_windowH - 26 };

                //draw the screen icons
                //XY screenIcons = { g_windowW, g_windowH - 10 };
                nextStatusBarOrigin = xySubtract(nextStatusBarOrigin, XY{ (int)(26 * wd->screenStack.size()), 0 });
                XY screenIconOrigin = nextStatusBarOrigin;

                for (int x = 0; x < wd->screenStack.size(); x++) {
                    BaseScreen* s = wd->screenStack[x];
                    //this is where screen icons were rendered
                    wd->screenButtons[x]->position = screenIconOrigin;
                    screenIconOrigin.x += 26;
                }
                wd->overlayWidgets.renderAll();
                //todo: make this a uilabel
                if (!wd->screenStack.empty()) {
                    std::string screenName = wd->screenStack[wd->currentScreen]->getName();
                    int statW = g_fnt->StatStringDimensions(screenName, 16).x;
                    XY screenNameOrigin = xySubtract({ g_windowW, g_windowH }, { 10 + statW, 55 });
                    g_fnt->RenderString(wd->screenStack[wd->currentScreen]->getName(), screenNameOrigin.x, screenNameOrigin.y, { 255,255,255,255 }, 16);
                }

                //draw battery icon
                int batteryRectW = 60;
                int batterySeconds, batteryPercent;
                SDL_PowerState powerstate = SDL_GetPowerInfo(&batterySeconds, &batteryPercent);
                if (powerstate != SDL_POWERSTATE_NO_BATTERY && powerstate != SDL_POWERSTATE_UNKNOWN) {
                    XY batteryOrigin = xySubtract(nextStatusBarOrigin, { 120, 0 });
                    XY batteryOriginLow = xyAdd(batteryOrigin, { 0,15 });
                    XY batteryEnd = xyAdd(batteryOrigin, { batteryRectW, 0 });

                    nextStatusBarOrigin = xySubtract(batteryOrigin, { 30,0 });

                    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x30);
                    SDL_Rect batteryRect = { batteryOrigin.x, batteryOrigin.y, batteryRectW, 16 };
                    SDL_RenderDrawRect(g_rd, &batteryRect);

                    SDL_Color primaryColor = powerstate == SDL_POWERSTATE_CHARGING ? SDL_Color{ 253, 255, 146, 0x80 }
                        : powerstate == SDL_POWERSTATE_CHARGED ? SDL_Color{ 78,255,249, 0x80 }
                        : powerstate == SDL_POWERSTATE_ERROR ? SDL_Color{ 255, 40, 40, 0x80 }
                    : SDL_Color{ 255,255,255, 0x80 };

                    SDL_SetRenderDrawColor(g_rd, primaryColor.r, primaryColor.g, primaryColor.b, primaryColor.a);
                    drawLine(batteryOrigin, batteryOriginLow);
                    drawLine(batteryEnd, xyAdd(batteryOriginLow, { batteryRectW, 0 }));
                    XY statPercent = statLineEndpoint(batteryOrigin, xyAdd(batteryOrigin, { batteryRectW, 0 }), batteryPercent / 100.0);
                    XY statPercentLow = { statPercent.x, batteryOriginLow.y };
                    drawLine(statPercent, statPercentLow);
                    drawLine(batteryOrigin, statPercent);
                    drawLine(batteryOriginLow, statPercentLow);
                    g_fnt->RenderString(std::format("{}%", batteryPercent), batteryEnd.x + 5, batteryEnd.y - 5, primaryColor);

                    if (powerstate == SDL_POWERSTATE_CHARGING) {
                        g_fnt->RenderString("+", batteryOrigin.x + 2, batteryOrigin.y - 7, { 253, 255, 146,0x80 });
                    }
                    else if (powerstate == SDL_POWERSTATE_CHARGED) {
                        g_fnt->RenderString(UTF8_DIAMOND, batteryOrigin.x - 18, batteryOrigin.y - 5, { 78,255,249, 0x80 });
                    }
                    else if (powerstate == SDL_POWERSTATE_ERROR) {
                        g_fnt->RenderString("x", batteryOrigin.x + 2, batteryOrigin.y - 7, { 255,60,60,0x80 });
                    }
                }

                if (g_config.showFPS) {
                    XY fpsOrigin = xySubtract(nextStatusBarOrigin, { 70, 0 });
                    nextStatusBarOrigin = fpsOrigin;
                    g_fnt->RenderString(std::format("{} FPS", lastFrameCount), fpsOrigin.x, fpsOrigin.y - 5, { 255,255,255,0x80 }, 16);
                }

                g_tickNotifications();
                g_renderNotifications();

                g_ttp->renderAll();
                g_renderVFX();

                g_cleanUpDoneAsyncThreads();
                if (anyPopupsOpen && !wd->hasPopupsOpen()) {
                    SDL_Rect wdrect = { 0, 0, g_windowW, g_windowH };
                    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
                    SDL_RenderFillRect(g_rd, &wdrect);
                }

                if (g_bgOpRunning) {
                    renderbgOpInProgressScreen();
                }
                else {
                    //draw the mouse position 4x4 square
                    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
                    SDL_Rect temp = { g_mouseX, g_mouseY, 4, 4 };
                    SDL_RenderFillRect(g_rd, &temp);
                }

                //g_fnt->RenderString("voidsprite 19.03.2024", 0, 0, SDL_Color{ 255,255,255,0x30 });


                if (wd->viewport != NULL) {
                    g_popRenderTarget();
                    SDL_RenderCopy(g_rd, wd->viewport, NULL, NULL);
                }

                //g_fnt->RenderString(std::format("Frame time: {}\nFPS: {}", g_deltaTime, round(1.0/g_deltaTime)), 0, 30);

                uint64_t ticksNonRenderEnd = SDL_GetTicks64();
                SDL_RenderPresent(wd->rd);
                if (!g_config.vsync) {
                    SDL_Delay(3);
                }
                if (!g_windowFocused) {
                    if (g_config.powerSaverLevel == 2 || (g_config.powerSaverLevel == 3 && powerstate != SDL_POWERSTATE_NO_BATTERY && batteryPercent <= 15)) {
                        SDL_Delay(500);
                    }
                    else if (g_config.powerSaverLevel == 1 || (g_config.powerSaverLevel == 3 && powerstate != SDL_POWERSTATE_NO_BATTERY)) {
                        SDL_Delay(45);
                    }
                    
                }
                uint64_t ticksEnd = SDL_GetTicks64(); 

                rtFrameCount++;
                u64 frameTimestampNow = SDL_GetTicks64() / 1000;
                if (frameTimestampNow != frameCountTimestamp) {
                    frameCountTimestamp = frameTimestampNow;
                    lastFrameCount = rtFrameCount;
                    rtFrameCount = 0;
                }

                g_deltaTime = ixmax(1, ticksEnd - ticksBegin) / 1000.0;
                g_frameDeltaTime = (ticksNonRenderEnd - ticksBegin) / 1000.0;

                ticksBegin = SDL_GetTicks64();

                if (wd->isMainWindow) {
                    //tl strings shouldn't be read every frame
                    static std::string tl1ActiveWorkspaceString = TL("vsp.rpc.1activeworkspace");
                    static std::string tlActiveWorkspacesString = TL("vsp.rpc.activeworkspaces");

                    g_updateRPC(
                        wd->screenStack.size() == 1 ? tl1ActiveWorkspaceString : std::format("{} {}", wd->screenStack.size(), tlActiveWorkspacesString),
                        wd->screenStack.size() > 0 ? wd->screenStack[wd->currentScreen]->getRPCString() : "-"
                    );
                    std::string newWindowTitle = windowTitle + std::format("   " UTF8_EMPTY_DIAMOND "{}   " UTF8_EMPTY_DIAMOND "{} {}", (wd->screenStack.size() > 0 ? wd->screenStack[wd->currentScreen]->getRPCString() : "--"), wd->screenStack.size(), tlActiveWorkspacesString);
                    if (newWindowTitle != wd->lastWindowTitle) {
                        SDL_SetWindowTitle(g_wd, newWindowTitle.c_str());
                        wd->lastWindowTitle = newWindowTitle;
                    }
                }
            }
        }

        g_waitAndRemoveAllBgOpAndAsyncThreads();

        loginfo("Deinit...");
        platformDeinit();

        g_deinitRPC();
        log_close();

    }
    catch (std::exception& e) {
        logerr("-------------------------------------------");
        logerr("voidsprite crashed with an uncaught exception");
        logerr(std::format("Details: \n {}", e.what()));
        log_close();
        log_duplicateLast();
        std::string errorTitle = std::format("voidsprite: {}", TL("vsp.fatalerror.title"));
        std::string errorMsg = std::format("{}\n   {}", TL("vsp.fatalerror.body"), e.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, errorTitle.c_str(), errorMsg.c_str(), g_wd);
    }
    return 0;
}