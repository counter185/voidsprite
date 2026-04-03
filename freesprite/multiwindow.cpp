#include <SDL3_ttf/SDL_ttf.h>

#include "multiwindow.h"
#include "Notification.h"
#include "BasePopup.h"
#include "BaseScreen.h"
#include "FontRenderer.h"
#include "ButtonStartScreenSession.h"
#include "ScreenWideNavBar.h"

#include "main.h"

#if VSP_PLATFORM == VSP_PLATFORM_WIN32
#include <windows.h>
//do not remove or panelmcblockpreview will not compile
#undef small
#endif


VSPWindow::VSPWindow(std::string title, XY size, u32 flags) {
    wd = SDL_CreateWindow(title.c_str(), size.x, size.y, flags);
    if (wd != NULL) {
        platformWindowCreated(this);
    }
    windowID = SDL_GetWindowID(wd);
    tryCreateRenderer();
    setWindowTitle(title);
    setVsync(g_config.vsync);
    SDL_SetWindowBordered(wd, !g_config.customWindowFrame);
    SDL_GetWindowSize(wd, &size.x, &size.y);
    unscaledWindowSize = scaledWindowSize = size;
    if (g_config.autoViewportScale) {
        autoViewportScale();
    }
    blurBuffer = new WindowBlurBuffer(this);
}

VSPWindow::~VSPWindow() {
    platformWindowDestroyed(this);
    if (blurBuffer != NULL) {
        delete blurBuffer;
    }
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
    if (fnt != NULL) {
        delete fnt;
    }
    for (BaseScreen* screen : screenStack) {
        delete screen;
    }
    for (BasePopup* popup : popupStack) {
        delete popup;
    }

}

VSPWindow* VSPWindow::windowEventTarget(SDL_Event evt) {
    SDL_Window* windowFromEvt = SDL_GetWindowFromEvent(&evt);
    if (windowFromEvt == NULL) {
        return g_mainWindow;
    }
    else {
        SDL_WindowID id = SDL_GetWindowID(windowFromEvt);
        return g_windows.contains(id) ? g_windows[id] : g_mainWindow;
    }
}

std::string VSPWindow::findFallbackSystemFont(std::vector<std::string> fonts)
{
    auto sysFontPaths = platformGetSystemFontPaths();
    for (auto& fp : sysFontPaths) {
        for (auto& fnt : fonts) {
            std::filesystem::path p = fp;
            p /= fnt;
            if (std::filesystem::exists(p)) {
                return convertStringToUTF8OnWin32(p);
            }
        }
    }
    return "";
}

