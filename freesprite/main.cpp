
#include "globals.h"

#include <SDL3/SDL_main.h>

#include "FontRenderer.h"
#include "maineditor.h"
#include "BaseScreen.h"
#include "StartScreen.h"
#include "BasePopup.h"
#include "BrushFill.h"
#include "Brush1pxLinePathfind.h"
#include "BrushCircle.h"
#include "Pattern.h"
#include "Notification.h"
#include "ToolRectMove.h"
#include "Brush9SegmentRect.h"
#include "Brush1x1ArcX.h"
#include "Brush1x1ArcY.h"
#include "BrushReplaceColor.h"
#include "ToolRectFlip.h"
#include "ToolRectRotate.h"
#include "ToolRectSwap.h"
#include "ToolText.h"
#include "ToolRectIsolate.h"
#include "Gamepad.h"
#include "FileIO.h"
#include "TooltipsLayer.h"
#include "ButtonStartScreenSession.h"
#include "CustomTemplate.h"
#include "ninesegmentpatterns.h"
#include "ToolGuideline.h"
#include "BrushBezierLine.h"
#include "background_operation.h"
#include "discord_rpc.h"
#include "colormodels.h"

#include "TemplateMC64x32Skin.h"
#include "TemplateRPG2KBattleAnim.h"
#include "TemplateRPG2KCharset.h"
#include "TemplateRPG2KChipset.h"
#include "TemplateRPG2KFaceset.h"
#include "TemplateRPG2KSystem.h"

#include "BaseFilter.h"

#include "ee_creature.h"

#include "main.h"

int g_windowW = 1280;
int g_windowH = 720;
XY unscaledWindowSize = {g_windowW, g_windowH};
int renderScale = 1;
SDL_Texture* viewport = NULL;
std::string g_programDirectory = "";

SDL_Window* g_wd;
SDL_Renderer* g_rd;
int g_mouseX = 0, g_mouseY = 0;
TextRenderer* g_fnt;
TooltipsLayer* g_ttp;
Gamepad* g_gamepad = NULL;
std::vector<std::string> g_cmdlineArgs;
bool fullscreen = false;
bool g_ctrlModifier = false;
bool g_shiftModifier = false;


SDL_Texture* g_mainlogo = NULL;
SDL_Texture* g_iconLayerAdd = NULL;
SDL_Texture* g_iconLayerDelete = NULL;
SDL_Texture* g_iconLayerUp = NULL;
SDL_Texture* g_iconLayerDown = NULL;
SDL_Texture* g_iconLayerDownMerge = NULL;
SDL_Texture* g_iconLayerDuplicate = NULL;
SDL_Texture* g_iconEraser = NULL;
SDL_Texture* g_iconBlendMode = NULL;
SDL_Texture* g_iconColorRGB = NULL;
SDL_Texture* g_iconColorHSV = NULL;
SDL_Texture* g_iconColorVisual = NULL;
SDL_Texture* g_iconNavbarTabFile = NULL;
SDL_Texture* g_iconNavbarTabEdit = NULL;
SDL_Texture* g_iconNavbarTabLayer = NULL;
SDL_Texture* g_iconNavbarTabView = NULL;
SDL_Texture* g_iconComment = NULL;
SDL_Texture* g_iconMenuPxDim = NULL;
SDL_Texture* g_iconMenuSpritesheet = NULL;
SDL_Texture* g_iconMenuTemplates = NULL;
SDL_Texture* g_iconNotifTheCreature = NULL;
SDL_Texture* g_iconNotifError = NULL;
SDL_Texture* g_iconNotifSuccess = NULL;
SDL_Texture* g_iconNewColor = NULL;

std::vector<BaseBrush*> g_brushes;
std::vector<Pattern*> g_patterns;
std::vector<BaseTemplate*> g_templates;

std::vector<Notification> g_notifications;

Timer64 lastPenEvent;

void g_addNotification(Notification a) {
    g_notifications.push_back(a);
}
void tickNotifications() {
    for (int x = 0; x < g_notifications.size(); x++) {
        if (g_notifications[x].timer.elapsedTime() > g_notifications[x].duration) {
            g_notifications.erase(g_notifications.begin() + x);
            x--;
        }
    }
}

