#pragma once

#include <SDL3_ttf/SDL_ttf.h>

#include "globals.h"
#include "Timer64.h"
#include "Notification.h"
#include "main.h"
#include "DrawableManager.h"
#include "ButtonStartScreenSession.h"
#include "BaseScreen.h"
#include "BasePopup.h"
#include "FontRenderer.h"

class VSPWindow {
public:
    SDL_WindowID windowID = -1;
    SDL_Window* wd = NULL;
    SDL_Renderer* rd = NULL;
    std::vector<BaseScreen*> screenStack;
    std::vector<BasePopup*> popupStack;
    int currentScreen = 0;
    Timer64 screenSwitchTimer;
    XY unscaledWindowSize;
    XY scaledWindowSize;
    int renderScale = 1;
    SDL_Texture* screenPreviewFB = NULL;
    SDL_Texture* viewport = NULL;
    TextRenderer* fnt = NULL;
    std::string lastWindowTitle = "";

    int mouseX = 0, mouseY = 0;

    DrawableManager overlayWidgets;
    std::vector<ButtonStartScreenSession*> screenButtons;

    bool isMainWindow = false;

    VSPWindow(std::string title, XY size, u32 flags) {
        wd = SDL_CreateWindow(title.c_str(), size.x, size.y, flags);
        windowID = SDL_GetWindowID(wd);
        tryCreateRenderer();
        setVsync(g_config.vsync);
        unscaledWindowSize = scaledWindowSize = size;
    }
    ~VSPWindow() {
        if (rd != NULL) {
            SDL_DestroyRenderer(rd);
        }
        if (wd != NULL) {
            SDL_DestroyWindow(wd);
        }
        if (screenPreviewFB != NULL) {
            tracked_destroyTexture(screenPreviewFB);
        }
        if (viewport != NULL) {
            tracked_destroyTexture(viewport);
        }
        for (BaseScreen* screen : screenStack) {
            delete screen;
        }
        for (BasePopup* popup : popupStack) {
            delete popup;
        }

    }

    static VSPWindow* tryCreateWindow(std::string title, XY size, u32 flags) {
        VSPWindow* ret = new VSPWindow(title, size, flags);
        if (ret->wd == NULL || ret->rd == NULL) {
            logerr(std::format("Failed to create window: {}\n  {}", title, SDL_GetError()));
            delete ret;
            return NULL;
        }
        ret->renderScale = g_renderScale;
        return ret;
    }

    static VSPWindow* windowEventTarget(SDL_Event evt) {
        SDL_Window* windowFromEvt = SDL_GetWindowFromEvent(&evt);
        if (windowFromEvt == NULL) {
            return g_mainWindow;
        } else {
            SDL_WindowID id = SDL_GetWindowID(windowFromEvt);
            return g_windows.contains(id) ? g_windows[id] : g_mainWindow;
        }
    }

    void initFonts() {
        if (fnt != NULL) {
            delete fnt;
        }
        fnt = new TextRenderer();
        TTF_Font* fontDefault = TTF_OpenFont(pathInProgramDirectory(FONT_PATH).c_str(), 18);
        fontDefault = fontDefault == NULL ? TTF_OpenFont(FONT_PATH, 18) : fontDefault;
        if (fontDefault != NULL) {
            fnt->AddFont(fontDefault, { {0, 0xFFFFFFFF} });
        }
        else {
            logerr("Failed to load the default font");
        }

        TTF_Font* fontJP = TTF_OpenFont(pathInProgramDirectory(FONT_PATH_JP).c_str(), 18);
        fontJP = fontJP == NULL ? TTF_OpenFont(FONT_PATH_JP, 18) : fontJP;
        if (fontJP != NULL) {
            fnt->AddFont(fontJP, {
                {0x3000, 0x30FF},   // CJK Symbols and Punctuation, hiragana, katakana
                {0xFF00, 0xFFEF},   // Halfwidth and Fullwidth Forms
                {0x4E00, 0x9FAF}    // CJK Unified Ideographs
                });
        }

        std::string customFont = visualConfigValue("general/font");
        if (!g_usingDefaultVisualConfig()) {
            PlatformNativePathString vcRoot = g_getCustomVisualConfigRoot();
            std::string customFontPath = convertStringToUTF8OnWin32(vcRoot) + "/" + customFont;
            if (std::filesystem::exists(vcRoot + convertStringOnWin32("/" + customFont))) {
                TTF_Font* fontCustom = TTF_OpenFont(customFontPath.c_str(), 18);
                if (fontCustom != NULL) {
                    fnt->AddFont(fontCustom, { {0, 0xFFFFFFFF} });
                }
            }
        }

        fnt->precacheFontCommonChars(18);
        /*std::string customFontPath = convertStringToUTF8OnWin32(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/appfont.ttf"));
        if (std::filesystem::exists(customFontPath)) {
            TTF_Font* fontCustom = TTF_OpenFont(customFontPath.c_str(), 18);
            if (fontCustom != NULL) {
                g_fnt->AddFont(fontCustom, { {0, 0xFFFFFFFF} });
            }
        }*/

        if (g_currentWindow == this) {
            //so that g_fnt reloads
            thisWindowsTurn();
        }
    }

    void addToWindowList() {
        g_windows[windowID] = this;
    }

    void thisWindowsTurn() {
        g_currentWindow = this;
        g_rd = rd;
        g_wd = wd;
        g_windowW = scaledWindowSize.x;
        g_windowH = scaledWindowSize.y;
        g_mouseX = mouseX;
        g_mouseY = mouseY;
        g_fnt = fnt;
        screenPreviewFramebuffer = screenPreviewFB;
    }