void VSPWindow::initFonts() {
    if (fnt != NULL) {
        delete fnt;
    }
    
    std::vector<std::string> fallbackFonts = {
        "Ubuntu.ttf",
        "Ubuntu-R.ttf",
        "LiberationSans-Regular.ttf",
        "SST-Medium.ttf",
        "seguisb.ttf",
        "segoeui.ttf",
        "tahoma.ttf",
        "arial.ttf"
    };

    std::vector<std::string> fallbackFontsJP = {
        "meiryo.ttc",
        //"msgothic.ttc",
        "YuGothB.ttc",
        "YuGothM.ttc"
    };

    fnt = new TextRenderer();
    TTF_Font* fontDefault = TTF_OpenFont(pathInProgramDirectory(FONT_PATH).c_str(), 18);
    fontDefault = fontDefault == NULL ? TTF_OpenFont(FONT_PATH, 18) : fontDefault;
    if (fontDefault == NULL) {
        std::string fallback = findFallbackSystemFont(fallbackFonts);
        if (fallback != "") {
            logwarn(frmt("Loading fallback system font: {}", fallback));
            fontDefault = TTF_OpenFont(fallback.c_str(), 18);
        }
    }

    if (fontDefault != NULL) {
        fnt->AddFont(fontDefault, { { 0, 0xFFFFFFFF } });
    }
    else {
        logerr("Failed to load the default font");
    }

    TTF_Font* fontJP = TTF_OpenFont(pathInProgramDirectory(FONT_PATH_JP).c_str(), 18);
    fontJP = fontJP == NULL ? TTF_OpenFont(FONT_PATH_JP, 18) : fontJP;
    if (fontJP == NULL) {
        std::string fallback = findFallbackSystemFont(fallbackFontsJP);
        if (fallback != "") {
            logwarn(frmt("Loading fallback system font: {}", fallback));
            fontJP = TTF_OpenFont(fallback.c_str(), 18);
        }
    }
    if (fontJP != NULL) {
        fnt->AddFont(fontJP, {
            { 0x3000, 0x30FF },   // CJK Symbols and Punctuation, hiragana, katakana
            { 0xFF00, 0xFFEF },   // Halfwidth and Fullwidth Forms
            { 0x4E00, 0x9FAF }    // CJK Unified Ideographs
        });
    }

    TTF_Font* fontCYR = TTF_OpenFont(pathInProgramDirectory(FONT_PATH_CYR).c_str(), 18);
    fontCYR = fontCYR == NULL ? TTF_OpenFont(FONT_PATH_CYR, 18) : fontCYR;
    //should be fine without its own fallback fonts
    if (fontCYR != NULL) {
        fnt->AddFont(fontCYR, {
            { 0x0400, 0x052F }, // Cyryllic + supplement
            { 0x2DE0, 0x2DFF }, // Cyryllic Extended-A
            { 0xA640, 0xA69F }, // Cyryllic Extended-B
            { 0x1C80, 0x1C8F }, // Cyryllic Extended-C
            { 0x1E030, 0x1E08F }    // Cyryllic Extended-D
        });
    }

    std::string customFont = visualConfigValue("general/font");
    if (!g_usingDefaultVisualConfig()) {
        PlatformNativePathString vcRoot = g_getCustomVisualConfigRoot();
        std::string customFontPath = convertStringToUTF8OnWin32(vcRoot) + "/" + customFont;
        if (std::filesystem::exists(vcRoot + convertStringOnWin32("/" + customFont))) {
            TTF_Font* fontCustom = TTF_OpenFont(customFontPath.c_str(), 18);
            if (fontCustom != NULL) {
                fnt->AddFont(fontCustom, { { 0, 0xFFFFFFFF } });
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

void VSPWindow::addToWindowList() {
    g_windows[windowID] = this;
}

void VSPWindow::thisWindowsTurn() {
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

bool VSPWindow::tryCreateRenderer() {
    g_availableRenderersNow.clear();
    for (int x = 0; x < SDL_GetNumRenderDrivers() - 1; x++) {
        char* name = (char*)SDL_GetRenderDriver(x);
        if (name != NULL) {
            std::string renderDriverName = name;
            g_availableRenderersNow.push_back(renderDriverName);
            loginfo(frmt("Renderer {}: {}", x, renderDriverName));
        }
    }
    if (std::find(g_availableRenderersNow.begin(), g_availableRenderersNow.end(), g_config.preferredRenderer) == g_availableRenderersNow.end()) {
        g_config.preferredRenderer = GlobalConfig().preferredRenderer;  //reset to default
    }
    std::string useRenderer = g_config.preferredRenderer;
    loginfo(frmt("Picking renderer: {}", useRenderer));
    rd = SDL_CreateRenderer(wd, useRenderer.c_str());
    while (rd == NULL) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("Renderer failed: {}", useRenderer)));
        logerr(frmt("Failed to create renderer: {}\n  {}", useRenderer, SDL_GetError()));
        g_availableRenderersNow.erase(std::remove(g_availableRenderersNow.begin(), g_availableRenderersNow.end(), useRenderer), g_availableRenderersNow.end());
        if (g_availableRenderersNow.empty()) {
            logerr("No renderers available");
            std::string errorTitle = frmt("voidsprite: {}", TL("vsp.error.norenderer.title"));
            std::string errorMsg = TL("vsp.error.norenderer.body");
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, errorTitle.c_str(), errorMsg.c_str(), wd);
            return false;
        }
        else {
            useRenderer = g_availableRenderersNow[0];
            g_config.preferredRenderer = useRenderer;
            loginfo(frmt("Trying renderer: {}", useRenderer));
            rd = SDL_CreateRenderer(wd, useRenderer.c_str());
        }
    }
    SDL_SetRenderDrawBlendMode(rd, SDL_BLENDMODE_BLEND);
    return true;
}

void VSPWindow::updateViewportScaler() {
    XY previousSize = scaledWindowSize;
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

    if (blurBuffer != NULL) {
        blurBuffer->windowResized();
    }

    for (auto*& screen : screenStack) {
        screen->screenResized(previousSize, scaledWindowSize);
    }

    if (g_currentWindow == this) {
        thisWindowsTurn();
    }
}

void VSPWindow::autoViewportScale() {
    int newViewportScale = 1;
    //okay no don't scale by height
    /*
    if (g_windowW > g_windowH) {
        newViewportScale = unscaledWindowSize.y < (720 * 1.5) ? 1
            : ixmax(2, unscaledWindowSize.y / 720);
    }
    else {*/
        newViewportScale = unscaledWindowSize.x < (1280 * 1.5) ? 1
            : ixmax(2, unscaledWindowSize.x / 1280);
    //}
    g_renderScale = newViewportScale;
    updateViewportScaler();
}

void VSPWindow::addScreen(BaseScreen* a, bool switchTo) {
    screenStack.push_back(a);
    if (switchTo) {
        g_switchScreen(screenStack.size() - 1);
    }
    ButtonStartScreenSession* screenButton = new ButtonStartScreenSession(this, screenStack.size() - 1);
    screenButtons.push_back(screenButton);
    overlayWidgets.addDrawable(screenButton);
}

void VSPWindow::detachScreen(BaseScreen* screen) {
    if (favScreen == screen) {
        favScreen = NULL;
    }
    auto index = std::find(screenStack.begin(), screenStack.end(), screen);
    if (index != screenStack.end()) {
        int i = index - screenStack.begin();
        screenStack.erase(index);
        overlayWidgets.removeDrawable(screenButtons[screenButtons.size() - 1]);
        screenButtons.pop_back();
        if (currentScreen >= screenStack.size()) {
            switchScreen(currentScreen - 1);
        }
    }
}

bool VSPWindow::closeScreen(BaseScreen* screen) {
    if (favScreen == screen) {
        favScreen = NULL;
    }
    //close subscreens first
    for (int x = 0; x < screenStack.size(); x++) {
        if (screenStack[x] != screen && screenStack[x]->isSubscreenOf() == screen) {
            //do not replace this with closeScreen, screens can be in other windows too
            g_closeScreen(screenStack[x]);
            x--;
        }
    }

    auto index = std::find(screenStack.begin(), screenStack.end(), screen);
    if (index == screenStack.end()) {
        return false;
    }
    else {
        int x = index - screenStack.begin();
        delete screenStack[x];
        if (x == currentScreen) {
            g_newVFX(VFX_SCREENCLOSE, 500);
        }
        screenStack.erase(screenStack.begin() + x);
        overlayWidgets.removeDrawable(screenButtons[screenButtons.size() - 1]);
        screenButtons.pop_back();
        if (currentScreen >= screenStack.size()) {
            switchScreen(currentScreen - 1);
        }
        return true;
    }
}

void VSPWindow::switchScreen(int index, int vfxExt) {
    if (index >= 0 && index < screenStack.size()) {
        if (index != currentScreen) {
            currentScreen = index;
            screenStack[currentScreen]->onReturnToScreen();
            g_newVFX(VFX_SCREENSWITCH, 800, vfxExt);
        }
        overlayWidgets.forceUnfocus();
    }
}

void VSPWindow::switchScreenLeft() {
    if (popupStack.empty()) {
        if (currentScreen != 0) {
            if (g_ctrlModifier) {
                g_switchScreen(0, 1);
            }
            else {
                g_switchScreen(currentScreen - 1, 1);
            }
        }
    }
}

void VSPWindow::switchScreenRight() {
    if (popupStack.empty()) {
        if (currentScreen < screenStack.size() - 1) {
            if (g_ctrlModifier) {
                g_switchScreen(screenStack.size() - 1, 2);
            }
            else {
                g_switchScreen(currentScreen + 1, 2);
            }
        }
    }
}

int VSPWindow::indexOfScreen(BaseScreen* screen)
{
    return std::find(screenStack.begin(), screenStack.end(), screen) - screenStack.begin();
}

void VSPWindow::assignFavScreen()
{
    BaseScreen* screenNow = screenStack[currentScreen];
    if (favScreen == screenNow) {
        favScreen = NULL;
    }
    else {
        favScreen = screenNow;
    }
    screenSwitchTimer.start();
}

void VSPWindow::switchToFavScreen()
{
    if (favScreen != NULL) {
        int index = std::find(screenStack.begin(), screenStack.end(), favScreen) - screenStack.begin();
        if (index < screenStack.size()) {
            g_switchScreen(index);
        }
        else {
            logerr("[VSPWindow] fav screen out of range");
        }
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.window.error.nofavscreen")));
    }

}

void VSPWindow::renderCustomWindowFrame()
{
    SDL_SetRenderDrawColor(rd, 255, 255, 255, 0x30);
    SDL_Rect wdr = {
        0,0,
        unscaledWindowSize.x,
        unscaledWindowSize.y
    };
    SDL_RenderDrawRect(rd, &wdr);

    const int actionButtonW = 46;
    const int actionButtonH = 36;

    double distanceToTopRight = xyDistance({ mouseX, mouseY }, { unscaledWindowSize.x,0 });
    bool mouseInRange = distanceToTopRight < actionButtonW * 4;

    if (mouseInRange != prevMouseInActionButtonRange) {
        prevMouseInActionButtonRange = mouseInRange;
        windowActionButtonsAnimTimer.start();
    }

    double timer = windowActionButtonsAnimTimer.started ? XM1PW3P1(windowActionButtonsAnimTimer.percentElapsedTime(300)) : 1.0;
    timer = (mouseInRange ? timer : (1.0-timer));

    //render close,maximize,minimize
    SDL_Rect buttonNow = { unscaledWindowSize.x - actionButtonW,0,actionButtonW,actionButtonH };
    buttonNow.h *= timer;

    for (int x = 0; x < 3; x++) {

        if (timer > 0) {
            //fill
            Fill::Gradient(0xD0202020, 0xD0202020, 0xD0000000, 0xD0000000).fill(buttonNow);
            if (pointInBox({ mouseX, mouseY }, buttonNow)) {
                if (x == 0) {
                    Fill::Gradient(0xD0300000, 0xD0300000, 0xD0600000, 0xD0600000).fill(buttonNow);
                }
                else {
                    Fill::Gradient(0xD0303030, 0xD0303030, 0xD0606060, 0xD0606060).fill(buttonNow);
                }

            }

            //outline
            SDL_SetRenderDrawColor(rd, 255, 255, 255, 0x30 * timer);
            SDL_RenderDrawRect(rd, &buttonNow);
        }

        double localTimer = XM1PW3P1(windowActionButtonsAnimTimer.percentElapsedTime(150, 70 * x));
        localTimer = (mouseInRange ? localTimer : (1.0 - localTimer));
        localTimer = timer == 0 ? 1.0 : localTimer;

        int innerW = actionButtonH / 3;

        SDL_Rect iconRect = {
            buttonNow.x + actionButtonW / 2 - innerW / 2,
            buttonNow.y + actionButtonH / 2 - innerW / 2,
            innerW, innerW };
        SDL_SetRenderDrawColor(rd, 255, 255, 255, timer > 0 ? 255 : 80);
        switch (x) {
            case 0:
                //close
                drawLine({ iconRect.x, iconRect.y }, { iconRect.x + iconRect.w, iconRect.y + iconRect.h }, localTimer);
                drawLine({ iconRect.x, iconRect.y + iconRect.h }, { iconRect.x + iconRect.w, iconRect.y }, localTimer);
                break;
            case 1:
                //maximize
                drawLine({ iconRect.x, iconRect.y }, { iconRect.x + iconRect.w, iconRect.y }, localTimer);
                drawLine({ iconRect.x, iconRect.y }, { iconRect.x, iconRect.y + iconRect.h }, localTimer);

                drawLine({ iconRect.x + iconRect.w, iconRect.y + iconRect.h }, { iconRect.x, iconRect.y + iconRect.h }, localTimer);
                drawLine({ iconRect.x + iconRect.w, iconRect.y + iconRect.h }, { iconRect.x + iconRect.w, iconRect.y }, localTimer);
                break;
            case 2:
                //minimize
                drawLine({ iconRect.x, iconRect.y + iconRect.h / 2 }, { iconRect.x + iconRect.w, iconRect.y + iconRect.h / 2 }, localTimer);
                break;
        }

        buttonNow.x -= actionButtonW;
    }
}

bool VSPWindow::handleCustomFrameInput(SDL_Event evt)
{
    const int actionButtonW = 46;
    const int actionButtonH = 36;
    SDL_Event cevt = convertTouchToMouseEvent(evt);
    switch (cevt.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            XY pos = { (int)evt.button.x, (int)evt.button.y };
            int buttonsOrigin = unscaledWindowSize.x - (actionButtonW * 3);
            if (pos.y < actionButtonH && pos.x > buttonsOrigin) {
                int buttonIndex = ixmin(2, (pos.x - buttonsOrigin) / actionButtonW);
                switch (buttonIndex) {
                    case 0:
                        //minimize
                        SDL_MinimizeWindow(wd);
                        break;
                    case 1:
                        //maximize
                        if ((SDL_GetWindowFlags(wd) & SDL_WINDOW_MAXIMIZED) == 0) {
                            SDL_MaximizeWindow(wd);
                        }
                        else {
                            SDL_RestoreWindow(wd);
                        }
                        break;
                    case 2:
                        //close
                        {
                            SDL_Event closeEvent = { .type = SDL_EVENT_WINDOW_CLOSE_REQUESTED };
                            closeEvent.window.windowID = SDL_GetWindowID(wd);
                            SDL_PushEvent(&closeEvent);
                        }
                        break;
                }
                return true;
            }
            return false;
    }
    return false;
}

#if VSP_PLATFORM == VSP_PLATFORM_WIN32
long VSPWindow::getWin32HitTestAt(XY pos)
{
    const int windowBorderSize = 5;
    const int actionButtonW = 45;
    const int actionButtonH = 35;

    if (xyDistance(pos, { 0,0 }) <= windowBorderSize) {
        return HTTOPLEFT;
    }
    else if (xyDistance(pos, { 0,unscaledWindowSize.y }) <= windowBorderSize) {
        return HTBOTTOMLEFT;
    }
    else if (xyDistance(pos, { unscaledWindowSize.x,0 }) <= windowBorderSize) {
        return HTTOPRIGHT;
    }
    else if (xyDistance(pos, { unscaledWindowSize.x,unscaledWindowSize.y }) <= windowBorderSize) {
        return HTBOTTOMRIGHT;
    }

    else if (pos.y <= windowBorderSize) {
        return HTTOP;
    }
    else if (pos.y >= (unscaledWindowSize.y - windowBorderSize)) {
        return HTBOTTOM;
    }
    else if (pos.x <= windowBorderSize) {
        return HTLEFT;
    }
    else if (pos.x >= (unscaledWindowSize.x - windowBorderSize)) {
        return HTRIGHT;
    }
    else if (pos.y < actionButtonH && pos.x > unscaledWindowSize.x - (actionButtonW * 3)) {
        return HTCLIENT;
    }

    ScreenWideNavBar* currentNavbar = screenStack[currentScreen]->getNavbar();
    if (currentNavbar == NULL || !popupStack.empty()) {
        if (pos.y < 30) {
            return HTCAPTION;
        }
    }
    else {
        if (currentNavbar->pointInWindowDrag(pos)) {
            return HTCAPTION;
        }
    }
    return HTCLIENT;
}
#endif

void WindowBlurBuffer::windowResized()
{
    if (!enabled) return;
    if (blurBuffer != NULL) {
        tracked_destroyTexture(blurBuffer);
    }
    if (fullscreenBuffer != NULL) {
        tracked_destroyTexture(fullscreenBuffer);
    }

    fullscreenBuffer = tracked_createTexture(
        parentWindow->rd,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        parentWindow->scaledWindowSize.x,
        parentWindow->scaledWindowSize.y);

    blurBuffer = tracked_createTexture(
        parentWindow->rd,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        ixmax(1, parentWindow->scaledWindowSize.x / downscaleFactor),
        ixmax(1, parentWindow->scaledWindowSize.y / downscaleFactor));

    SDL_SetTextureScaleMode(fullscreenBuffer, SDL_SCALEMODE_LINEAR);
    SDL_SetTextureScaleMode(blurBuffer, SDL_SCALEMODE_LINEAR);
    initialized = false;
}

void WindowBlurBuffer::pushFullscreenBuffer() {
    if (!enabled) return;
    if (!initialized) {
        g_pushRenderTarget(blurBuffer);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 255);
        SDL_RenderClear(parentWindow->rd);
        g_popRenderTarget();
        initialized = true;
    }

    if (fullscreenBuffer != NULL) {
        g_pushRenderTarget(fullscreenBuffer);
    }
}

