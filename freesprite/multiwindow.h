#pragma once

#include "globals.h"
#include "Timer64.h"
#include "DrawableManager.h"

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
    std::string windowTitle = "";

    BaseScreen* favScreen = NULL;

    int mouseX = 0, mouseY = 0;

    DrawableManager overlayWidgets;
    std::vector<ButtonStartScreenSession*> screenButtons;

    bool isMainWindow = false;

    VSPWindow(std::string title, XY size, u32 flags);
    ~VSPWindow();

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

    static VSPWindow* windowEventTarget(SDL_Event evt);

    bool hasPopupsOpen() {
        return !popupStack.empty();
    }

    void initFonts();

    void addToWindowList();

    void thisWindowsTurn();

    void setVsync(bool vsync) {
        if (rd != NULL) {
            SDL_SetRenderVSync(rd, vsync ? 1 : SDL_RENDERER_VSYNC_DISABLED);
        }
    }

    void setWindowTitle(std::string wdTitle) {
        windowTitle = wdTitle;
        std::string finalWdTitle = UTF8_DIAMOND + wdTitle;
        SDL_SetWindowTitle(wd, finalWdTitle.c_str());
    }

    bool tryCreateRenderer();

    void updateViewportScaler();

    void autoViewportScale();

    void addScreen(BaseScreen* a, bool switchTo);

    void detachScreen(BaseScreen* screen);

    bool closeScreen(BaseScreen* screen);

    void switchScreen(int index);

    void switchScreenLeft();
    void switchScreenRight();

    int indexOfScreen(BaseScreen* screen);

    void assignFavScreen();
    void switchToFavScreen();
};