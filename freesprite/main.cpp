
#include "globals.h"
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


SDL_Texture* IMGLoadToTexture(std::string path) {
    SDL_Surface* srf = IMG_Load(pathInProgramDirectory(path).c_str());
    srf = srf == NULL ? IMG_Load(path.c_str()) : srf;
    if (srf == NULL) {
        g_addNotification(ErrorNotification("Error", "Can't load: " + path));
        return NULL;
    }
    SDL_Texture* ret = SDL_CreateTextureFromSurface(g_rd, srf);
    SDL_FreeSurface(srf);
    return ret;
}

void UpdateViewportScaler(){
    if (viewport != NULL) {
        SDL_DestroyTexture(viewport);
    }
    g_windowW = unscaledWindowSize.x / renderScale;
    g_windowH = unscaledWindowSize.y / renderScale;
    if (renderScale == 1) {
        viewport = NULL;
        return;
    }
   
    viewport = SDL_CreateTexture(g_rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, g_windowW, g_windowH);
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

    platformPreInit();
    int canInit = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
#if SDL_IMAGE_MINOR_VERSION <= 5
#define IMG_INIT_AVIF 0
#define IMG_INIT_JXL 0
#endif
    IMG_Init(IMG_INIT_AVIF | IMG_INIT_JPG | IMG_INIT_JXL | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
    g_wd = SDL_CreateWindow("void\xE2\x97\x86sprite", 50, 50, g_windowW, g_windowH, SDL_WINDOW_RESIZABLE | (_WIN32 ? SDL_WINDOW_HIDDEN : 0));
    g_rd = SDL_CreateRenderer(g_wd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    platformInit();
    SDL_SetRenderDrawBlendMode(g_rd, SDL_BLENDMODE_BLEND);

    SDL_Surface* cursorSrf = IMG_Load(VOIDSPRITE_ASSETS_PATH "assets/app_cursor.png");
    if (cursorSrf != NULL) {
        SDL_Cursor* cur = SDL_CreateColorCursor(cursorSrf, 0, 0);
        SDL_SetCursor(cur);
        SDL_FreeSurface(cursorSrf);
    }

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

    SDL_Surface* srf = SDL_CreateRGBSurfaceWithFormat(0, 50, 50, 32, SDL_PIXELFORMAT_ARGB8888);
    memcpy(srf->pixels, the_creature, 50 * 50 * 4);
    g_iconNotifTheCreature = SDL_CreateTextureFromSurface(g_rd, srf);
    SDL_FreeSurface(srf);
    //SDL_Texture* the_creature = IMGLoadToTexture(VOIDSPRITE_ASSETS_PATH "assets/kaosekai.png");

    g_gamepad = new Gamepad();
    g_gamepad->TryCaptureGamepad();

    g_setupIO();

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
    for (BaseBrush*& brush : g_brushes) {
        brush->cachedIcon = IMGLoadToTexture(brush->getIconPath());
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

    g_loadConfig();

    TTF_Init();
    g_fnt = new TextRenderer();

    g_ttp = new TooltipsLayer();

    StartScreen* launchpad = new StartScreen();
    g_addScreen(launchpad, screenStack.empty());

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

            switch (evt.type) {
                case SDL_QUIT:
                    //return 0;
                    break;
                case SDL_KEYDOWN:
                    if (evt.key.keysym.sym == SDLK_LEFTBRACKET) {
                        if (currentScreen != 0) {
                            if (g_ctrlModifier) {
                                g_switchScreen(0);
                            }
                            else {
                                g_switchScreen(currentScreen - 1);
                            }
                        }
                    }
                    else if (evt.key.keysym.sym == SDLK_RIGHTBRACKET) {
                        if (currentScreen < screenStack.size() - 1) {
                            if (g_ctrlModifier) {
                                g_switchScreen(screenStack.size() - 1);
                            }
                            else {
                                g_switchScreen(currentScreen + 1);
                            }
                        }
                    }
                    else if (evt.key.keysym.sym == SDLK_w) {
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
                    else if (evt.key.keysym.sym == SDLK_F11) {
                        fullscreen = !fullscreen;
                        SDL_SetWindowFullscreen(g_wd, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                        screenSwitchTimer.start();
                    }
                    else if (evt.key.keysym.sym == SDLK_LCTRL){
                        g_ctrlModifier = true;
                    }
                    else if (evt.key.keysym.sym == SDLK_LSHIFT){
                        g_shiftModifier = true;
                    }
                    else if (evt.key.keysym.sym == SDLK_EQUALS){
                        if (g_ctrlModifier) {
                            renderScale++;
                            UpdateViewportScaler();
                        }
                    }
                    else if (evt.key.keysym.sym == SDLK_MINUS){
                        if (g_ctrlModifier){
                            if (renderScale-- <= 1){
                                renderScale = 1;
                            }
                            UpdateViewportScaler();

                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (evt.key.keysym.sym == SDLK_LCTRL){
                        g_ctrlModifier = false;
                    }
                    else if (evt.key.keysym.sym == SDLK_LSHIFT) {
                        g_shiftModifier = false;
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (evt.window.event == SDL_WINDOWEVENT_RESIZED) {
                        g_windowW = evt.window.data1;
                        g_windowH = evt.window.data2;
                        unscaledWindowSize = { g_windowW, g_windowH };
                        UpdateViewportScaler();
                    }
                    break;
                case SDL_MOUSEMOTION:
                    g_mouseX = evt.motion.x;
                    g_mouseY = evt.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    //g_addNotification(Notification("WARNING", "<- the creature", 5000, the_creature));
                    break;
            }
            g_gamepad->TakeEvent(evt);
            if (!DrawableManager::processInputEventInMultiple({ overlayWidgets }, evt)) {
                if (!popupStack.empty() && popupStack[popupStack.size() - 1]->takesInput()) {
                    popupStack[popupStack.size() - 1]->takeInput(evt);
                }
                else {
                    if (!screenStack.empty()) {
                        screenStack[currentScreen]->takeInput(evt);
                    }
                }
            }
        }

        if (!popupStack.empty()) {
            popupStack[popupStack.size() - 1]->tick();
        }
        if (!screenStack.empty()) {
            screenStack[currentScreen]->tick();
        }

        if (viewport != NULL){
            SDL_SetRenderTarget(g_rd, viewport);
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


        //draw the screen icons
        XY screenIcons = {g_windowW, g_windowH - 10};
        screenIcons = xySubtract(screenIcons, XY{ (int)(26 * screenStack.size()), 16});
        
        for (int x = 0; x < screenStack.size(); x++) {
            BaseScreen* s = screenStack[x];
            //this is where screen icons were rendered
            screenButtons[x]->position = screenIcons;
            screenIcons.x += 26;
        }
        overlayWidgets.renderAll();
        //todo: make this a uilabel
        if (!screenStack.empty()) {
            g_fnt->RenderString(screenStack[currentScreen]->getName(), g_windowW - 200, g_windowH - 52);
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

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
        SDL_Rect temp = {g_mouseX, g_mouseY, 4, 4};
        SDL_RenderFillRect(g_rd, &temp);

        //g_fnt->RenderString("voidsprite 19.03.2024", 0, 0, SDL_Color{ 255,255,255,0x30 });


        if (viewport != NULL){
            SDL_SetRenderTarget(g_rd, NULL);
            SDL_RenderCopy(g_rd, viewport, NULL, NULL);
        }

        SDL_RenderPresent(g_rd);
        uint64_t ticksEnd = SDL_GetTicks64();

        g_deltaTime = dxmax(0.001, ticksEnd - ticksBegin);
    }

    return 0;
}