    void setVsync(bool vsync) {
        if (rd != NULL) {
            SDL_SetRenderVSync(rd, vsync ? 1 : SDL_RENDERER_VSYNC_DISABLED);
        }
    }

    bool tryCreateRenderer() {
        g_availableRenderersNow.clear();
        for (int x = 0; x < SDL_GetNumRenderDrivers() - 1; x++) {
            char* name = (char*)SDL_GetRenderDriver(x);
            if (name != NULL) {
                std::string renderDriverName = name;
                g_availableRenderersNow.push_back(renderDriverName);
                loginfo(std::format("Renderer {}: {}", x, renderDriverName));
            }
        }
        if (std::find(g_availableRenderersNow.begin(), g_availableRenderersNow.end(), g_config.preferredRenderer) == g_availableRenderersNow.end()) {
            g_config.preferredRenderer = GlobalConfig().preferredRenderer;  //reset to default
        }
        std::string useRenderer = g_config.preferredRenderer;
        loginfo(std::format("Picking renderer: {}", useRenderer));
        rd = SDL_CreateRenderer(wd, useRenderer.c_str());
        while (rd == NULL) {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), std::format("Renderer failed: {}", useRenderer)));
            logerr(std::format("Failed to create renderer: {}\n  {}", useRenderer, SDL_GetError()));
            g_availableRenderersNow.erase(std::remove(g_availableRenderersNow.begin(), g_availableRenderersNow.end(), useRenderer), g_availableRenderersNow.end());
            if (g_availableRenderersNow.empty()) {
                logerr("No renderers available");
                std::string errorTitle = std::format("voidsprite: {}", TL("vsp.error.norenderer.title"));
                std::string errorMsg = TL("vsp.error.norenderer.body");
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, errorTitle.c_str(), errorMsg.c_str(), g_wd);
                return false;
            }
            else {
                useRenderer = g_availableRenderersNow[0];
                g_config.preferredRenderer = useRenderer;
                loginfo(std::format("Trying renderer: {}", useRenderer));
                rd = SDL_CreateRenderer(g_wd, useRenderer.c_str());
            }
        }
        SDL_SetRenderDrawBlendMode(rd, SDL_BLENDMODE_BLEND);
        return true;
    }

    void updateViewportScaler() {
        if (viewport != NULL) {
            tracked_destroyTexture(viewport);
        }
        if (screenPreviewFB != NULL) {
            tracked_destroyTexture(screenPreviewFB);
        }
        scaledWindowSize.x = unscaledWindowSize.x / g_renderScale;
        scaledWindowSize.y = unscaledWindowSize.y / g_renderScale;
        screenPreviewFB = tracked_createTexture(rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, scaledWindowSize.x, scaledWindowSize.y);
        SDL_SetTextureScaleMode(screenPreviewFB, SDL_SCALEMODE_LINEAR);
        if (g_renderScale == 1) {
            viewport = NULL;
        }
        else {
            viewport = tracked_createTexture(rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, scaledWindowSize.x, scaledWindowSize.y);
        }

        if (g_currentWindow == this) {
            thisWindowsTurn();
        }
    }

    void autoViewportScale() {
        int newViewportScale = 1;
        if (g_windowW > g_windowH) {
            newViewportScale = unscaledWindowSize.y < (720 * 1.5) ? 1
                    : ixmax(2, unscaledWindowSize.y / 720);
        }
        else {
            newViewportScale = unscaledWindowSize.x < (1280 * 1.5) ? 1
                    : ixmax(2, unscaledWindowSize.x / 1280);
        }
        g_renderScale = newViewportScale;
        updateViewportScaler();
    }

    void addScreen(BaseScreen* a, bool switchTo) {
        screenStack.push_back(a);
        if (switchTo) {
            g_switchScreen(screenStack.size() - 1);
        }
        ButtonStartScreenSession* screenButton = new ButtonStartScreenSession(this, screenStack.size() - 1);
        screenButtons.push_back(screenButton);
        overlayWidgets.addDrawable(screenButton);
    }

    bool closeScreen(BaseScreen* screen) {
        for (int x = 0; x < screenStack.size(); x++) {
            if (screenStack[x]->isSubscreenOf() == screen) {
                g_closeScreen(screenStack[x]);
                x = 0;
            }
            if (screenStack[x] == screen) {
                delete screenStack[x];
                if (x == currentScreen) {
                    g_newVFX(VFX_SCREENCLOSE, 500);
                }
                screenStack.erase(screenStack.begin() + x);
                overlayWidgets.removeDrawable(screenButtons[screenButtons.size() - 1]);
                screenButtons.pop_back();
                if (currentScreen >= screenStack.size()) {
                    g_switchScreen(currentScreen - 1);
                }
                x--;
                //return true;
            }
        }
        return false;
    }

    void switchScreen(int index) {
        if (index >= 0 && index < screenStack.size()) {
            if (index != currentScreen) {
                currentScreen = index;
                screenStack[currentScreen]->onReturnToScreen();
                g_newVFX(VFX_SCREENSWITCH, 800);
            }
            overlayWidgets.forceUnfocus();
        }
    }

    void switchScreenLeft() {
        if (popupStack.empty()) {
            if (currentScreen != 0) {
                if (g_ctrlModifier) {
                    g_switchScreen(0);
                }
                else {
                    g_switchScreen(currentScreen - 1);
                }
            }
        }
    }
    void switchScreenRight() {
        if (popupStack.empty()) {
            if (currentScreen < screenStack.size() - 1) {
                if (g_ctrlModifier) {
                    g_switchScreen(screenStack.size() - 1);
                }
                else {
                    g_switchScreen(currentScreen + 1);
                }
            }
        }
    }
};