std::vector<BasePopup*> popupStack;
void g_addPopup(BasePopup* a) {
    popupStack.push_back(a);
    a->startTimer.start();
}
void g_popDisposeLastPopup(bool dispose) {
    if (dispose) {
        delete popupStack[popupStack.size() - 1];
    }
    popupStack.pop_back();
}
void g_closePopup(BasePopup* a) {
    for (int x = 0; x < popupStack.size(); x++) {
        if (popupStack[x] == a) {
            delete popupStack[x];
            popupStack.erase(popupStack.begin() + x);
        }
    }
}


void g_addScreen(BaseScreen* a, bool switchTo) {
    screenStack.push_back(a);
    if (switchTo) {
        g_switchScreen(screenStack.size() - 1);
    }
    ButtonStartScreenSession* screenButton = new ButtonStartScreenSession(screenStack.size() - 1);
    screenButtons.push_back(screenButton);
    overlayWidgets.addDrawable(screenButton);
}
void g_closeScreen(BaseScreen* screen) {
    for (int x = 0; x < screenStack.size(); x++) {
        if (screenStack[x]->isSubscreenOf() == screen) {
            g_closeScreen(screenStack[x]);
            x = 0;
        }
        if (screenStack[x] == screen) {
            delete screenStack[x];
            screenStack.erase(screenStack.begin() + x);
            overlayWidgets.removeDrawable(screenButtons[screenButtons.size() - 1]);
            screenButtons.pop_back();
            if (currentScreen >= screenStack.size()) {
                g_switchScreen(currentScreen - 1);
            }
            x--;
        }
    }
}

//do not use
//what will happen if i do?
void g_closeLastScreen() {
    g_closeScreen(screenStack[screenStack.size()-1]);
    //delete screenStack[screenStack.size() - 1];
    //screenStack.pop_back();
}

void g_switchScreen(int index) {
    if (index >= 0 && index < screenStack.size()) {
        if (index != currentScreen) {
            currentScreen = index;
            screenStack[currentScreen]->onReturnToScreen();
            screenSwitchTimer.start();
        }
        overlayWidgets.forceUnfocus();
    }
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
        g_addNotification(ErrorNotification("Error", "Can't load: " + path));
        return NULL;
    }
    SDL_Texture* ret = tracked_createTextureFromSurface(g_rd, srf);
    SDL_FreeSurface(srf);
    return ret;
}

void UpdateViewportScaler(){
    if (viewport != NULL) {
        tracked_destroyTexture(viewport);
    }
    if (screenPreviewFramebuffer != NULL) {
        tracked_destroyTexture(screenPreviewFramebuffer);
    }
    g_windowW = unscaledWindowSize.x / renderScale;
    g_windowH = unscaledWindowSize.y / renderScale;
    screenPreviewFramebuffer = tracked_createTexture(g_rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, g_windowW, g_windowH);
    SDL_SetTextureScaleMode(screenPreviewFramebuffer, SDL_SCALEMODE_LINEAR);
    if (renderScale == 1) {
        viewport = NULL;
        return;
    }
   
    viewport = tracked_createTexture(g_rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, g_windowW, g_windowH);
}

void renderbgOpInProgressScreen() {

    SDL_Rect r = { 0, 0, g_windowW, g_windowH };
    //SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80 * g_bgOpStartTimer.percentElapsedTime(600));
    //SDL_RenderFillRect(g_rd, &r);

    u32 colFB = PackRGBAtoARGB(0, 0, 0, (u8)(0xd0 * g_bgOpStartTimer.percentElapsedTime(600)));
    u32 colHB = PackRGBAtoARGB(0, 0, 0, (u8)(0x80 * g_bgOpStartTimer.percentElapsedTime(600)));

    renderGradient({ 0,0,g_windowW,g_windowH / 2 }, colFB, colFB, colHB, colHB);
    renderGradient({ 0,g_windowH/2,g_windowW,g_windowH / 2 + 1 }, colHB, colHB, colFB, colFB);

    TooltipsLayer localttp;

    localttp.addTooltip(Tooltip{ {0,g_windowH - 30 }, "Operation in progress. Please wait...", {255,255,255,255}, g_bgOpStartTimer.percentElapsedTime(600) });
    localttp.renderAll();
    //g_fnt->RenderString("Operation in progress...", 5, 10, {255,255,255,(u8)(255* g_bgOpStartTimer.percentElapsedTime(600)) });
}