void WindowBlurBuffer::renderFullscreenBufferToScreen()
{
    if (fullscreenBuffer) {
        g_popRenderTarget();
        SDL_SetTextureAlphaMod(fullscreenBuffer, 255);
		SDL_RenderCopy(parentWindow->rd, fullscreenBuffer, NULL, NULL);
        g_pushRenderTarget(fullscreenBuffer);
    }
}

void WindowBlurBuffer::popAndApplyFullscreenBuffer()
{
    if (!enabled) return;
    if (fullscreenBuffer != NULL) {
        g_popRenderTarget();

        g_pushRenderTarget(blurBuffer);
        SDL_SetTextureAlphaMod(fullscreenBuffer, 10);
        SDL_RenderCopy(parentWindow->rd, fullscreenBuffer, NULL, NULL);
        SDL_SetTextureAlphaMod(fullscreenBuffer, 255);
        g_popRenderTarget();

        g_pushRenderTarget(fullscreenBuffer);
        SDL_RenderCopy(parentWindow->rd, blurBuffer, NULL, NULL);
        /*SDL_SetRenderDrawColor(parentWindow->rd, 0, 0, 0, 255);
        SDL_RenderClear(parentWindow->rd);
        SDL_Rect targetRegion = { 0,0, parentWindow->scaledWindowSize.x, parentWindow->scaledWindowSize.y };
        SDL_SetTextureAlphaMod(blurBuffer, 255/12);
        SDL_BlendMode blendRGBaddAlpha = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            SDL_BLENDOPERATION_ADD,
            SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE,
            SDL_BLENDOPERATION_ADD);
        SDL_SetTextureBlendMode(blurBuffer, blendRGBaddAlpha);
        for (int i = -6; i <= 6; i++) {
            SDL_Rect offsetRegion = targetRegion;
            offsetRegion.x = i*downscaleFactor; 
            //offsetRegion.y = i;
            SDL_RenderCopy(parentWindow->rd, blurBuffer, NULL, &offsetRegion);
        }*/
        g_popRenderTarget();
    }
}

void WindowBlurBuffer::blurBehindAllPanels(std::vector<Drawable*>& drawables)
{
    if (!enabled) return;
    for (auto*& d : drawables) {
        if (d->isPanel()) {
            XY renderDimensions = d->getRenderDimensions();
            g_currentWindow->blurBuffer->renderBlurBehind({ d->position.x, d->position.y, renderDimensions.x, renderDimensions.y });
        }
    }
}

void WindowBlurBuffer::renderBlurBehind(SDL_Rect region)
{
    if (!enabled) return;
    SDL_RenderCopy(parentWindow->rd, fullscreenBuffer, &region, &region);
}
