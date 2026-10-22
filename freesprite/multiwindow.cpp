#include <SDL3_ttf/SDL_ttf.h>

#include "multiwindow.h"
#include "Notification.h"
#include "BasePopup.h"
#include "BaseScreen.h"
#include "FontRenderer.h"
#include "ButtonStartScreenSession.h"
#include "ScreenWideNavBar.h"
#include "TooltipsLayer.h"

#include "main.h"
#include "thumbnail_loader.h"

#if VSP_PLATFORM == VSP_PLATFORM_WIN32
#include <Windows.h>
#endif

#define windowW scaledWindowSize.x
#define windowH scaledWindowSize.y


VSPWindow::VSPWindow(std::string title, XY size, u32 flags) {
    flags |= this->defaultWindowFlags();
    wd = SDL_CreateWindow(title.c_str(), size.x, size.y, flags);
    if (wd != NULL) {
        platformWindowCreated(this);
    }
    windowID = SDL_GetWindowID(wd);
    tryCreateRenderer();
    setWindowTitle(title);
    setVsync(g_config.vsync);
    setCustomWindowFrame(this->shouldUseCustomWindowFrame());
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
    bool fontDefaultIsFallback = false;
    TTF_Font* fontDefault = TTF_OpenFont(pathInProgramDirectory(FONT_PATH).c_str(), 18);
    fontDefault = fontDefault == NULL ? TTF_OpenFont(FONT_PATH, 18) : fontDefault;
    if (fontDefault == NULL) {
        fontDefaultIsFallback = true;
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

    TTF_Font* fontJP = NULL;
    //load fontJP only if mplus failed
    if (fontDefaultIsFallback) {
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
    auto renderDrivers = this->getRenderDrivers();
    if (g_availableRenderersNow.empty()) {
        g_availableRenderersNow = renderDrivers;
    }
    if (std::find(g_availableRenderersNow.begin(), g_availableRenderersNow.end(), g_config.preferredRenderer) == g_availableRenderersNow.end()) {
        g_config.preferredRenderer = GlobalConfig().preferredRenderer;  //reset to default
    }
    bool preferredRendererIsOnList = g_config.preferredRenderer == ""
        || std::find(renderDrivers.begin(), renderDrivers.end(), g_config.preferredRenderer) != renderDrivers.end();
    std::string useRenderer = preferredRendererIsOnList ? g_config.preferredRenderer : renderDrivers.front();
    loginfo(frmt("Picking renderer: {}", useRenderer));
    rd = SDL_CreateRenderer(wd, useRenderer.c_str());
    while (rd == NULL) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("Renderer failed: {}", useRenderer)));
        logerr(frmt("Failed to create renderer: {}\n  {}", useRenderer, SDL_GetError()));
        std::erase(renderDrivers, useRenderer);
        if (renderDrivers.empty()) {
            logerr("No renderers available");
            std::string errorTitle = frmt("voidsprite: {}", TL("vsp.error.norenderer.title"));
            std::string errorMsg = TL("vsp.error.norenderer.body");
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, errorTitle.c_str(), errorMsg.c_str(), wd);
            return false;
        }
        else {
            useRenderer = renderDrivers.front();
            g_config.preferredRenderer = useRenderer;
            loginfo(frmt("Trying renderer: {}", useRenderer));
            if (useRenderer == SDL_SOFTWARE_RENDERER) {
                g_addNotification(ErrorNotification("Falling back to software renderer","Expect slow performance."));
            }
            rd = SDL_CreateRenderer(wd, useRenderer.c_str());
        }
    }
    SDL_SetRenderDrawBlendMode(rd, SDL_BLENDMODE_BLEND);
    return true;
}

std::vector<std::string> VSPWindow::getRenderDrivers()
{
    std::vector<std::string> ret;
    for (int x = 0; x < SDL_GetNumRenderDrivers() - 1; x++) {
        char* name = (char*)SDL_GetRenderDriver(x);
        if (name != NULL) {
            std::string renderDriverName = name;
            ret.push_back(renderDriverName);
            loginfo(frmt("Renderer {}: {}", x, renderDriverName));
        }
    }

    if (std::find(ret.begin(), ret.end(), "opengl") != ret.end()) {
        //put opengl at the front so that transparent windows work on vmware
        std::erase(ret, "opengl");
        ret.insert(ret.begin(), "opengl");
    }

    ret.push_back(SDL_SOFTWARE_RENDERER);
    return ret;
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
    a->scaleScreen(g_config.uiScale);
    screenStack.push_back(a);
    if (switchTo) {
        switchScreen(screenStack.size() - 1);
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
            g_switchScreen(currentScreen - 1, 1);
        }
    }
}