int main(int argc, char** argv)
{
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
    g_programDirectory = std::string(argv[0]);
    g_programDirectory = g_programDirectory.substr(0, g_programDirectory.find_last_of("/\\"));
    //g_addNotification(Notification("", g_programDirectory));

    srand(time(NULL));

    std::cout << getAllLibsVersions();

    platformPreInit();

    g_loadConfig();

    for (int x = 0; x < SDL_GetNumRenderDrivers() - 1; x++) {
        char* name = (char*)SDL_GetRenderDriver(x);
        if (name != NULL) {
            std::string renderDriverName = name;
            g_availableRenderersNow.push_back(renderDriverName);
            std::cout << "Renderer " << x << ": " << renderDriverName << "\n";
        }
    }
    if (std::find(g_availableRenderersNow.begin(), g_availableRenderersNow.end(), g_config.preferredRenderer) == g_availableRenderersNow.end()) {
        g_config.preferredRenderer = GlobalConfig().preferredRenderer;  //reset to default
    }

    std::string useRenderer = g_config.preferredRenderer;
    std::cout << "Picking renderer: " << useRenderer << "\n";

    SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "composition");
    int canInit = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD);
    //IMG_Init(-1);   //😈time to get evil
    const char* windowTitle = "void" UTF8_DIAMOND "sprite"
#if _DEBUG
        " " UTF8_DIAMOND " DEBUG"