void VSPWindow::switchScreenRight() {
    if (popupStack.empty()) {
        if (currentScreen < screenStack.size() - 1) {
            g_switchScreen(currentScreen + 1, 2);
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

void VSPWindow::tickWindow()
{
    if (!popupStack.empty()) {
        popupStack[popupStack.size() - 1]->tick();
    }
    if (!screenStack.empty()) {
        screenStack[currentScreen]->tick();
    }
}

void VSPWindow::renderWindow()
{
    if (viewport != NULL) {
        g_pushRenderTarget(viewport);
    }

    clearWindow();

    renderScreenAndPopups();

    //debug: render safearea
    /*auto safeArea = wd->getSafeAreaCorners();
    SDL_Rect safeAreaRect = {safeArea.first.x, safeArea.first.y,
                             safeArea.second.x - safeArea.first.x,
                             safeArea.second.y - safeArea.first.y};
    SDL_SetRenderDrawColor(rd, 255,0,0,255);
    SDL_RenderDrawRect(rd, &safeAreaRect);*/

    overlayRenderQueue.renderAll();

    renderVFX();


    if (this->shouldRenderStatus()) {
        renderDebugInfo();
        renderStatusBar();
    }

    g_tickNotifications();
    g_renderNotifications();

    g_ttp->renderAll();

    g_cleanUpDoneAsyncThreads();
    if (main_anyPopupsOpen() && !hasPopupsOpen()) {
        SDL_Rect wdrect = { 0, 0, g_windowW, g_windowH };
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        SDL_RenderFillRect(g_rd, &wdrect);
    }

    if (g_bgOpRunning) {
        renderBackgroundOpInProgressScreen();
    }
    else {
        //draw the mouse position 4x4 square
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
        SDL_Rect temp = { g_mouseX, g_mouseY, 4, 4 };
        SDL_RenderFillRect(g_rd, &temp);
    }

    if (viewport != NULL) {
        g_popRenderTarget();
        SDL_RenderCopy(g_rd, viewport, NULL, NULL);
    }

    if (this->shouldUseCustomWindowFrame()) {
        renderCustomWindowFrame();
    }

    SDL_RenderPresent(rd);
    doFrameDelay();
}

void VSPWindow::doFrameDelay()
{
    if (!g_config.vsync) {
        SDL_Delay(3);
    }
    int batterySeconds, batteryPercent;
    SDL_PowerState powerstate = SDL_GetPowerInfo(&batterySeconds, &batteryPercent);
    if (!g_windowFocused && !g_fullFramerateThisFrame) {
        if (g_config.powerSaverLevel == 2 || (g_config.powerSaverLevel == 3 && powerstate != SDL_POWERSTATE_NO_BATTERY && batteryPercent <= 15)) {
            SDL_Delay(500);
        }
        else if (g_config.powerSaverLevel == 1 || (g_config.powerSaverLevel == 3 && powerstate != SDL_POWERSTATE_NO_BATTERY)) {
            SDL_Delay(45);
        }

    }
}

void VSPWindow::renderDebugInfo()
{
#if _DEBUG
    XY origin = { windowW - 240, windowH - 90 };
    for (auto& mem : g_named_memmap) {
        g_fnt->RenderString(frmt("{} | {}", mem.first, bytesToFriendlyString(mem.second)), origin.x,
            origin.y, { 255, 255, 255, 100 }, 14);
        origin.y -= 16;
    }
    g_fnt->RenderString(frmt("Textures created: {}", g_allocated_textures), origin.x, origin.y,
        { 255, 255, 255, 100 }, 14);
    origin.y -= 16;
    g_fnt->RenderString(frmt("Drawables created: {}", g_createdDrawables), origin.x, origin.y,
        { 255, 255, 255, 100 }, 14);
    origin.y -= 16;
    g_fnt->RenderString(frmt("Thumbnail cache: {}", bytesToFriendlyString(thumbnails_cacheSize)), origin.x, origin.y,
        { 255, 255, 255, 100 }, 14);
    origin.y -= 16;
    //g_fnt->RenderString(frmt("{} FPS", lastFrameCount), origin.x, origin.y, { 255,255,255,100 }, 14);
    //origin.y -= 16;
#endif
}

void VSPWindow::renderStatusBar()
{
    int rightX = getSafeAreaRight();
    XY nextStatusBarOrigin = { windowW - rightX, windowH - 26 };

    //draw the screen icons
    //XY screenIcons = { g_windowW, g_windowH - 10 };
    nextStatusBarOrigin = xySubtract(nextStatusBarOrigin, XY{ (int)(26 * screenStack.size()), 0 });
    XY screenIconOrigin = nextStatusBarOrigin;

    for (int x = 0; x < screenStack.size(); x++) {
        BaseScreen* s = screenStack[x];
        //this is where screen icons were rendered
        screenButtons[x]->position = screenIconOrigin;
        screenIconOrigin.x += 26;
    }
    overlayWidgets.renderAll();
    //todo: make this a uilabel
    if (!screenStack.empty()) {
        std::string screenName = screenStack[currentScreen]->getName();
        int statW = g_fnt->StatStringDimensions(screenName, 16).x;
        XY screenNameOrigin = xySubtract({ windowW - rightX, windowH }, { 10 + statW, 55 });
        g_fnt->RenderString(screenStack[currentScreen]->getName(), screenNameOrigin.x, screenNameOrigin.y, { 255,255,255,255 }, 16);
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

        if (batteryPercent <= 10 && powerstate != SDL_POWERSTATE_CHARGING && (SDL_GetTicks64() - lastLowBatteryPulseTime) > 3000) {
            lastLowBatteryPulseTime = SDL_GetTicks64();
            g_newVFX(VFX_LOWBATTERYPULSE, 2000, 0, batteryRect);
        }

        SDL_Color primaryColor =
            powerstate == SDL_POWERSTATE_CHARGING ? SDL_Color{ 253, 255, 146, 0x80 }
            : powerstate == SDL_POWERSTATE_CHARGED ? SDL_Color{ 78,255,249, 0x80 }
            : powerstate == SDL_POWERSTATE_ERROR ? SDL_Color{ 255, 40, 40, 0x80 }
            : batteryPercent <= 10 ? SDL_Color{ 255, 0x50, 0x50, 0x80 }
        : SDL_Color{ 255,255,255, 0x80 };

        SDL_SetRenderDrawColor(rd, primaryColor.r, primaryColor.g, primaryColor.b, primaryColor.a);
        drawLine(batteryOrigin, batteryOriginLow);
        drawLine(batteryEnd, xyAdd(batteryOriginLow, { batteryRectW, 0 }));
        XY statPercent = statLineEndpoint(batteryOrigin, xyAdd(batteryOrigin, { batteryRectW, 0 }), batteryPercent / 100.0);
        XY statPercentLow = { statPercent.x, batteryOriginLow.y };
        drawLine(statPercent, statPercentLow);
        drawLine(batteryOrigin, statPercent);
        drawLine(batteryOriginLow, statPercentLow);
        g_fnt->RenderString(frmt("{}%", batteryPercent), batteryEnd.x + 5, batteryEnd.y - 5, primaryColor);

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

    if (g_config.showSystemClock) {
        std::tm sysTime = getLocalTime();
        XY sysTimeOrigin = xySubtract(nextStatusBarOrigin, { 70, 0 });
        nextStatusBarOrigin = sysTimeOrigin;
        XY timeEndpoint = g_fnt->RenderString(frmt("{:02}:{:02}", sysTime.tm_hour, sysTime.tm_min), sysTimeOrigin.x, sysTimeOrigin.y - 5, { 255,255,255,0x80 }, 16);
        g_fnt->RenderString(frmt(":{:02}", sysTime.tm_sec), timeEndpoint.x, timeEndpoint.y, { 255,255,255,0x50 }, 12);
    }

    if (g_config.showFPS) {
        XY fpsOrigin = xySubtract(nextStatusBarOrigin, { 70, 0 });
        nextStatusBarOrigin = fpsOrigin;
        g_fnt->RenderString(frmt("{} FPS", lastFrameCount), fpsOrigin.x, fpsOrigin.y - 5, { 255,255,255,0x80 }, 16);
    }
}

void VSPWindow::renderBackgroundOpInProgressScreen()
{
    SDL_Rect r = { 0, 0, g_windowW, g_windowH };

    u32 colFB = PackRGBAtoARGB(0, 0, 0, (u8)(0xd0 * g_bgOpStartTimer.percentElapsedTime(600)));
    u32 colHB = PackRGBAtoARGB(0, 0, 0, (u8)(0x80 * g_bgOpStartTimer.percentElapsedTime(600)));

    renderGradient({ 0,0,g_windowW,g_windowH / 2 }, colFB, colFB, colHB, colHB);
    renderGradient({ 0,g_windowH / 2,g_windowW,g_windowH / 2 + 1 }, colHB, colHB, colFB, colFB);

    TooltipsLayer localttp;

    static std::string bgOpInProgressText = TL("vsp.bgop.inprogress");

    std::string currentText = bgOpInProgressText;

    int lines = 1;
    auto progressReports = g_bgOpProgressReport->getCurrentSections();
    for (const std::string& report : progressReports) {
        currentText += frmt("\n    {}", report);
        lines++;
    }

    localttp.addTooltip(Tooltip{ {0,g_windowH - (12 + 18 * lines) }, currentText, {255,255,255,255}, g_bgOpStartTimer.percentElapsedTime(600) });
    localttp.renderAll();
}

void VSPWindow::renderScreenAndPopups()
{
    if (!screenStack.empty()) {
        if (!popupStack.empty()) {
            g_ttp->takeTooltips = false;
            screenStack[currentScreen]->render();
            g_ttp->takeTooltips = true;
        }
        else {
            screenStack[currentScreen]->render();
        }
    }

    for (BasePopup*& popup : popupStack) {
        if (popup != popupStack.back()) {
            g_ttp->takeTooltips = false;
            popup->render();
            g_ttp->takeTooltips = true;
        }
        else {
            popup->render();
        }
    }
}

void VSPWindow::renderVFX()
{
    g_renderVFX(this);
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

    double distanceToTopRight = xyDistance(unscaledMousePos, { unscaledWindowSize.x,0 });
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
            if (pointInBox(unscaledMousePos, buttonNow)) {
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
                renderWindowIconClose(iconRect, localTimer);
                break;
            case 1:
                //maximize
                if (g_config.framelessWindowButtonStyle == 1) {
                    renderWindowIconKDEMaximize(iconRect, localTimer);
                } else {
                    renderWindowIconWindowsMaximize(iconRect, localTimer);
                }
                break;
            case 2:
                //minimize
                if (g_config.framelessWindowButtonStyle == 1) {
                    renderWindowIconKDEMinimize(iconRect, localTimer);
                } else {
                    renderWindowIconWindowsMinimize(iconRect, localTimer);
                }
                break;
        }

        buttonNow.x -= actionButtonW;
    }
}

void VSPWindow::renderWindowIconClose(SDL_Rect iconRect, double localTimer) {
    drawLine({ iconRect.x, iconRect.y }, { iconRect.x + iconRect.w, iconRect.y + iconRect.h }, localTimer);
    drawLine({ iconRect.x, iconRect.y + iconRect.h }, { iconRect.x + iconRect.w, iconRect.y }, localTimer);
}

void VSPWindow::renderWindowIconWindowsMaximize(SDL_Rect iconRect, double localTimer) {
    bool maximized = (SDL_GetWindowFlags(wd) & SDL_WINDOW_MAXIMIZED) != 0;
    XY topRight = { iconRect.x + iconRect.w, iconRect.y };
    if (maximized) {
        drawLine(topRight, { iconRect.x + 3, topRight.y }, localTimer);
        drawLine(topRight, { topRight.x, iconRect.y + iconRect.h - 3 }, localTimer);

        topRight.x -= 3;
        topRight.y += 3;
    }

    drawLine({ iconRect.x, topRight.y }, { topRight.x, topRight.y }, localTimer);
    drawLine({ iconRect.x, topRight.y }, { iconRect.x, iconRect.y + iconRect.h }, localTimer);

    drawLine({ topRight.x, iconRect.y + iconRect.h }, { iconRect.x, iconRect.y + iconRect.h }, localTimer);
    drawLine({ topRight.x, iconRect.y + iconRect.h }, { topRight.x, topRight.y }, localTimer);
}

void VSPWindow::renderWindowIconKDEMaximize(SDL_Rect iconRect, double localTimer) {
    if ((SDL_GetWindowFlags(wd) & SDL_WINDOW_MAXIMIZED) == 0) {
        drawLine({ iconRect.x, iconRect.y + iconRect.h / 2 + iconRect.h / 4 }, { iconRect.x + iconRect.w / 2, iconRect.y + iconRect.h / 2 - iconRect.h / 4 }, localTimer);
        drawLine({ iconRect.x + iconRect.w, iconRect.y + iconRect.h / 2 + iconRect.h / 4 }, { iconRect.x + iconRect.w / 2, iconRect.y + iconRect.h / 2 - iconRect.h / 4 }, localTimer);
    } else {
        drawLine({ iconRect.x, iconRect.y + iconRect.h / 2 }, { iconRect.x + iconRect.w / 2, iconRect.y + iconRect.h / 2 - iconRect.h / 2 }, localTimer);
        drawLine({ iconRect.x + iconRect.w, iconRect.y + iconRect.h / 2}, { iconRect.x + iconRect.w / 2, iconRect.y + iconRect.h / 2 - iconRect.h / 2 }, localTimer);
        drawLine({ iconRect.x, iconRect.y + iconRect.h / 2 }, { iconRect.x + iconRect.w / 2, iconRect.y + iconRect.h / 2 + iconRect.h / 2 }, localTimer);
        drawLine({ iconRect.x + iconRect.w, iconRect.y + iconRect.h / 2}, { iconRect.x + iconRect.w / 2, iconRect.y + iconRect.h / 2 + iconRect.h / 2 }, localTimer);
    }
}

void VSPWindow::renderWindowIconWindowsMinimize(SDL_Rect iconRect, double localTimer) {
    drawLine({ iconRect.x, iconRect.y + iconRect.h / 2 }, { iconRect.x + iconRect.w, iconRect.y + iconRect.h / 2 }, localTimer);
}

void VSPWindow::renderWindowIconKDEMinimize(SDL_Rect iconRect, double localTimer) {
    drawLine({ iconRect.x, iconRect.y + iconRect.h / 2 - iconRect.h / 4 }, { iconRect.x + iconRect.w / 2, iconRect.y + iconRect.h / 2 + iconRect.h / 4 }, localTimer);
    drawLine({ iconRect.x + iconRect.w, iconRect.y + iconRect.h / 2 - iconRect.h / 4 }, { iconRect.x + iconRect.w / 2, iconRect.y + iconRect.h / 2 + iconRect.h / 4 }, localTimer);
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
                        if (!IsMaximized()) {
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
                if (buttonIndex == 2) {
                    g_newVFX(VFX_WINDOWBUTTONPULSE, 1000, buttonIndex, { buttonsOrigin + actionButtonW * buttonIndex, 0, actionButtonW, actionButtonH });
                }
                return true;
            }
            return false;
    }
    return false;
}

std::pair<XY,XY> VSPWindow::getSafeAreaCorners() {
#if VSP_PLATFORM == VSP_PLATFORM_ANDROID
    SDL_Rect r{};
    if (SDL_GetWindowSafeArea(wd, &r)) {
        return {{r.x / renderScale,r.y / renderScale}, {(r.w+r.x) / renderScale, (r.h+r.y) / renderScale}};
    } else
#endif
    {
        return {{0,0},{unscaledWindowSize.x / renderScale, unscaledWindowSize.y / renderScale}};
    }
}

int VSPWindow::getSafeAreaLeft() {
    return getSafeAreaCorners().first.x;
}

int VSPWindow::getSafeAreaRight() {
    return (unscaledWindowSize.x / renderScale) - getSafeAreaCorners().second.x;
}

bool VSPWindow::IsMaximized()
{
    return (SDL_GetWindowFlags(wd) & SDL_WINDOW_MAXIMIZED) != 0;
}

SDL_HitTestResult VSPWindow::getSDLHitTestAt(XY pos)
{
    if (!g_config.customWindowFrame) {
        return SDL_HITTEST_NORMAL;
    }
    const int windowBorderSize = 5;
    const int actionButtonW = 45;
    const int actionButtonH = 35;

    if (!IsMaximized()) {
        if (xyDistance(pos, { 0,0 }) <= windowBorderSize) {
            return SDL_HITTEST_RESIZE_TOPLEFT;
        }
        else if (xyDistance(pos, { 0,unscaledWindowSize.y }) <= windowBorderSize) {
            return SDL_HITTEST_RESIZE_BOTTOMLEFT;
        }
        else if (xyDistance(pos, { unscaledWindowSize.x,0 }) <= windowBorderSize) {
            return SDL_HITTEST_RESIZE_TOPRIGHT;
        }
        else if (xyDistance(pos, { unscaledWindowSize.x,unscaledWindowSize.y }) <= windowBorderSize) {
            return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
        }

        else if (pos.y <= windowBorderSize) {
            return SDL_HITTEST_RESIZE_TOP;
        }
        else if (pos.y >= (unscaledWindowSize.y - windowBorderSize)) {
            return SDL_HITTEST_RESIZE_BOTTOM;
        }
        else if (pos.x <= windowBorderSize) {
            return SDL_HITTEST_RESIZE_LEFT;
        }
        else if (pos.x >= (unscaledWindowSize.x - windowBorderSize)) {
            return SDL_HITTEST_RESIZE_RIGHT;
        }
    }
    if (pos.y < actionButtonH && pos.x > unscaledWindowSize.x - (actionButtonW * 3)) {
        return SDL_HITTEST_NORMAL;
    }

    ScreenWideNavBar* currentNavbar = currentScreen < screenStack.size() ? screenStack[currentScreen]->getNavbar() : NULL;
    if (currentNavbar == NULL || !popupStack.empty()) {
        if (pos.y < 30 * g_renderScale) {
            return SDL_HITTEST_DRAGGABLE;
        }
    }
    else {
        if (currentNavbar->pointInWindowDrag({ pos.x/g_renderScale, pos.y/g_renderScale })) {
            return SDL_HITTEST_DRAGGABLE;
        }
    }
    return SDL_HITTEST_NORMAL;
}

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

void RenderQueue::enqueue(std::function<void()> fn) 
{
    if (queue.size() >= 100) {
        queue.clear();
    }
    queue.push_back(fn);
}

void RenderQueue::renderAll() 
{
    for (auto& op : queue) {
        op();
    }
    queue.clear();
}

SingleScreenTransparentWindow::SingleScreenTransparentWindow(std::string title, XY size, u32 flags)
{
    flags |= this->defaultWindowFlags();
    wd = SDL_CreateWindow(title.c_str(), size.x, size.y, flags);
    if (wd != NULL) {
        platformWindowCreated(this);
    }
    windowID = SDL_GetWindowID(wd);
    tryCreateRenderer();
    setWindowTitle(title);
    setVsync(g_config.vsync);
    setCustomWindowFrame(this->shouldUseCustomWindowFrame());
    SDL_GetWindowSize(wd, &size.x, &size.y);
    unscaledWindowSize = scaledWindowSize = size;
    if (g_config.autoViewportScale) {
        autoViewportScale();
    }
    blurBuffer = new WindowBlurBuffer(this);
    setCustomWindowFrame(this->shouldUseCustomWindowFrame());
}

std::vector<std::string> SingleScreenTransparentWindow::getRenderDrivers()
{
    auto baseDrivers = VSPWindow::getRenderDrivers();
    std::vector<std::string> transparentBlacklist = {
        "direct3d12",       //fails to create
        "direct3d",         //does not support transparency
        "vulkan",           //does not support transparency
        "gpu"               //fails to create
    };
    for (auto& s : transparentBlacklist) {
        auto f = std::find(baseDrivers.begin(), baseDrivers.end(), s);
        if (f != baseDrivers.end()) {
            std::erase(baseDrivers, *f);
        }
    }
    return baseDrivers;
}

void SingleScreenTransparentWindow::setIgnoreMouseInput(bool ignoreMouseInput)
{
    //loginfo(frmt("setIgnoreMouseInput {}", ignoreMouseInput));
    ignoreMouse = ignoreMouseInput;
    SDL_SetWindowAlwaysOnTop(wd, ignoreMouseInput);
#if VSP_PLATFORM == VSP_PLATFORM_WIN32
    SDL_PropertiesID p = SDL_GetWindowProperties(wd);
    HWND h = (HWND)SDL_GetPointerProperty(p, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (prevStyle == 0) {
        prevStyle = GetWindowLong(h, GWL_EXSTYLE);
    }

    if (ignoreMouseInput) {
        SetWindowLong(h, GWL_EXSTYLE, prevStyle | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST);
    }
    else {
        SetWindowLong(h, GWL_EXSTYLE, prevStyle);
    }
#endif
}