#endif
    ;
    g_wd = SDL_CreateWindow(windowTitle, g_windowW, g_windowH, SDL_WINDOW_RESIZABLE | (_WIN32 ? SDL_WINDOW_HIDDEN : 0));
    g_rd = SDL_CreateRenderer(g_wd, useRenderer.c_str());
    SDL_SetRenderVSync(g_rd, g_config.vsync ? 1 : SDL_RENDERER_VSYNC_DISABLED);
    platformInit();
    SDL_SetRenderDrawBlendMode(g_rd, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
    //SDL_SetHint(SDL_HINT_PEN_MOUSE_EVENTS, "0");
    SDL_SetHint(SDL_HINT_PEN_TOUCH_EVENTS, "0");

    unscaledWindowSize = {g_windowW, g_windowH};
    UpdateViewportScaler();

    SDL_Surface* cursorSrf = IMG_Load(VOIDSPRITE_ASSETS_PATH "assets/app_cursor.png");
    if (cursorSrf != NULL) {
        SDL_Cursor* cur = SDL_CreateColorCursor(cursorSrf, 0, 0);
        SDL_SetCursor(cur);
        SDL_FreeSurface(cursorSrf);
    }
    g_setupColorModels();

    g_setupIO();
    g_loadPalettesToColorMap();
    g_generateColorMap();

    g_mainlogo = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/mainlogo.png");
    g_iconLayerAdd = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_layer_add.png");
    g_iconLayerDelete = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_layer_delete.png");
    g_iconLayerUp = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_layer_up.png");
    g_iconLayerDown = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_layer_down.png");
    g_iconLayerDownMerge = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_layer_downmerge.png");
    g_iconLayerDuplicate = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_layer_duplicate.png");
    g_iconEraser = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_eraser.png");
    g_iconBlendMode = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_blendmode.png");
    g_iconColorRGB = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_color_rgb.png");
    g_iconColorHSV = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_color_hsv.png");
    g_iconColorVisual = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_color_visual.png");
    g_iconNavbarTabFile = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/tab_file.png");
    g_iconNavbarTabEdit = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/tab_edit.png");
    g_iconNavbarTabLayer = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/tab_layer.png");
    g_iconNavbarTabView = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/tab_view.png");
    g_iconComment = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_message.png");
    g_iconMenuPxDim = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/menu_pxdim.png");
    g_iconMenuSpritesheet = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/menu_sptl.png");
    g_iconMenuTemplates = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/menu_templates.png");
    g_iconNotifError = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/notif_error.png");
    g_iconNotifSuccess = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/notif_success.png");
    g_iconNewColor = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/icon_newcolor.png");

    SDL_Surface* srf = SDL_CreateSurface(50, 50, SDL_PIXELFORMAT_ARGB8888);
    memcpy(srf->pixels, the_creature, 50 * 50 * 4);
    g_iconNotifTheCreature = tracked_createTextureFromSurface(g_rd, srf);
    SDL_FreeSurface(srf);
    //SDL_Texture* the_creature = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/kaosekai.png");

    g_gamepad = new Gamepad();
    g_gamepad->TryCaptureGamepad();

    //load brushes
    g_brushes.push_back(new Brush1x1());
    g_brushes.push_back(new Brush1x1PixelPerfect());
    g_brushes.push_back(new Brush1x1Burst());
    g_brushes.push_back(new Brush1x1ArcX());
    g_brushes.push_back(new Brush1x1ArcY());
    g_brushes.push_back(new Brush3pxCircle());
    g_brushes.push_back(new Brush1pxLine());
    g_brushes.push_back(new Brush1pxLinePathfind());
    g_brushes.push_back(new BrushBezierLine());
    g_brushes.push_back(new BrushRect());
    g_brushes.push_back(new BrushRectFill());
    g_brushes.push_back(new Brush9SegmentRect());
    g_brushes.push_back(new BrushCircle());
    g_brushes.push_back(new BrushCircleArc());
    g_brushes.push_back(new BrushFill());
    g_brushes.push_back(new BrushReplaceColor());
    g_brushes.push_back(new ToolColorPicker());
    g_brushes.push_back(new ToolRectIsolate());
    g_brushes.push_back(new ToolRectClone());
    g_brushes.push_back(new ToolRectMove());
    g_brushes.push_back(new ToolRectSwap());
    g_brushes.push_back(new ToolRectFlip());
    g_brushes.push_back(new ToolRectRotate());
    g_brushes.push_back(new ToolComment());
    g_brushes.push_back(new ToolGuideline());
    g_brushes.push_back(new ToolSetXSymmetry());
    g_brushes.push_back(new ToolSetYSymmetry());
    g_brushes.push_back(new ToolMeasure());
    g_brushes.push_back(new ToolText());
    int i = 0;
    for (BaseBrush*& brush : g_brushes) {
        brush->cachedIcon = IMGLoadToTexture(brush->getIconPath());
        std::string keybindKey = std::format("brush:{}", i++);
        if (g_config.keybinds.contains(keybindKey)) {
            brush->keybind = (SDL_Scancode)g_config.keybinds[keybindKey];
        }
    }

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

    // load templates
    g_templates = {
        new TemplateRPG2KCharset(),
        new TemplateRPG2KChipset(),
        new TemplateRPG2KFaceset(),
        new TemplateRPG2KSystem(),
        new TemplateRPG2KBattleAnim(),
        new TemplateMC64x32Skin()
    };
    int customTemplates = 0;
    auto customTemplatePaths = joinVectors({
        platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("templates/"), ".png"),
        platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("templates/"), ".voidsn")
        });
    for (auto& t : customTemplatePaths) {
        CustomTemplate* tt = CustomTemplate::tryLoad(t);
        if (tt != NULL) {
            g_templates.push_back(tt);
            customTemplates++;
        }
    }

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

    TTF_Init();
    g_fnt = new TextRenderer();

    g_ttp = new TooltipsLayer();

    StartScreen* launchpad = new StartScreen();
    g_addScreen(launchpad, screenStack.empty());

    if (g_config.useDiscordRPC) {
        g_initRPC();
    }

    platformPostInit();

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

    SDL_Event evt;
    while (!screenStack.empty()) {
        uint64_t ticksBegin = SDL_GetTicks64();
        while (SDL_PollEvent(&evt)) {
            DrawableManager::processHoverEventInMultiple({ overlayWidgets }, evt);

            //events that can fire during bg operation
            switch (evt.type) {
                case SDL_EVENT_KEY_DOWN:
                    if (evt.key.scancode == SDL_SCANCODE_LCTRL) {
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
                case SDL_EVENT_WINDOW_RESIZED:
                    g_windowW = evt.window.data1;
                    g_windowH = evt.window.data2;
                    unscaledWindowSize = { g_windowW, g_windowH };
                    UpdateViewportScaler();
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    if (!lastPenEvent.started || lastPenEvent.elapsedTime() > 100) {
                        g_mouseX = (int)(evt.motion.x);
                        g_mouseY = (int)(evt.motion.y);
                    }
                    break;
                case SDL_EVENT_PEN_MOTION:
                    lastPenEvent.start();
                    g_mouseX = (int)(evt.pmotion.x);
                    g_mouseY = (int)(evt.pmotion.y);
                    break;
                case SDL_EVENT_PEN_DOWN:
                case SDL_EVENT_PEN_UP:
                case SDL_EVENT_PEN_BUTTON_DOWN:
                case SDL_EVENT_PEN_BUTTON_UP:
                    lastPenEvent.start();
                    break;
            }

            if (g_bgOpRunning) {
                continue;
            }

            //events that can't fire during bg operation
            switch (evt.type) {
                case SDL_QUIT:
                    //return 0;
                    break;
                case SDL_KEYDOWN:
                    if (evt.key.scancode == SDL_SCANCODE_LEFTBRACKET) {
                        if (currentScreen != 0) {
                            if (g_ctrlModifier) {
                                g_switchScreen(0);
                            }
                            else {
                                g_switchScreen(currentScreen - 1);
                            }
                        }
                    }
                    else if (evt.key.scancode == SDL_SCANCODE_RIGHTBRACKET) {
                        if (currentScreen < screenStack.size() - 1) {
                            if (g_ctrlModifier) {
                                g_switchScreen(screenStack.size() - 1);
                            }
                            else {
                                g_switchScreen(currentScreen + 1);
                            }
                        }
                    }
                    else if (evt.key.scancode == SDL_SCANCODE_W) {
                        if (g_ctrlModifier) {
                            if (g_shiftModifier) {
                                if (favourite && fav_screen < screenStack.size()) {
                                    g_switchScreen(fav_screen);
                                }
                            }
                            else {
                                fav_screen = currentScreen;
                                favourite = !favourite;
                                //screenSwitchTimer.start();
                            }
                        }
                    }
                    else if (evt.key.scancode == SDL_SCANCODE_F11) {
                        fullscreen = !fullscreen;
                        SDL_SetWindowFullscreen(g_wd, fullscreen);
                        screenSwitchTimer.start();
                    }
                    else if (evt.key.scancode == SDL_SCANCODE_EQUALS){
                        if (g_ctrlModifier) {
                            renderScale++;
                            UpdateViewportScaler();
                        }
                    }
                    else if (evt.key.scancode == SDL_SCANCODE_MINUS){
                        if (g_ctrlModifier){
                            if (renderScale-- <= 1){
                                renderScale = 1;
                            }
                            UpdateViewportScaler();

                        }
                    }
                    break;
            }
            g_gamepad->TakeEvent(evt);
            if (!g_bgOpRunning && !DrawableManager::processInputEventInMultiple({ overlayWidgets }, evt)) {
                if (!popupStack.empty() && popupStack[popupStack.size() - 1]->takesInput()) {
                    BasePopup* popup = popupStack[popupStack.size() - 1];
                    popup->takeInput(popup->takesTouchEvents() ? evt : convertTouchToMouseEvent(evt));
                }
                else {
                    if (!screenStack.empty()) {
                        screenStack[currentScreen]->takeInput(screenStack[currentScreen]->takesTouchEvents() ? evt : convertTouchToMouseEvent(evt));
                    }
                }
            }
        }

        g_windowFocused = (SDL_GetWindowFlags(g_wd) & SDL_WINDOW_INPUT_FOCUS) != 0;

        if (!popupStack.empty()) {
            popupStack[popupStack.size() - 1]->tick();
        }
        if (!screenStack.empty()) {
            screenStack[currentScreen]->tick();
        }

        if (viewport != NULL){
            g_pushRenderTarget(viewport);
        }

        SDL_SetRenderDrawColor(g_rd, 0,0,0,255);
        SDL_RenderClear(g_rd);

        if (!screenStack.empty()) {
            screenStack[currentScreen]->render();
        }

        for (BasePopup*& popup : popupStack) {
            popup->render();
        }

        //screen switching animation
        if (screenSwitchTimer.started) {
            double animTimer = XM1PW3P1(screenSwitchTimer.percentElapsedTime(800));
            if (animTimer < 1) {
                double reverseAnimTimer = 1.0 - animTimer;
                XY windowOffset = { g_windowW / 16 , g_windowH / 16 };
                SDL_Rect rect = {
                    windowOffset.x * reverseAnimTimer,
                    windowOffset.y * reverseAnimTimer,
                    g_windowW - 2 * windowOffset.x * reverseAnimTimer,
                    g_windowH - 2 * windowOffset.y * reverseAnimTimer,
                };
                SDL_SetRenderDrawColor(g_rd, 255, 255, 255,  0xd0 * reverseAnimTimer);
                SDL_RenderDrawRect(g_rd, &rect);
            }
        }

        /*if (!popupStack.empty()) {
            popupStack[popupStack.size() - 1]->render();
        }*/


#if _DEBUG
        XY origin = {g_windowW - 240, g_windowH - 90};
        for (auto& mem : g_named_memmap) {
            g_fnt->RenderString(std::format("{} | {}", mem.first, bytesToFriendlyString(mem.second)), origin.x,
                                origin.y, {255, 255, 255, 100});
            origin.y -= 20;
        }
        g_fnt->RenderString(std::format("Textures created: {}", g_allocated_textures), origin.x, origin.y,
                            {255, 255, 255, 100});
        origin.y -= 20;
#endif

        //draw the screen icons
        XY screenIcons = {g_windowW, g_windowH - 10};
        screenIcons = xySubtract(screenIcons, XY{ (int)(26 * screenStack.size()), 16});
        XY screenIconOrigin = screenIcons;
        
        for (int x = 0; x < screenStack.size(); x++) {
            BaseScreen* s = screenStack[x];
            //this is where screen icons were rendered
            screenButtons[x]->position = screenIcons;
            screenIcons.x += 26;
        }
        overlayWidgets.renderAll();
        //todo: make this a uilabel
        if (!screenStack.empty()) {
            std::string screenName = screenStack[currentScreen]->getName();
            int statW = g_fnt->StatStringDimensions(screenName).x;
            g_fnt->RenderString(screenStack[currentScreen]->getName(), g_windowW - ixmax(200, 10 + statW), g_windowH - 55);
        }

        //draw battery icon
        int batteryRectW = 60;
        int batterySeconds, batteryPercent;
        SDL_PowerState powerstate = SDL_GetPowerInfo(&batterySeconds, &batteryPercent);
        if (powerstate != SDL_POWERSTATE_NO_BATTERY && powerstate != SDL_POWERSTATE_UNKNOWN) {
            XY batteryOrigin = xySubtract(screenIconOrigin, { 120, 0 });
            XY batteryOriginLow = xyAdd(batteryOrigin, { 0,16 });
            XY batteryEnd = xyAdd(batteryOrigin, { batteryRectW, 0 });

            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x30);
            SDL_Rect batteryRect = { batteryOrigin.x, batteryOrigin.y, batteryRectW, 16 };
            SDL_RenderDrawRect(g_rd, &batteryRect);

            SDL_Color primaryColor = powerstate == SDL_POWERSTATE_CHARGING ? SDL_Color{253, 255, 146, 0x80}
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
                g_fnt->RenderString("+", batteryOrigin.x + 2, batteryOrigin.y - 7, { 253, 255, 146,0x80});
            }
            else if (powerstate == SDL_POWERSTATE_CHARGED) {
                g_fnt->RenderString(UTF8_DIAMOND, batteryOrigin.x - 18, batteryOrigin.y - 5, { 78,255,249, 0x80 });
            }
            else if (powerstate == SDL_POWERSTATE_ERROR) {
                g_fnt->RenderString("x", batteryOrigin.x + 2, batteryOrigin.y - 7, { 255,60,60,0x80 });
            }
        }
        
        //render notifications
        tickNotifications();
        int notifY = 30;
        int notifOriginX = g_windowW - 450;
        for (Notification& notif : g_notifications) {
            //background
            int notifX = notifOriginX + 30 * (1.0 - XM1PW3P1(notif.timer.percentElapsedTime(300)));
            SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0xd0 * XM1PW3P1(notif.timer.percentElapsedTime(200) * (1.0-notif.timer.percentElapsedTime(500, notif.duration-500)))));
            SDL_Rect r = { notifX, notifY, 400, 60 };
            SDL_RenderFillRect(g_rd, &r);

            //animated border lines
            //gradient
            uint32_t color = sdlcolorToUint32(notif.color);
            XY leftEP = statLineEndpoint(XY{ r.x, r.y }, XY{ r.x, r.y + r.h }, XM1PW3P1(notif.timer.percentElapsedTime(300)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)));
            renderGradient({ r.x, r.y, r.w / 4, leftEP.y - r.y }, modAlpha(color, 0x40), modAlpha(color, 0), modAlpha(color,0x40), modAlpha(color,0));

            SDL_SetRenderDrawColor(g_rd, notif.color.r, notif.color.g, notif.color.b, 0x80 + (uint8_t)(0x60 * (1.0 - XM1PW3P1(notif.timer.percentElapsedTime(500)))));
            //left line
            drawLine(XY{r.x, r.y}, leftEP);
            //right line
            drawLine(XY{ r.x + r.w, r.y + r.h }, XY{r.x+r.w, r.y}, XM1PW3P1(notif.timer.percentElapsedTime(300))* (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)));

            //icon
            int textX = notifX + 10;
            if (notif.icon != NULL) {
                SDL_Rect iconRect = { notifX + 5, notifY + 5, 50, 50 };
                SDL_SetTextureAlphaMod(notif.icon, (uint8_t)(0xff * XM1PW3P1(notif.timer.percentElapsedTime(200, 200)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500))));
                SDL_RenderCopy(g_rd, notif.icon, NULL, &iconRect);
                SDL_SetTextureAlphaMod(notif.icon, 0xff);
                textX += 50;
            }

            //text
            g_fnt->RenderString(notif.title, textX, notif.message != "" ? notifY + 5 : notifY + 15, SDL_Color{255,255,255,(uint8_t)(0xff * XM1PW3P1(notif.timer.percentElapsedTime(200, 100)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)))});
            g_fnt->RenderString(notif.message, textX, notif.title != "" ? notifY + 30 : notifY + 15, SDL_Color{255,255,255,(uint8_t)(0xd0 * XM1PW3P1(notif.timer.percentElapsedTime(200, 150)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)))});
            notifY += 65;
        }

        g_ttp->renderAll();

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


        if (viewport != NULL){
            g_popRenderTarget();
            SDL_RenderCopy(g_rd, viewport, NULL, NULL);
        }

        //g_fnt->RenderString(std::format("Frame time: {}\nFPS: {}", g_deltaTime, round(1.0/g_deltaTime)), 0, 30);

        uint64_t ticksNonRenderEnd = SDL_GetTicks64();
        SDL_RenderPresent(g_rd);
        if (!g_config.vsync) {
            SDL_Delay(3);
        }
        if (!g_windowFocused) {
            SDL_Delay(45);
        }
        uint64_t ticksEnd = SDL_GetTicks64();

        g_deltaTime = ixmax(1, ticksEnd - ticksBegin) / 1000.0;
        g_frameDeltaTime = (ticksNonRenderEnd - ticksBegin) / 1000.0;

        g_updateRPC(
            screenStack.size() == 1 ? "1 active workspace" : std::format("{} active workspaces", screenStack.size()),
            screenStack.size() > 0 ? screenStack[currentScreen]->getRPCString() : "-"
        );
    }

    if (threadSet) {
        g_bgOpThread.join();
    }

    g_deinitRPC();

    return 0;
}
