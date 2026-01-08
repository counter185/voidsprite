#include "json/json.hpp"

#include "maineditor.h"
#include "multiwindow.h"
#include "MainEditorPalettized.h"
#include "FontRenderer.h"
#include "EditorBrushPicker.h"
#include "EditorLayerPicker.h"
#include "EditorColorPicker.h"
#include "EditorTouchToggle.h"
#include "CollapsableDraggablePanel.h"
#include "ScreenWideNavBar.h"
#include "ScreenWideActionBar.h"
#include "Notification.h"
#include "EditorSpritesheetPreview.h"
#include "FileIO.h"
#include "Gamepad.h"
#include "LayerPalettized.h"
#include "UICheckbox.h"
#include "TooltipsLayer.h"
#include "ee_creature.h"
#include "BaseFilter.h"
#include "RenderFilter.h"
#include "PanelReference.h"
#include "keybinds.h"
#include "background_operation.h"
#include "brush/BaseBrush.h"
#include "PanelPreview.h"
#include "UIStackPanel.h"
#include "UndoStack.h"
#include "EditorFramePicker.h"

#include "TilemapPreviewScreen.h"
#include "MinecraftSkinPreviewScreen.h"
#include "RPG2KTilemapPreviewScreen.h"
#include "SpritesheetPreviewScreen.h"
#include "NineSegmentPatternEditorScreen.h"
#include "MinecraftBlockPreviewScreen.h"
#include "ViewSessionScreen.h"

#include "PopupIntegerScale.h"
#include "PopupTextBox.h"
#include "PopupSetEditorPixelGrid.h"
#include "PopupTileGeneric.h"
#include "PopupYesNo.h"
#include "PopupGlobalConfig.h"
#include "PopupPickColor.h"
#include "PopupApplyFilter.h"
#include "PopupExportScaled.h"
#include "PopupFilePicker.h"
#include "PopupContextMenu.h"
#include "PopupSetupNetworkCanvas.h"
#include "PopupFreeformTransform.h"
#include "PopupChooseAction.h"

#include "discord_rpc.h"

#if defined(__unix__)
#include <time.h>
#elif defined(_WIN32)
#include <time.h>
#else
#include <ctime>
#include "UIStackPanel.h"
#endif

using namespace nlohmann;

SDL_Rect MainEditor::getPaddedTilePosAndDimensions(XY tilePos)
{
    XY tileDim = xyEqual(tileDimensions, { 0,0 }) ? canvas.dimensions : tileDimensions;

    XY origin = {
        tilePos.x * tileDim.x,
        tilePos.y * tileDim.y
    };
    return SDL_Rect{
        origin.x,
        origin.y,
        tileDim.x - ixmax(0,tileGridPaddingBottomRight.x),
        tileDim.y - ixmax(0,tileGridPaddingBottomRight.y)
    };
}

XY MainEditor::getPaddedTileDimensions()
{
    XY ret = XY{
        tileDimensions.x - ixmax(0,tileGridPaddingBottomRight.x),
        tileDimensions.y - ixmax(0,tileGridPaddingBottomRight.y)
    };
    return { 
        ret.x <= 0 ? tileDimensions.x : ret.x,
        ret.y <= 0 ? tileDimensions.y : ret.y
    };
}

MainEditor::MainEditor(XY dimensions) {

    canvas.dimensions = { dimensions.x, dimensions.y };
    //canvasCenterPoint = XY{ texW / 2, texH / 2 };
    getLayerStack().push_back(new Layer(canvas.dimensions.x, canvas.dimensions.y));
    FillTexture();

    setUpWidgets();
    recenterCanvas();
    initLayers();
}
MainEditor::MainEditor(SDL_Surface* srf) {

    //todo i mean just use MainEditor(Layer*) here
    canvas.dimensions = { srf->w, srf->h };

    Layer* nlayer = new Layer(canvas.dimensions.x, canvas.dimensions.y);
    getLayerStack().push_back(nlayer);
    SDL_ConvertPixels(srf->w, srf->h, srf->format, srf->pixels, srf->pitch, SDL_PIXELFORMAT_ARGB8888, nlayer->pixels32(), canvas.dimensions.x*4);

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditor::MainEditor(Layer* layer)
{
    canvas.dimensions = { layer->w, layer->h };
    getLayerStack().push_back(layer);
    loadSingleLayerExtdata(layer);

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditor::MainEditor(std::vector<Layer*> layers)
{
    canvas.dimensions = { layers[0]->w, layers[0]->h };
    this->getLayerStack() = layers;

    setUpWidgets();
    recenterCanvas();
    initLayers();
    
}

MainEditor::MainEditor(std::vector<Frame*> fframes)
{
    Layer*& firstLayer = fframes.front()->layers.front();
    canvas.dimensions = { firstLayer->w, firstLayer->h };
    for (auto*& f : frames) {
        delete f;
    }
    frames = fframes;

    setUpWidgets();
    recenterCanvas();
    initLayers();
    
}

MainEditor::~MainEditor() {
    discardUndoStack();
    discardRedoStack();
    endNetworkSession();
    tracked_destroyTexture(frameFB.second);
    for (Frame*& frame : frames) {
        delete frame;
    }
    for (BaseScreen*& unopenedScreen : hintOpenScreensInInteractiveMode) {
        delete unopenedScreen;
    }
}


void MainEditor::render() {
    SDL_SetRenderDrawColor(g_rd, backgroundColor.r/6*5, backgroundColor.g/6*5, backgroundColor.b/6*5, 255);
    SDL_RenderClear(g_rd);
    DrawBackground();

    SDL_Rect canvasRenderRect = canvas.getCanvasOnScreenRect();
    //render references under the canvas
    for (auto& refPanel : openReferencePanels) {
        if (refPanel->currentMode == REFERENCE_UNDER_CANVAS) {
            Layer* p = refPanel->getLayer();
            SDL_Rect fit = fitInside(canvasRenderRect, { 0,0, p->w, p->h });
            p->render(fit, (u8)(refPanel->opacity * 255));
        }
    }
    
    //prepare frame framebuffer
    if (frameFB.first != g_rd || !xyEqual(frameFBSize, canvas.dimensions)) {
        SDL_DestroyTexture(frameFB.second);
        frameFB.second = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, canvas.dimensions.x, canvas.dimensions.y);
        frameFBSize = canvas.dimensions;
        frameFB.first = g_rd;
    }
    //render all layers
    framesMutex.lock();
    for (Layer* imgLayer : getCurrentFrame()->layers) {
        bool isCurrentActiveLayer = imgLayer == getCurrentLayer();
        if (!imgLayer->hidden) {
            uint8_t alpha = imgLayer->layerAlpha;
            SDL_Rect renderRect = canvasRenderRect;
            double layerFadeIn = layerSwitchTimer.started ? XM1PW3P1(layerSwitchTimer.percentElapsedTime(1300)) : 1.0;
            if (variantSwitchTimer.started && isCurrentActiveLayer) {
                double percent = XM1PW3P1(variantSwitchTimer.percentElapsedTime(400));
                renderRect.x += (lastVariantSwitchWasRight ? 1.0 : -1.0) * (40.0 * (1.0 - percent));
                layerFadeIn *= percent;
            }
            if (!isCurrentActiveLayer) {
                layerFadeIn = 1.0;
            }
            imgLayer->render(renderRect, (uint8_t)(alpha * layerFadeIn));
        }
    }
    //render backtrace frames
    for (int backFrame = 0; backFrame < backtraceFrames; backFrame++) {
        int targetIndex = activeFrame - (backFrame + 1);
        if (targetIndex >= 0) {
            Frame* f = frames[targetIndex];
            renderFrameTo(f, frameFB.second);
            double alpha = traceOpacity * (1.0 - (double)backFrame / backtraceFrames);
            SDL_Color colorMod = traceColorMod ? uint32ToSDLColor(g_config.backtraceColor) : SDL_Color{ 255,255,255,255 };
            SDL_SetTextureColorMod(frameFB.second, colorMod.r, colorMod.g, colorMod.b);
            SDL_SetTextureAlphaMod(frameFB.second, (u8)(alpha * 255));
            SDL_RenderCopy(g_rd, frameFB.second, NULL, &canvasRenderRect);
        }
        else {
            break;
        }
    }
    //render fwdtrace frames
    for (int fwdFrame = 0; fwdFrame < fwdtraceFrames; fwdFrame++) {
        int targetIndex = activeFrame + (fwdFrame + 1);
        if (targetIndex < (int)frames.size()) {
            Frame* f = frames[targetIndex];
            renderFrameTo(f, frameFB.second);
            double alpha = traceOpacity * (1.0 - (double)fwdFrame / fwdtraceFrames);
            SDL_Color colorMod = traceColorMod ? uint32ToSDLColor(g_config.fwdtraceColor) : SDL_Color{ 255,255,255,255 };
            SDL_SetTextureColorMod(frameFB.second, colorMod.r, colorMod.g, colorMod.b);
            SDL_SetTextureAlphaMod(frameFB.second, (u8)(alpha * 255));
            SDL_RenderCopy(g_rd, frameFB.second, NULL, &canvasRenderRect);
        }
        else {
            break;
        }
    }
    framesMutex.unlock();
    for (auto& refPanel : openReferencePanels) {
        if (refPanel->currentMode == REFERENCE_OVER_CANVAS) {
            Layer* p = refPanel->getLayer();
            SDL_Rect fit = fitInside(canvasRenderRect, { 0,0, p->w, p->h });
            p->render(fit, (u8)(refPanel->opacity * 255));
        }
    }

    //draw a separate 1x1 grid if the scale is >= 1600%
    if (canvas.scale >= 10) {

        uint8_t tileGridAlpha = canvas.scale < 16 ? 0x10 * ((canvas.scale - 9) / 7.0) : 0x10;
        SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, tileGridAlpha);
        canvas.drawTileGrid({ 1,1 });

    }

    drawTileGrid();
    drawRowColNumbers();
    drawNetworkCanvasClients();
    drawIsolatedFragment();
    drawSymmetryLines();
    renderGuidelines();
    drawSplitSessionFragments();
    drawZoomLines();

    //draw tile repeat preview
    if (qModifier || (lockedTilePreview.x >= 0 && lockedTilePreview.y >= 0)) {
        
        XY mouseInCanvasPoint = canvas.screenPointToCanvasPoint({ g_mouseX, g_mouseY });
        if (!qModifier || (qModifier && canvas.pointInCanvasBounds(mouseInCanvasPoint))) {

            XY tileDim = tileDimensions.x != 0 && tileDimensions.y != 0 ? tileDimensions : canvas.dimensions;

            XY tilePosition = !qModifier ? lockedTilePreview :
                XY{
                    mouseInCanvasPoint.x / tileDim.x,
                    mouseInCanvasPoint.y / tileDim.y
                };

            SDL_Rect paddedTileRect = getPaddedTilePosAndDimensions(tilePosition);
            SDL_Rect tileRect = {
                canvas.currentDrawPoint.x + paddedTileRect.x * canvas.scale,
                canvas.currentDrawPoint.y + paddedTileRect.y * canvas.scale,
                paddedTileRect.w * canvas.scale,
                paddedTileRect.h * canvas.scale
            };
            SDL_Rect canvasClipRect = paddedTileRect;

            for (int yy = -1; yy <= 1; yy++) {
                for (int xx = -1; xx <= 1; xx++) {
                    if (yy == 0 && xx == 0) {
                        continue;
                    }

                    for (Layer* imgLayer : getLayerStack()) {
                        if (!imgLayer->hidden) {
                            uint8_t alpha = imgLayer->layerAlpha;
                            XY position = { tileRect.x + (xx * canvas.scale * paddedTileRect.w),
                                tileRect.y + (yy * canvas.scale * paddedTileRect.h)};
                            SDL_Rect finalTileRect = { position.x, position.y, tileRect.w, tileRect.h };
                            imgLayer->render(finalTileRect, canvasClipRect, alpha);
                        }
                    }
                }
            }

            //lock animation
            if (tileLockTimer.started && (lockedTilePreview.x >= 0 && lockedTilePreview.y >= 0)) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
                double anim = 1.0 - XM1PW3P1(tileLockTimer.percentElapsedTime(300));
                drawLine(XY{ tileRect.x, tileRect.y }, XY{ tileRect.x + tileRect.w, tileRect.y }, anim);
                drawLine(XY{ tileRect.x, tileRect.y }, XY{ tileRect.x, tileRect.y + tileRect.h }, anim);
                drawLine(XY{ tileRect.x + tileRect.w, tileRect.y + tileRect.h }, XY{ tileRect.x, tileRect.y + tileRect.h }, anim);
                drawLine(XY{ tileRect.x + tileRect.w, tileRect.y + tileRect.h }, XY{ tileRect.x + tileRect.w, tileRect.y }, anim);
            }
        }
    }

    renderComments();

    renderUndoStack();

    if (currentBrush != NULL) {
        currentBrush->renderOnCanvas(this, canvas.scale);
    }    

    if (wxsManager.anyFocused() && navbar->focused) {
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        SDL_Rect fullscreen = {0, 0, g_windowW, g_windowH};
        SDL_RenderFillRect(g_rd, &fullscreen);
    }

    if (!hideUI) {
        DrawForeground();
        wxsManager.renderAll();
    }

    if (eraserMode) {
        SDL_Rect eraserRect = { g_mouseX + 6, g_mouseY - 30, 28, 28 };
        SDL_SetTextureAlphaMod(g_iconEraser->get(), 0xa0);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x60);
        SDL_RenderFillRect(g_rd, &eraserRect);
        SDL_RenderCopy(g_rd, g_iconEraser->get(), NULL, &eraserRect);
    }
    if (currentPattern != NULL && currentPattern != g_patterns[0]) {
        SDL_Rect patternRect = { g_mouseX + 38, g_mouseY - 30, 22, 22 };
        SDL_SetTextureAlphaMod(currentPattern->cachedIcon->get(), 0xa0);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x60);
        SDL_RenderFillRect(g_rd, &patternRect);
        SDL_RenderCopy(g_rd, currentPattern->cachedIcon->get(), NULL, &patternRect);
    }
    if (g_config.showPenPressure && leftMouseHold && penPressure > 0 && penPressure < 1) {
        SDL_Rect pressureIndicatorBounds = { g_mouseX - 50, g_mouseY - 60, 100, 15 };
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xa0);
        SDL_RenderFillRect(g_rd, &pressureIndicatorBounds);
        SDL_Rect pressureBar = offsetRect(pressureIndicatorBounds, -2);
        pressureBar.w *= penPressure;
        SDL_SetRenderDrawColor(g_rd, 255,255,255, 0xa0);
        SDL_RenderFillRect(g_rd, &pressureBar);
    }
}

void MainEditor::tick() {

    if (hintOpenScreensInInteractiveMode.size() > 0) {
        for (BaseScreen*& s : hintOpenScreensInInteractiveMode) {
            g_addScreen(s);
        }
        hintOpenScreensInInteractiveMode.clear();
    }

    mainThreadOps.process();

    if (frameAnimationPlaying && frameAnimMSPerFrame > 0) {
        u64 elapsed = frameAnimationStartTimer.elapsedTime();
        elapsed /= frameAnimMSPerFrame;
        elapsed %= frames.size();
        int targetFrame = (int)elapsed;
        if (targetFrame != activeFrame) {
            switchFrame(targetFrame);
        }
    }

    if (g_windowFocused) {
        u64 timestampNow = SDL_GetTicks64() / 1000;
        if (lastTimestamp != timestampNow) {
            lastTimestamp = timestampNow;
            editTime++;
        }
    }

    if (abs(g_gamepad->gamepadLSX) > 0.05f || abs(g_gamepad->gamepadLSY) > 0.05f) {
        canvas.panCanvas({
            (int)(-g_gamepad->gamepadLSX * 10),
            (int)(-g_gamepad->gamepadLSY * 10)
        });
        RecalcMousePixelTargetPoint(g_windowW / 2, g_windowH / 2);
        currentBrush->mouseMotion(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
    }

    canvas.lockToScreenBounds();

    //fuck it we ball
    if (!compactEditor && layerPicker != NULL) {
        layerPicker->position.x = g_windowW - 260;
    }

    if (isPalettized) {
        MainEditorPalettized* thisUpCast = (MainEditorPalettized*)this;
        uint32_t col = 0;
        if (thisUpCast->pickedPaletteIndex >= 0 && thisUpCast->pickedPaletteIndex < thisUpCast->palette.size()) {
            col = thisUpCast->palette[thisUpCast->pickedPaletteIndex];
        }
        g_gamepad->SetLightbar((col >> 16) & 0xff, (col >> 8) & 0xff, col & 0xff);
    }
    else {
        g_gamepad->SetLightbar((pickedColor >> 16) & 0xff, (pickedColor >> 8) & 0xff, pickedColor & 0xff);
    }

    tickAutosave();
    timerSinceLastSave.startIfNotStarted();

    if (networkCanvasBroadcastRPC) {
        RPCLobbyInfo lobbyInfo;
        lobbyInfo.id = networkCanvasLobbyID;
        lobbyInfo.isPrivate = networkCanvasRPCPrivate;
        lobbyInfo.joinSecret = networkCanvasRPCAddress;
        networkClientsListMutex.lock();
        lobbyInfo.currentSize = networkClients.size();
        networkClientsListMutex.unlock();
        lobbyInfo.maxSize = 16;
        g_pushRPCLobbyInfo(lobbyInfo);
    }

    if (networkRunning && (!networkCanvasLastLANBroadcast.started || networkCanvasLastLANBroadcast.elapsedTime() > 3000)) {
        networkCanvasBroadcastToLAN();
    }

    if (closeNextTick) {
        if (!g_currentWindow->hasPopupsOpen()) {
            g_closeScreen(this);
        }
    }
}

void MainEditor::DrawBackground()
{
    static Fill fillPrimary = visualConfigFill("maineditor/bg");
    static Fill fillAlt = visualConfigFill("maineditor/bg_alt");
    //uint32_t colorBG1 = 0xFF000000 | (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x000000 : 0xDFDFDF);
    //uint32_t colorBG2 = 0xFF000000 | (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x202020 : 0x808080);
    //renderGradient({ 0,0, g_windowW, g_windowH }, colorBG1, colorBG1, colorBG1, colorBG2);
    (usingAltBG() ? fillAlt : fillPrimary).fill({ 0,0,g_windowW,g_windowH });

    uint64_t bgtimer = g_config.animatedBackground >= 3 ? 0 : SDL_GetTicks64();
    if (g_config.animatedBackground == 1 || g_config.animatedBackground == 3) {
        int lineX = 400;
        for (int x = 40 + (bgtimer % 5000 / 5000.0 * 60); x < g_windowW + lineX; x += 60) {
            SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x40 / (g_windowFocused ? 1 : 3));
            SDL_RenderDrawLine(g_rd, x, 0, x - lineX, g_windowH);
            if (g_windowFocused) {
                SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x40 / 2);
                SDL_RenderDrawLine(g_rd, x - 1, 0, x - lineX - 1, g_windowH);
                SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x40 / 3);
                SDL_RenderDrawLine(g_rd, x - 2, 0, x - lineX - 2, g_windowH);
            }

            SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x0d);
            SDL_RenderDrawLine(g_rd, g_windowW - x, 0, g_windowW - x + lineX / 4 * 6, g_windowH);
            SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x0d/2);
            SDL_RenderDrawLine(g_rd, g_windowW - x - 1, 0, g_windowW - x + lineX / 4 * 6 - 1, g_windowH);
        }
    }
    else if (g_config.animatedBackground == 2 || g_config.animatedBackground == 4) {
        //draw lines in the background
        int lineX = 400;
        for (int x = 40 + (bgtimer % 5000 / 5000.0 * 60); x < g_windowW + lineX; x += 60) {
            XY l1 = { x, 0 };
            XY l2 = { x - lineX, g_windowH };
            XY ll1 = xyAdd(l1, { 4, 0 });
            XY ll2 = xyAdd(l2, { 4, 0 });
            uint32_t c = PackRGBAtoARGB(0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x40);
            renderGradient(l1, ll1, l2, ll2, c, modAlpha(c, 0), c, modAlpha(c, 0));

            l1 = { g_windowW - x, 0 };
            l2 = { g_windowW - x + lineX / 4 * 6, g_windowH };
            ll1 = xyAdd(l1, { 16, 0 });
            ll2 = xyAdd(l2, { 16, 0 });
            c = PackRGBAtoARGB(0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x06);
            renderGradient(l1, ll1, l2, ll2, c, modAlpha(c, 0), c, modAlpha(c, 0));
            ll1 = xySubtract(l1, { 16,0 });
            ll2 = xySubtract(l2, { 16,0 });
            renderGradient(l1, ll1, l2, ll2, c, modAlpha(c, 0), c, modAlpha(c, 0));
            //SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x0d);
            //SDL_RenderDrawLine(g_rd, g_windowW - x, 0, g_windowW - x + lineX / 4 * 6, g_windowH);
        }
    }

    //draw border around canvas
    SDL_Color borderColor = {0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0xff};
    canvas.drawCanvasOutline(6, borderColor);
}

void MainEditor::drawSymmetryLines() {
    if (symmetryEnabled[0]) {
        int symXPos = symmetryPositions.x / 2;
        bool symXMiddle = symmetryPositions.x % 2;
        int lineDrawXPoint = canvas.currentDrawPoint.x + symXPos * canvas.scale + (symXMiddle ? canvas.scale /2 : 0);

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
        SDL_RenderDrawLine(g_rd, lineDrawXPoint, 0, lineDrawXPoint, g_windowH);
    }
    if (symmetryEnabled[1]) {
        int symYPos = symmetryPositions.y / 2;
        bool symYMiddle = symmetryPositions.y % 2;
        int lineDrawYPoint = canvas.currentDrawPoint.y + symYPos * canvas.scale + (symYMiddle ? canvas.scale /2 : 0);

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
        SDL_RenderDrawLine(g_rd,0, lineDrawYPoint, g_windowW, lineDrawYPoint);
    }
}

void MainEditor::evalIsolatedFragmentRender()
{
    renderedIsolatedFragmentPoints.clear();
    isolatedFragment.forEachScanline([&](ScanlineMapElement sme) {
        if (sme.size.x == 0) {
            return;
        }
        for (int x = 0; x < sme.size.x; x++) {
            XY onCanvasPoint = xyAdd(sme.origin, { x,0 });
            IsolatedFragmentPoint newRenderPoint;
            bool hasPointAbove = isolatedFragment.pointExists(xySubtract(onCanvasPoint, { 0,1 }));
            bool hasPointBelow = isolatedFragment.pointExists(xyAdd(onCanvasPoint, { 0,1 }));
            newRenderPoint.directions |= !hasPointAbove ? FRAGMENT_DIRECTION_UP : 0;
            newRenderPoint.directions |= !hasPointBelow ? FRAGMENT_DIRECTION_DOWN : 0;
            newRenderPoint.directions |= (x == 0) ? FRAGMENT_DIRECTION_LEFT : 0;
            newRenderPoint.directions |= (x == (sme.size.x - 1)) ? FRAGMENT_DIRECTION_RIGHT : 0;
            if (newRenderPoint.directions != 0) {
                newRenderPoint.directions2x = newRenderPoint.directions;
                newRenderPoint.directions2x &= ((x & 1) != 0) ? ~(FRAGMENT_DIRECTION_UP | FRAGMENT_DIRECTION_DOWN) : 0xff;
                newRenderPoint.directions2x &= (hasPointAbove && hasPointBelow && (sme.origin.y & 1) != 0) ? ~(FRAGMENT_DIRECTION_LEFT | FRAGMENT_DIRECTION_RIGHT) : 0xff;

                newRenderPoint.directions4x = newRenderPoint.directions2x;
                newRenderPoint.directions4x &= ((x & 0b11) != 0) ? ~(FRAGMENT_DIRECTION_UP | FRAGMENT_DIRECTION_DOWN) : 0xff;
                newRenderPoint.directions4x &= (hasPointAbove && hasPointBelow && (sme.origin.y & 0b11) != 0) ? ~(FRAGMENT_DIRECTION_LEFT | FRAGMENT_DIRECTION_RIGHT) : 0xff;

                newRenderPoint.onCanvasPixelPosition = onCanvasPoint;
                renderedIsolatedFragmentPoints.push_back(newRenderPoint);
            }
        }
    });
    shouldUpdateRenderedIsolatedFragmentPoints = false;
}

void MainEditor::drawIsolatedFragment()
{
    if (isolateEnabled) {

        int xincrement = canvas.scale == 1 ? 3 : canvas.scale < 6 ? 2 : 1;
        int vincrement = canvas.scale <= 2 ? 4 : canvas.scale <= 5 ? 2 : 1;

        int scale = canvas.scale <= 4 ? (canvas.scale == 1 ? 2 : 1) : 0;

        if (shouldUpdateRenderedIsolatedFragmentPoints) {
            evalIsolatedFragmentRender();
        }
        for (IsolatedFragmentPoint& fp : renderedIsolatedFragmentPoints) {
            u8 scaledDirections = scale == 2 ? fp.directions4x :
                                  scale == 1 ? fp.directions2x 
                                  : fp.directions;
            if (scaledDirections == 0) {
                continue;
            }
            XY onScreenPoint = canvas.canvasPointToScreenPoint(fp.onCanvasPixelPosition);
            SDL_Rect onScreenPointRect = canvas.canvasRectToScreenRect({ fp.onCanvasPixelPosition.x, fp.onCanvasPixelPosition.y, 1,1 });
            if (onScreenPoint.y > g_windowH) {
                break;
            }
            else if (onScreenPointRect.y + onScreenPointRect.h < 0) {
                continue;
            }
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
            if (scaledDirections & FRAGMENT_DIRECTION_UP) {
                drawLine(onScreenPoint, xyAdd(onScreenPoint, { onScreenPointRect.w / (scale ? 1 : 2), 0 }));
            }
            if (scaledDirections & FRAGMENT_DIRECTION_DOWN) {
                drawLine(xyAdd(onScreenPoint, {0, onScreenPointRect.h}), xyAdd(onScreenPoint, { onScreenPointRect.w / (scale ? 1 : 2), onScreenPointRect.h }));
            }
            if (scaledDirections & FRAGMENT_DIRECTION_LEFT) {
                drawLine(onScreenPoint, xyAdd(onScreenPoint, { 0, onScreenPointRect.h / (scale ? 1 : 2) }));
            }
            if (scaledDirections & FRAGMENT_DIRECTION_RIGHT) {
                drawLine(xyAdd(onScreenPoint, { onScreenPointRect.w, 0 }), xyAdd(onScreenPoint, { onScreenPointRect.w, onScreenPointRect.h / (scale ? 1 : 2) }));
            }
        }
    }
}

void MainEditor::drawTileGrid()
{
    SDL_Rect canvasRenderRect = canvas.getCanvasOnScreenRect();
    /*canvasRenderRect.w = texW * scale;
    canvasRenderRect.h = texH * scale;
    canvasRenderRect.x = canvasCenterPoint.x;
    canvasRenderRect.y = canvasCenterPoint.y;*/

    //draw tile lines
    SDL_Color c = { 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, tileGridAlpha };
    SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, c.a);
    canvas.drawTileGrid(tileDimensions);

    SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, c.a / 3 * 2);
    //draw tile grid padding
    if (tileDimensions.x != 0) {
        int dx = canvasRenderRect.x;
        while (dx < g_windowW && dx < canvasRenderRect.x + canvasRenderRect.w) {
            dx += tileDimensions.x * canvas.scale;
            if (tileGridPaddingBottomRight.x > 0) {
                SDL_RenderDrawLine(g_rd, dx - tileGridPaddingBottomRight.x * canvas.scale , canvasRenderRect.y, dx - tileGridPaddingBottomRight.x * canvas.scale, canvasRenderRect.y + canvasRenderRect.h);
            }
        }
    }
    if (tileDimensions.y != 0) {
        int dy = canvasRenderRect.y;
        while (dy < g_windowH && dy < canvasRenderRect.y + canvasRenderRect.h) {
            dy += tileDimensions.y * canvas.scale;
            if (tileGridPaddingBottomRight.y > 0) {
                SDL_RenderDrawLine(g_rd, canvasRenderRect.x, dy - tileGridPaddingBottomRight.y * canvas.scale, canvasRenderRect.x + canvasRenderRect.w, dy - tileGridPaddingBottomRight.y * canvas.scale);
            }
        }
    }
}

void MainEditor::renderGuidelines() {
    /*README - FUNCTIONALITY DOCUMENTATION
    * [1] Draw / place vertical or horizontal / diagonal ones at any point
    * [2] Snap to grid lines 
    * [3] "breakpoint" system : shift-lmb to create a "point" on a guideline if a grid is present. the guideline may alter direction after that point
    * [4] option to stop rendering / render specific colour / render all like comments?
    */
    for (Guideline& guide : guidelines) {

        u8  a = (pickedColor >> 24) & 0xff,
            r = (pickedColor >> 16) & 0xff,
            g = (pickedColor >> 8) & 0xff,
            b = pickedColor & 0xff;

        hsv h = rgb2hsv({ r / 255.0, g / 255.0, b / 255.0 });
        h.v = dxmax(0.2, h.v);
        h.s /= 2;
        rgb rgbval = hsv2rgb(h);
        SDL_Color c = rgb2sdlcolor(rgbval);

        SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, 120);
        if (!guide.vertical) {
            //horizontal
            int gYPos = guide.position / 2;
            bool gYMiddle = guide.position % 2;
            int lineDrawYPoint = canvas.currentDrawPoint.y + gYPos * canvas.scale + (gYMiddle ? canvas.scale / 2 : 0);
            SDL_RenderDrawLine(g_rd, 0, lineDrawYPoint, g_windowW, lineDrawYPoint);
        }
        if (guide.vertical) {
            //vertical
            int gXPos = guide.position / 2;
            bool gXMiddle = guide.position % 2;
            int lineDrawXPoint = canvas.currentDrawPoint.x + gXPos * canvas.scale + (gXMiddle ? canvas.scale / 2 : 0);
            SDL_RenderDrawLine(g_rd, lineDrawXPoint, 0, lineDrawXPoint, g_windowH);
        }
    }

}

void MainEditor::drawSplitSessionFragments()
{
    TooltipsLayer localTtp;
    if (splitSessionData.set) {
        for (SplitSessionImage& img : splitSessionData.images) {
            SDL_Rect r = canvas.canvasRectToScreenRect({
                img.positionInOverallImage.x, img.positionInOverallImage.y,
                img.dimensions.x, img.dimensions.y
                });
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
            SDL_RenderDrawRect(g_rd, &r);

            if (pointInBox({ g_mouseX, g_mouseY }, r)) {
                img.animTimer.startIfNotStarted();
                localTtp.addTooltip(Tooltip{ {ixmax(0,r.x), ixmax(30 + actionbar->wxHeight, r.y - 30)},
                                          img.originalFileName,
                                          {0xff, 0xff, 0xff, 0xff},
                                          img.animTimer.percentElapsedTime(300)});
            }
            else {
                img.animTimer.stop();
            }
        }
    }
    localTtp.renderAll();
}

void MainEditor::drawZoomLines() {
    if (zoomKeyHeld) {
        XY origin = xyAdd(zoomOrigin, {30,0});
        SDL_Rect r = {origin.x, 0, 50 * XM1PW3P1(zoomKeyTimer.percentElapsedTime(500)), g_windowH};
        renderGradient(r, 0xFF000000, 0x00000000, 0xFF000000, 0x00000000);
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);

        drawLine({origin.x + 5, origin.y}, {origin.x + 20, origin.y}, XM1PW3P1(zoomKeyTimer.percentElapsedTime(400)));
        int yy = zoomPixelStep;
        int i = 0;
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x60);
        while (origin.y + yy < g_windowH || origin.y - yy >= 0) {
            i++;
            drawLine({origin.x + 5, origin.y + yy}, {origin.x + 15, origin.y + yy}, XM1PW3P1(zoomKeyTimer.percentElapsedTime(200, 50 * i)));
            drawLine({origin.x + 15, origin.y - yy}, {origin.x + 5, origin.y - yy}, XM1PW3P1(zoomKeyTimer.percentElapsedTime(200, 50 * i)));
            yy += zoomPixelStep;
        }
    }
}

void MainEditor::drawRowColNumbers()
{
    if (canvas.scale >= 20) {
        const u8 opacity = 0x50;
        SDL_Color textColor = backgroundColor.r == 255 ? SDL_Color{ 0,0,0,opacity } : SDL_Color{ 255,255,255,opacity };
        SDL_Rect canvasDrawRect = canvas.getCanvasOnScreenRect();
        bool drawHorizontal = canvasDrawRect.y + canvasDrawRect.h < g_windowH;
        int indexOffset = g_config.rowColIndexesStartAt1 ? 1 : 0;
        if (canvasDrawRect.x + canvasDrawRect.w < g_windowW) {
            //vertical
            XY origin = {canvasDrawRect.x + canvasDrawRect.w + 2, canvasDrawRect.y};
            for (int i = 0; i < canvas.dimensions.y + (drawHorizontal ? 0 : 1) && origin.y < g_windowH; i++) {
                if (origin.y >= 0) {
                    int fh = g_fnt->StatStringDimensions(std::to_string(i+ indexOffset)).y;
                    int dy = (canvas.scale / 2) - (fh / 2);
                    g_fnt->RenderString(std::to_string(i+ indexOffset), origin.x, origin.y + dy, textColor);
                }
                origin.y += canvas.scale;
            }
        }
        if (drawHorizontal) {
            //horizontal
            XY origin = {canvasDrawRect.x, canvasDrawRect.y + canvasDrawRect.h + 2};
            for (int i = 0; i < canvas.dimensions.x + 1 && origin.x < g_windowW; i++) {
                if (origin.x >= 0) {
                    int fw = g_fnt->StatStringDimensions(std::to_string(i+ indexOffset)).x;
                    int dx = (canvas.scale / 2) - (fw / 2);
                    g_fnt->RenderString(std::to_string(i+ indexOffset), origin.x + dx, origin.y, textColor);
                }
                origin.x += canvas.scale;
            }
        }
    }
}

void MainEditor::drawNetworkCanvasClients()
{
    networkClientsListMutex.lock();
    for (auto& client : networkClients) {
        if (thisClientInfo != NULL && client->uid == thisClientInfo->uid) {
            //skip myself
            continue;
        }
        SDL_Rect clientRect = canvas.canvasRectToScreenRect({ client->cursorPosition.x, client->cursorPosition.y, 1,1 });
        SDL_Color color = uint32ToSDLColor(0xFF000000 | client->clientColor);
        SDL_SetRenderDrawColor(g_rd, color.r, color.g, color.b, 255);
        SDL_RenderDrawRect(g_rd, &clientRect);
        if (xyDistance({ g_mouseX, g_mouseY }, { clientRect.x, clientRect.y }) < 20) {
            g_ttp->addTooltip(Tooltip{ {clientRect.x + 20, clientRect.y}, client->clientName, color, 1.0 });
        }

        SDL_SetRenderDrawColor(g_rd, color.r, color.g, color.b, 0xA0);
        clientRect = offsetRect(clientRect, 5);
        SDL_RenderDrawRect(g_rd, &clientRect);

        SDL_SetRenderDrawColor(g_rd, color.r, color.g, color.b, 0x40);
        clientRect = offsetRect(clientRect, 15);
        SDL_RenderDrawRect(g_rd, &clientRect);
    }
    networkClientsListMutex.unlock();
}

void MainEditor::inputMouseRight(XY at, bool down)
{
    RecalcMousePixelTargetPoint(at.x, at.y);
    if (currentBrush != NULL && currentBrush->overrideRightClick()) {
        if (down) {
            currentBrush->rightClickPress(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
        }
        else {
            currentBrush->rightClickRelease(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
        }
    }
    else {
        if (down) {
            bool pickFromWholeImage = !g_ctrlModifier;
            setActiveColor(!pickFromWholeImage ? getCurrentLayer()->getPixelAt(mousePixelTargetPoint)
                : pickColorFromAllLayers(mousePixelTargetPoint));
            playColorPickerVFX(pickFromWholeImage);
        }
    }
}

void MainEditor::DrawForeground()
{
    drawBottomBar();

    g_fnt->RenderString(frmt("{}x{} ({}%)", canvas.dimensions.x, canvas.dimensions.y, canvas.scale * 100), 2, g_windowH - 28, SDL_Color{255,255,255,0xa0});

    XY endpoint = g_fnt->RenderString(frmt("{}:{}", mousePixelTargetPoint.x, mousePixelTargetPoint.y), 200, g_windowH - 28, SDL_Color{255,255,255,0xd0});
    if (tileDimensions.x != 0 && tileDimensions.y != 0) {
        std::string s = frmt("(t{}:{} in{}:{})", 
            (int)floor(mousePixelTargetPoint.x / (float)tileDimensions.x), 
            (int)floor(mousePixelTargetPoint.y / (float)tileDimensions.y),
            mousePixelTargetPoint.x%tileDimensions.x,
            mousePixelTargetPoint.y%tileDimensions.y);
        endpoint = g_fnt->RenderString(s, endpoint.x + 5, endpoint.y, SDL_Color{ 255,255,255,0x90 });
    }

    if (currentBrush != NULL) {
        static std::string eraserModeText = TL("vsp.maineditor.erasermode");
        g_fnt->RenderString(frmt("{} {}", currentBrush->getName(), eraserMode ? eraserModeText : ""), ixmax(endpoint.x + 30, 370), g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
    }

    if (currentPattern != NULL) {
        g_fnt->RenderString(frmt("{}", currentPattern->getName()), 620, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
    }

    SDL_Color textColor = SDL_Color{ (u8)(255 - backgroundColor.r), (u8)(255 - backgroundColor.g), (u8)(255 - backgroundColor.b), 0x60};

    g_fnt->RenderString(secondsTimeToHumanReadable(editTime), 2, g_windowH - 28 * 2, { textColor.r, textColor.g, textColor.b, (u8)(g_windowFocused ? 0x50 : 0x30) });

    if (changesSinceLastSave != NO_UNSAVED_CHANGES) {
        std::string unsavedSymbol = changesSinceLastSave == CHANGES_RECOVERY_AUTOSAVED ? UTF8_EMPTY_DIAMOND : UTF8_DIAMOND;
        int fw = g_fnt->StatStringDimensions(unsavedSymbol).x;
        g_fnt->RenderString(unsavedSymbol, g_windowW - fw - 2, g_windowH - 70, SDL_Color{ textColor.r, textColor.g, textColor.b,0x70 });

        std::string timeString = frmt("({})", secondsTimeToHumanReadable(timerSinceLastSave.elapsedTime() / 1000));
        int fw2 = g_fnt->StatStringDimensions(timeString, 12).x;
        g_fnt->RenderString(timeString, g_windowW - fw - fw2 - 3, g_windowH - 70, SDL_Color{ textColor.r, textColor.g, textColor.b,0x60 }, 12);
    }
}

void MainEditor::renderComments()
{
    if (commentViewMode == COMMENTMODE_HIDE_ALL) {
        return;
    }
    static Fill commentFill = visualConfigFill("maineditor/comments/fill");
    TooltipsLayer localTtp;
    localTtp.border = false;
    localTtp.tooltipFill = commentFill;

    XY origin = canvas.currentDrawPoint;
    for (CommentData& c : getCommentStack()) {
        XY onScreenPosition = canvas.canvasPointToScreenPoint(c.position); //xyAdd(origin, { c.position.x * scale, c.position.y * scale });
        SDL_Rect iconRect = { onScreenPosition.x, onScreenPosition.y, 16, 16 };
        SDL_SetTextureAlphaMod(g_iconComment->get(g_rd), 0x80);
        SDL_RenderCopy(g_rd, g_iconComment->get(g_rd), NULL, &iconRect);
        if (commentViewMode == COMMENTMODE_SHOW_ALL || (commentViewMode == COMMENTMODE_SHOW_HOVERED && xyDistance(onScreenPosition, XY{ g_mouseX, g_mouseY }) < 32)) {
            if (!c.hovered) {
                c.animTimer.start();
                c.hovered = true;
            }
            //int yOffset = 16 * (1.0f- XM1PW3P1(c.animTimer.percentElapsedTime(200)));
            //g_fnt->RenderString(c.data, onScreenPosition.x + 17, onScreenPosition.y + yOffset, SDL_Color{ 255,255,255, (uint8_t)(0xff * c.animTimer.percentElapsedTime(200))});
            localTtp.addTooltip(Tooltip{ XY{onScreenPosition.x + 17, onScreenPosition.y}, c.data, {0xff,0xff,0xff,0xff}, c.animTimer.percentElapsedTime(200) });
        } else {
            c.hovered = false;
        }
    }
    localTtp.renderAll();
}

void MainEditor::renderUndoStack()
{
    XY center = { g_windowW / 2, g_windowH - 40 };

    double mouseDistance = xyDistance({ g_mouseX, g_mouseY }, center);

    int xOffset = undoTimer.started ? ((lastUndoWasRedo ? -1 : 1) * 5 * XM1PW3P1(undoTimer.percentElapsedTime(200))) : 0;
    center.x += xOffset;

    uint8_t lineShade = !usingAltBG() ? 0xff : 0x00;

    XY undoOrigin = xySubtract(center, { 10, 0 });
    XY redoOrigin = xyAdd(center, { 10, 0 });

    for (int x = 0; x < undoStack.size(); x++) {
        int xPos = undoOrigin.x - x * 10;
        SDL_SetRenderDrawColor(g_rd, lineShade, lineShade, lineShade, 0x30);
        SDL_RenderDrawLine(g_rd, xPos, center.y, xPos, center.y - 10);
    }

    for (int x = 0; x < redoStack.size(); x++) {
        int xPos = redoOrigin.x + x * 10;
        SDL_SetRenderDrawColor(g_rd, lineShade, lineShade, lineShade, 0x30);
        SDL_RenderDrawLine(g_rd, xPos, center.y, xPos, center.y - 10);
    }

    //center line
    SDL_SetRenderDrawColor(g_rd, lineShade, lineShade, lineShade, 0x50);
    SDL_RenderDrawLine(g_rd, center.x, center.y + 2, center.x, center.y - (15 * XM1PW3P1(undoTimer.started ? undoTimer.percentElapsedTime(200) : 1.0)));

    if (mouseDistance < 40) {
        static std::string undoText = TL("vsp.maineditor.undostack");
        int xw = g_fnt->StatStringDimensions(undoText, 14).x;
        g_fnt->RenderString(undoText, center.x - (xw / 2), center.y - 35, SDL_Color{ lineShade,lineShade,lineShade,0x50 }, 14);
    }
}

void MainEditor::initLayers()
{
    layerPicker->updateLayers();
}

void MainEditor::setUpWidgets()
{
    compactEditor = g_config.compactEditor;

    mainEditorKeyActions = {
        {
            SDL_SCANCODE_F,
            {
                makeNavbarSection(TL("vsp.nav.file"), g_iconNavbarTabFile,
                {
                    {SDL_SCANCODE_S, { TL("vsp.nav.save"), [this]() { this->trySaveImage(); } } },
                    {SDL_SCANCODE_D, { TL("vsp.maineditor.saveas"), [this]() { this->trySaveAsImage(); } } },
                    {SDL_SCANCODE_W, { TL("vsp.maineditor.nav.forceautosave"), [this]() { this->createRecoveryAutosave(); } } },
                    {SDL_SCANCODE_F, { TL("vsp.maineditor.nav.exportscaled"),
                            [this]() {
                                PopupExportScaled* popup = new PopupExportScaled(this);
                                popup->setCallbackListener(EVENT_MAINEDITOR_EXPORTSCALED, this);
                                g_addPopup(popup);
                            }
                        }
                    },
                    {SDL_SCANCODE_E, { TL("vsp.maineditor.exportpal"), [this]() { this->tryExportPalettizedImage(); } } },
                    {SDL_SCANCODE_A, { TL("vsp.maineditor.exportind"), [this]() { this->exportTilesIndividually(); } } },
                    {SDL_SCANCODE_R, { TL("vsp.maineditor.paledit"),
                            [this]() {
                                MainEditorPalettized* newEditor = this->toPalettizedSession();
                                if (newEditor != NULL) {
                                    g_addScreen(newEditor);
                                }
                            }
                        }
                    },
                    {SDL_SCANCODE_C, { TL("vsp.maineditor.copyflattoclipboard"), [this]() { this->copyImageToClipboard(); } } },
                    {SDL_SCANCODE_V, { TL("vsp.cmn.paste"), [this]() { this->promptPasteImageFromClipboard(); } } },
#if VSP_NETWORKING
                    {SDL_SCANCODE_N, { TL("vsp.maineditor.startcollab"), [this]() { promptStartNetworkSession(); } } },
#endif
                    {SDL_SCANCODE_P, { TL("vsp.maineditor.preference"), [this]() { g_addPopup(new PopupGlobalConfig()); } } },
                    {SDL_SCANCODE_X, { TL("vsp.cmn.close"), [this]() { this->requestSafeClose(); } } },
                })
            }
        },
        {
            SDL_SCANCODE_E,
            {
                makeNavbarSection(TL("vsp.maineditor.edit"), g_iconNavbarTabEdit, {
                    {SDL_SCANCODE_Z, { TL("vsp.maineditor.undo"), [this]() { this->undo(); } } },
                    {SDL_SCANCODE_R, { TL("vsp.maineditor.redo"), [this]() { this->redo(); } } },
                    {SDL_SCANCODE_X, { TL("vsp.maineditor.symx"),
                            [this]() {
                                this->symmetryEnabled[0] = !this->symmetryEnabled[0];
                            }
                        }
                    },
                    {SDL_SCANCODE_Y, { TL("vsp.maineditor.symy"),
                            [this]() {
                                this->symmetryEnabled[1] = !this->symmetryEnabled[1];
                            }
                        }
                    },
                    {SDL_SCANCODE_F, { TL("vsp.maineditor.flipallx"), [this]() { this->flipAllLayersOnX(); } } },
                    {SDL_SCANCODE_G, { TL("vsp.maineditor.flipally"), [this]() { this->flipAllLayersOnY(); } } },
                    {SDL_SCANCODE_C, { TL("vsp.maineditor.rescanv"),
                            [this]() {
                                g_addPopup(new PopupTileGeneric(this, TL("vsp.maineditor.rescanv"), "New canvas size:", this->canvas.dimensions, EVENT_MAINEDITOR_RESIZELAYER));
                            }
                        }
                    },
                    {SDL_SCANCODE_S, { TL("vsp.maineditor.dsel"),
                            [this]() { this->isolateEnabled = false; }
                        }
                    },
                    {SDL_SCANCODE_V, { TL("vsp.maineditor.rescanv_bytile"),
                            [this]() {
                                if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Set the pixel grid first."));
                                }
                                else {
                                    g_addPopup(new PopupTileGeneric(this, "Resize canvas by tile size", "New tile size:", XY{ this->tileDimensions.x, this->tileDimensions.y }, EVENT_MAINEDITOR_RESIZELAYER_BY_TILE));
                                }
                            }
                        }
                    },
                    {SDL_SCANCODE_B, { TL("vsp.maineditor.rescanv_ntile"),
                            [this]() {
                                if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Set the pixel grid first."));
                                }
                                else {
                                    g_addPopup(new PopupTileGeneric(this, "Resize canvas by tile count", "New tile count:", XY{ (int)ceil(this->canvas.dimensions.x / (float)this->tileDimensions.x), (int)ceil(this->canvas.dimensions.y / (float)this->tileDimensions.y) }, EVENT_MAINEDITOR_RESIZELAYER_BY_TILECOUNT));
                                }
                            }
                        }
                    },
                    {SDL_SCANCODE_N, { TL("vsp.maineditor.intscale"),
                            [this]() {
                                g_addPopup(new PopupIntegerScale(this, TL("vsp.maineditor.intscale"), "Scale:", canvas.dimensions, XY{ 1,1 }, EVENT_MAINEDITOR_INTEGERSCALE));
                            }
                        }
                    },
                    {SDL_SCANCODE_M, { TL("vsp.maineditor.canvscale"),
                            [this]() {
                                g_addPopup(new PopupTileGeneric(this, TL("vsp.maineditor.canvscale"), "New size:", this->canvas.dimensions, EVENT_MAINEDITOR_RESCALELAYER));
                            }
                        }
                    },
                    {SDL_SCANCODE_COMMA, { TL("vsp.maineditor.rescanv_reorder"),
                            [this]() {
                                g_addPopup(new PopupTileGeneric(this, TL("vsp.maineditor.rescanv_reorder"), "New size:", this->canvas.dimensions, EVENT_MAINEDITOR_RESIZELAYER_REORDER_TILES));
                            }
                        }
                    },
                    {SDL_SCANCODE_P, { TL("vsp.maineditor.nineseg"),
                            [this]() {
                                g_addScreen(new NineSegmentPatternEditorScreen(this));
                            }
                        }
                    },
                })
            }
        },
        {
            SDL_SCANCODE_L,
            {
                makeNavbarSection(TL("vsp.maineditor.layer"), g_iconNavbarTabLayer,
                {
                    {SDL_SCANCODE_F, { TL("vsp.maineditor.flipx"),[this]() { this->layer_flipHorizontally();}}},
                    {SDL_SCANCODE_G, { TL("vsp.maineditor.flipy"),[this]() { this->layer_flipVertically(); }}},
                    {SDL_SCANCODE_X, { TL("vsp.maineditor.printcol"),
                            [this]() {
                                g_addNotification(Notification("", frmt("{} colors in current layer", this->getCurrentLayer()->numUniqueColors(true))));
                            }
                        }
                    },
                    {SDL_SCANCODE_R, { TL("vsp.maineditor.renlayer"),[this]() { this->layer_promptRenameCurrent(); }}},
                    {SDL_SCANCODE_S, { TL("vsp.maineditor.isolatealpha"),[this]() { this->layer_selectCurrentAlpha(); }}},
                    {SDL_SCANCODE_A, { TL("vsp.maineditor.removealpha"),[this]() { this->layer_setAllAlpha255(); }}},
                    {SDL_SCANCODE_K, { TL("vsp.maineditor.setckey"),
                            [this]() {
                                PopupPickColor* newPopup = new PopupPickColor(TL("vsp.maineditor.setckey"), TL("vsp.maineditor.setckeydesc"));
                                newPopup->setCallbackListener(EVENT_MAINEDITOR_SETCOLORKEY, this);
                                g_addPopup(newPopup);
                            }
                        }
                    },
                    {SDL_SCANCODE_C, { TL("vsp.maineditor.nav.layer.copylayertoclipboard"),[this]() {this->copyLayerToClipboard(this->getCurrentLayer());}}
                    },
                    {SDL_SCANCODE_E, { TL("vsp.maineditor.nav.layer.clearselection"),[this]() { this->layer_clearSelectedArea(); }}},
                    {SDL_SCANCODE_W, { TL("vsp.maineditor.nav.layer.fillselection"),[this]() { this->layer_fillActiveColor(); }}},
                    {SDL_SCANCODE_M, { TL("vsp.maineditor.nav.layer.newvariant"),[this]() { this->layer_newVariant(); }}},
                    {SDL_SCANCODE_N, { TL("vsp.maineditor.nav.layer.copyvariant"),[this]() { this->layer_duplicateActiveVariant(); }}},
                    {SDL_SCANCODE_T, { TL("vsp.maineditor.nav.layer.renvariant"),[this]() { this->layer_promptRenameCurrentVariant(); }}},
                })
            }
        },
        {
            SDL_SCANCODE_Q,
            {
                TL("vsp.maineditor.tab.filters"),
                {},
                {
                },
                NULL
            }
        },
        {
            SDL_SCANCODE_R,
            {
                TL("vsp.maineditor.tab.render"),
                {},
                {
                },
                NULL
            }
        },
        {
            SDL_SCANCODE_V,
            {
                TL("vsp.nav.view"),
                {},
                {
                    {SDL_SCANCODE_R, { "Recenter canvas",
                            [this]() { this->recenterCanvas(); }
                        }
                    },
                    {SDL_SCANCODE_F, { "Add reference...",
                            [this]() {
                                PopupFilePicker::PlatformAnyImageImportDialog(this, TL("vsp.popup.addreference"), EVENT_MAINEDITOR_ADD_REFERENCE, true);
                            }
                        }
                    },
                    {SDL_SCANCODE_B, { "Toggle background color",
                            [this]() {
                                this->backgroundColor.r = ~this->backgroundColor.r;
                                this->backgroundColor.g = ~this->backgroundColor.g;
                                this->backgroundColor.b = ~this->backgroundColor.b;
                            }
                        }
                    },
                    {SDL_SCANCODE_C, { "Toggle comments",
                            [this]() {
                                (*(int*)&this->commentViewMode)++;
                                (*(int*)&this->commentViewMode) %= 3;
                                g_addNotification(Notification(frmt("{}",
                                    this->commentViewMode == COMMENTMODE_HIDE_ALL ? "All comments hidden" :
                                    this->commentViewMode == COMMENTMODE_SHOW_HOVERED ? "Comments shown on hover" :
                                    "All comments shown"), "", 1500
                                ));
                            }
                        }
                    },
                    {SDL_SCANCODE_G, { "Set pixel grid...",
                            [this]() { g_addPopup(new PopupSetEditorPixelGrid(this, "Set pixel grid", "Enter grid size <w>x<h>:")); }
                        }
                    },
                    {SDL_SCANCODE_P, { "Open preview panel...",
                            [this]() {
                                openPreviewPanel();
                            }
                        }
                    },
                    {SDL_SCANCODE_T, { "Open touch mode panel...",
                            [this]() {
                                openTouchModePanel();
                            }
                        }
                    },
                    {SDL_SCANCODE_A, { "Open frames panel...",
                            [this]() {
                                framePicker->enabled = true;
                                framePicker->playPanelOpenVFX();
                            }
                        }
                    },
                },
                g_iconNavbarTabView
            }
        },
        {
            SDL_SCANCODE_P,
            makeNavbarSection(TL("vsp.nav.preview"), {
                {SDL_SCANCODE_F, { "Preview in separate workspace...",
                        [this]() {
                            g_addScreen(new ViewSessionScreen(this));
                        }
                    }
                },
                {SDL_SCANCODE_3, { "Preview 3D cube...",
                        [this]() {
                            if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Tile grid must be set"));
                                return;
                            }
                            MinecraftBlockPreviewScreen* newScreen = new MinecraftBlockPreviewScreen(this);
                            g_addScreen(newScreen);
                        }
                    }
                },
                {SDL_SCANCODE_S, { "Preview spritesheet...",
                        [this]() {
                            if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Set the pixel grid first."));
                                return;
                            }
                            SpritesheetPreviewScreen* newScreen = new SpritesheetPreviewScreen(this);
                            g_addScreen(newScreen);
                        }
                    }
                },
                {SDL_SCANCODE_T, { "Preview tileset...",
                        [this]() {
                            if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Set the pixel grid first."));
                                return;
                            }
                            TilemapPreviewScreen* newScreen = new TilemapPreviewScreen(this);
                            g_addScreen(newScreen);
                        }
                    }
                },
                {SDL_SCANCODE_M, { "Preview Minecraft skin...",
                        [this]() {
                            if (!MinecraftSkinPreviewScreen::dimensionsValidForPreview(this->canvas.dimensions)) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid size. Aspect must be 1:1 or 2:1."));
                                return;
                            }
                            g_addScreen(new MinecraftSkinPreviewScreen(this));
                        }
                    }
                },
#if VSP_USE_LIBLCF
                {SDL_SCANCODE_R, { "Preview RPG Maker 2K / 2K3 ChipSet...",
                        [this]() {
                            if (!xyEqual(this->canvas.dimensions, {480, 256})) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Dimensions must be 480x256"));
                                return;
                            }
                            RPG2KTilemapPreviewScreen* newScreen = new RPG2KTilemapPreviewScreen(this);
                            g_addScreen(newScreen);
                        }
                    }
                },
#endif
            })
        }
    };

    SDL_Scancode keyorder[] = { SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R, SDL_SCANCODE_T, SDL_SCANCODE_Y, SDL_SCANCODE_U, SDL_SCANCODE_I, SDL_SCANCODE_O, SDL_SCANCODE_P,
                               SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_H, SDL_SCANCODE_J, SDL_SCANCODE_K, SDL_SCANCODE_L,
                               SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V, SDL_SCANCODE_B, SDL_SCANCODE_N, SDL_SCANCODE_M };

    //add print
    if (platformSupportsFeature(VSP_FEATURE_OS_PRINTER)) {
        mainEditorKeyActions[SDL_SCANCODE_F].order.insert(mainEditorKeyActions[SDL_SCANCODE_F].order.begin() + 9, SDL_SCANCODE_L);
        mainEditorKeyActions[SDL_SCANCODE_F].actions[SDL_SCANCODE_L] = {
            TL("vsp.maineditor.nav.print"),
            [this]() {
                PopupIntegerScale* printScalePopup = new PopupIntegerScale(this, TL("vsp.maineditor.nav.print.scale"), TL("vsp.maineditor.nav.print.scale.desc"), canvas.dimensions, {1,1}, EVENT_MAINEDITOR_PRINT, false);
                g_addPopup(printScalePopup);
            }
        };
    }

    //load filters
    int i = 0;
    for (auto& filter : g_filters) {
        mainEditorKeyActions[SDL_SCANCODE_Q].actions[keyorder[i++]] = {
            filter->name(), [this, filter]() {
                Layer* currentLayer = this->getCurrentLayer();
                filter->setupParamBounds(currentLayer);
                PopupApplyFilter* newPopup = new PopupApplyFilter(this, currentLayer, filter);
                g_addPopup(newPopup);
                if (filter->getParameters().size() == 0) {
                    newPopup->applyAndClose();
                }
            }
        };
    }
    //load render filters
    i = 0;
    for (auto& filter : g_renderFilters) {
        mainEditorKeyActions[SDL_SCANCODE_R].actions[keyorder[i++]] = {
            filter->name(), [this, filter]() {
                PopupApplyFilter* newPopup = new PopupApplyFilter(this, this->getCurrentLayer(), filter);
                g_addPopup(newPopup);
                if (filter->getParameters().size() == 0) {
                    newPopup->applyAndClose();
                }
            }
        };
    }

    navbar = new ScreenWideNavBar(this, mainEditorKeyActions, { SDL_SCANCODE_F, SDL_SCANCODE_E, SDL_SCANCODE_L, SDL_SCANCODE_Q, SDL_SCANCODE_R, SDL_SCANCODE_V, SDL_SCANCODE_P }); 
    wxsManager.addDrawable(navbar);

    makeActionBar();

    colorPicker = new EditorColorPicker(this);
    CollapsableDraggablePanel* colorPickerPanel = new CollapsableDraggablePanel(TL("vsp.maineditor.panel.colorpicker.title"), colorPicker);
    colorPickerPanel->position.y = 67;
    colorPickerPanel->position.x = 10;
    wxsManager.addDrawable(colorPickerPanel);
    colorPicker->setColorRGB(pickedColor);
    regenerateLastColors();

    brushPicker = new EditorBrushPicker(this);
    brushPicker->position.y = 458;
    brushPicker->position.x = 10;
    wxsManager.addDrawable(brushPicker);

    layerPicker = new EditorLayerPicker(this);
    layerPicker->position = XY{ 440, 80 };
    layerPicker->anchor = XY{ 1,0 };
    wxsManager.addDrawable(layerPicker);

    framePicker = new EditorFramePicker(this);
    framePicker->position = XY{ 10 + colorPickerPanel->getDimensions().x, 67};
    if (frames.size() == 1) {
        framePicker->enabled = false;
    }
    wxsManager.addDrawable(framePicker);
    
    //this must happen after actionbar init
    setActiveBrush(g_brushes[0]);
    currentPattern = g_patterns[0];

    if (g_lastConfirmInputWasTouch) {
        openTouchModePanel();
    }

    if (compactEditor) {
        
        std::vector<CompactEditorSection> createSections = {
            {colorPickerPanel, g_iconCompactColorPicker},
            {brushPicker, g_iconCompactToolPicker},
            {layerPicker, g_iconCompactLayerPicker},
            {framePicker, g_iconCompactFramePicker}
        };

        SetupCompactEditor(createSections);
    }
}

void MainEditor::SetupCompactEditor(std::vector<CompactEditorSection> createSections)
{
    int y = 120;
    int h = 80;
    for (auto& section : createSections) {
        section.targetPanel->enabled = false;

        UIButton* btn = new UIButton();
        btn->icon = section.icon;
        btn->position = { 0, y };
        btn->wxWidth = 80;
        btn->wxHeight = h;
        section.targetPanel->position = xyAdd(btn->position, { 100, 0 });
        btn->onClickCallback = [this, section](UIButton* b) {
            section.targetPanel->enabled = !section.targetPanel->enabled;
            //section.targetPanel->tryMoveOutOfOOB();
            this->wxsManager.forceFocusOn(section.targetPanel);
            };
        wxsManager.addDrawable(btn);

        y += h;
    }
}

void MainEditor::openTouchModePanel()
{
    if (this->touchModePanel == NULL) {
        this->touchModePanel = new EditorTouchToggle(this);
        this->touchModePanel->position = { g_windowW - this->touchModePanel->wxWidth - 10, g_windowH - this->touchModePanel->wxHeight - 40 };
        this->addWidget(this->touchModePanel);
    }
}

void MainEditor::makeActionBar()
{
    //action bar
    actionbar = new ScreenWideActionBar({});
    actionbar->position = { 0, navbar->wxHeight };

    int actionBarButtonSize = compactEditor ? 60 : 30;

    int nextNavbarX = 5;
    UIButton* undoButton = new UIButton("", TL("vsp.maineditor.undo"));
    undoButton->icon = g_iconActionBarUndo;
    undoButton->onClickCallback = [this](UIButton* btn) { undo(); };
    undoButton->position = { nextNavbarX,0 };
    undoButton->wxWidth = actionBarButtonSize;
    undoButton->wxHeight = actionBarButtonSize;
    nextNavbarX += actionBarButtonSize + 5;

    UIButton* redoButton = new UIButton("", TL("vsp.maineditor.redo"));
    redoButton->icon = g_iconActionBarRedo;
    redoButton->onClickCallback = [this](UIButton* btn) { redo(); };
    redoButton->position = { nextNavbarX,0 };
    redoButton->wxWidth = actionBarButtonSize;
    redoButton->wxHeight = actionBarButtonSize;
    nextNavbarX += actionBarButtonSize + 5;

    UIButton* saveButton = new UIButton("", TL("vsp.nav.save"));
    saveButton->icon = g_iconActionBarSave;
    saveButton->onClickCallback = [this](UIButton* btn) { if (g_shiftModifier) trySaveAsImage(); else trySaveImage(); };
    saveButton->position = { nextNavbarX,0 };
    saveButton->wxWidth = actionBarButtonSize;
    saveButton->wxHeight = actionBarButtonSize;
    nextNavbarX += actionBarButtonSize + 5;

    UIButton* zoomoutButton = new UIButton("", TL("vsp.cmn.zoomout"));
    zoomoutButton->icon = g_iconActionBarZoomOut;
    zoomoutButton->onClickCallback = [this](UIButton* btn) { canvas.zoom(-1, { g_windowW / 2, g_windowH / 2 }); };
    zoomoutButton->position = { nextNavbarX,0 };
    zoomoutButton->wxWidth = actionBarButtonSize;
    zoomoutButton->wxHeight = actionBarButtonSize;
    nextNavbarX += actionBarButtonSize + 5;

    UIButton* zoominButton = new UIButton("", TL("vsp.cmn.zoomin"));
    zoominButton->icon = g_iconActionBarZoomIn;
    zoominButton->onClickCallback = [this](UIButton* btn) { canvas.zoom(1, {g_windowW/2, g_windowH/2}); };
    zoominButton->position = { nextNavbarX,0 };
    zoominButton->wxWidth = actionBarButtonSize;
    zoominButton->wxHeight = actionBarButtonSize;
    nextNavbarX += actionBarButtonSize + 5;

    UIStackPanel* actionsStack = UIStackPanel::Horizontal(5, {
        undoButton, redoButton, saveButton, zoomoutButton, zoominButton
    });
    actionsStack->position = { 5, 5 };
    actionbar->addDrawable(actionsStack);

    toolPropertiesPanel = new Panel();
    toolPropertiesPanel->sizeToContent = true;
    toolPropertiesPanel->position = { nextNavbarX + 50, 0 };
    actionbar->addDrawable(toolPropertiesPanel);

    wxsManager.addDrawable(actionbar);
}

void MainEditor::initToolParameters()
{
    toolPropertiesPanel->subWidgets.freeAllDrawables();
    int x = 0;
    if (currentBrush != NULL) {
        for (auto& [key, prop] : currentBrush->getProperties()) {
            UILabel* label = new UILabel(prop.name);
            label->position = {x, 4};
            x += label->statSize().x + 20;
            toolPropertiesPanel->subWidgets.addDrawable(label);

            UILabel* valueLabel = NULL;
            if (prop.type != 3) {
                valueLabel = new UILabel();
                valueLabel->position = { x, 4 };
                valueLabel->color = { 255,255,255,0xa0 };
                x += 40;
                toolPropertiesPanel->subWidgets.addDrawable(valueLabel);
            }

            switch (prop.type) {
                case 1: //int
                    {
                        UISlider* slider = new UISlider();
                        slider->setValue(prop.min, prop.max, this->toolProperties[key]);
                        valueLabel->setText(std::to_string((int)slider->getValue(prop.min, prop.max)));
                        slider->wxHeight = 18;
                        slider->wxWidth = 150;
                        slider->position = { x, 8 };
                        slider->onChangeValueCallback = [this, prop, key, valueLabel](UISlider* s, float) {
                            int v = s->getValue(prop.min, prop.max);
                            this->toolProperties[key] = v;
                            valueLabel->setText(std::to_string(v));
                            };
                        toolPropertiesPanel->subWidgets.addDrawable(slider);
                        x += 110;
                    }
                    break;
                case 3: //bool
                    {
                        UICheckbox* chkbx = new UICheckbox("", this->toolProperties[key] == 1);
                        chkbx->position = { x, 3 };
                        chkbx->onStateChangeCallback = [this, key, valueLabel](UICheckbox* c, bool state) {
                            this->toolProperties[key] = state ? 1 : 0;
                        };
                        toolPropertiesPanel->subWidgets.addDrawable(chkbx);
                        x += 60;
                    }
                    break;
            }
        }
    }
    actionbar->evalHeight();
}

void MainEditor::addWidget(Drawable* wx)
{
    wxsManager.addDrawable(wx);
}

void MainEditor::removeWidget(Drawable* wx)
{
    if (!wxsManager.removeDrawable(wx) && wx != NULL && wx->isPanel()) {
        auto deleteTarget = ((Panel*)wx)->getTopmostParent();
        wxsManager.removeDrawable(deleteTarget);
    }

    auto findRefPanel = std::find(openReferencePanels.begin(), openReferencePanels.end(), wx);
    if (findRefPanel != openReferencePanels.end()) {
        openReferencePanels.erase(findRefPanel);
    }
    if (wx == (Drawable*)touchModePanel) {
        touchModePanel = NULL;
    }
}

void MainEditor::RecalcMousePixelTargetPoint(int x, int y) {
    mousePixelTargetPoint = canvas.screenPointToCanvasPoint({ x,y });
    mousePixelTargetPoint2xP =
        XY{
            (int)((canvas.currentDrawPoint.x - x) / (float)(-canvas.scale) / 0.5f),
            (int)((canvas.currentDrawPoint.y - y) / (float)(-canvas.scale) / 0.5f),
        };
}

bool MainEditor::requestSafeClose() {
    if (changesSinceLastSave == NO_UNSAVED_CHANGES) {
        closeThisScreen();
        return true;
    }
    else {
        
        std::string timeSinceLastSaveStr = frmt("{} {}", TL("vsp.maineditor.popup.safeclose.desc2"), secondsTimeToHumanReadable(timerSinceLastSave.elapsedTime() / 1000));

        g_addPopup(new PopupChooseAction(
            TL("vsp.maineditor.popup.safeclose"),
            frmt("{}\n\n{}", TL("vsp.maineditor.popup.safeclose.desc"), timeSinceLastSaveStr),
            {
                { SDL_SCANCODE_N, { TL("vsp.cmn.cancel"), [this]() {}}},
                { SDL_SCANCODE_C, { TL("vsp.cmn.discard"), [this]() { closeThisScreen(); }}},
                { SDL_SCANCODE_S, { TL("vsp.nav.save"), [this]() { trySaveImage(); }}},
            }
        ));
    }
    return false;
}

void MainEditor::zoom(int how_much)
{
    canvas.zoom(how_much);
}

bool MainEditor::isInBounds(XY pos)
{
    return
        canvas.pointInCanvasBounds(pos)
        && (!isolateEnabled || (isolateEnabled && isolatedFragment.pointExists(pos)));
}

void MainEditor::takeInput(SDL_Event evt) {

    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        if (requestSafeClose()) {
            return;
        }
    }

    LALT_TO_SUMMON_NAVBAR;

    if (evt.type == SDL_DROPFILE) {
        std::string path = evt.drop.data;
        if (!tryAddReference(convertStringOnWin32(path))) {
            if (tryInstallPalette(convertStringOnWin32(path))) {
                g_addNotification(SuccessNotification(TL("vsp.launchpad.paletteinstall"), fileNameFromPath(path)));
                g_reloadColorMap();
                colorPicker->reloadColorLists();
            }
        }
    }

    if ((evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) && evt.key.scancode == SDL_SCANCODE_Q) {
        qModifier = evt.key.down;
    }

    if (evt.type == SDL_KEYDOWN && evt.key.scancode == SDL_SCANCODE_F1) {
        hideUI = !hideUI;
    }

    if (!DrawableManager::processInputEventInMultiple({wxsManager}, evt)) {

        switch (evt.type) {
            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_UP:
            case SDL_EVENT_FINGER_MOTION:
                if (touchMode != TOUCHMODE_PAN) {
                    evt = convertTouchToMouseEvent(evt);
                    if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN || evt.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                        evt.button.button = touchMode == TOUCHMODE_LEFTCLICK ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT;
                    }
                }
                break;

        }

        if (!g_keybindManager.processKeybinds(evt, "maineditor", this)) {
            switch (evt.type) {
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    if (evt.button.button == SDL_BUTTON_LEFT) {
                        if (middleMouseHold && evt.button.down) {
                            zoomKeyHeld = true;
                            zoomKeyTimer.start();
                            zoomInitial = 0;
                            zoomOrigin = {(int)evt.button.x, (int)evt.button.y};
                        } else {
                            zoomKeyHeld = false;
                            RecalcMousePixelTargetPoint((int)evt.button.x, (int)evt.button.y);
                            if (evt.button.which != SDL_PEN_MOUSEID) {
                                penPressure = 1.0f;
                            }
                            if (currentBrush != NULL) {
                                if (evt.button.down) {
                                    if (!currentBrush->isReadOnly()) {
                                        commitStateToCurrentLayer();
                                    }
                                    currentBrush->clickPress(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                                    currentBrushMouseDowned = true;
                                }
                                else {
                                    if (currentBrushMouseDowned) {
                                        currentBrush->clickRelease(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                                        networkCanvasStateUpdated(activeFrame, selLayer);
                                        currentBrushMouseDowned = false;
                                        leftMouseReleaseTimer.start();
                                    }

                                }
                            }
                            mouseHoldPosition = mousePixelTargetPoint;
                            mouseHoldPosition2xP = mousePixelTargetPoint2xP;
                            leftMouseHold = evt.button.down;
                        }
                    }
                    else if (evt.button.button == SDL_BUTTON_MIDDLE) {
                        middleMouseHold = evt.button.down;
                        zoomKeyHeld = false;
                    }
                    else if (evt.button.button == SDL_BUTTON_RIGHT) {
                        inputMouseRight({ (int)evt.button.x, (int)evt.button.y }, evt.button.down);
                    }
                    break;
    #if __ANDROID__ 
                //limiting this to android because on pc you can just rebind these buttons however you want
                case SDL_EVENT_PEN_BUTTON_DOWN:
                case SDL_EVENT_PEN_BUTTON_UP:
                    if (evt.pbutton.button == 1) {    //this should be the s-pen button
                        //inputMouseRight({ (int)(evt.button.x * g_windowW), (int)(evt.button.y * g_windowH) }, evt.pbutton.down);
                    }
                    break;
    #endif
                case SDL_EVENT_PEN_MOTION:
                    //logprintf("SDL_EVENT_PEN_MOTION: %i  %i %i\n", evt.type == SDL_EVENT_PEN_MOTION, (int)evt.pmotion.x, (int)evt.pmotion.y);
                case SDL_EVENT_MOUSE_MOTION:
                    if (!penDown || evt.type != SDL_EVENT_MOUSE_MOTION) {
                        XY xy = evt.type == SDL_EVENT_PEN_MOTION ? XY{(int)evt.pmotion.x, (int)evt.pmotion.y}
                                                                 : XY{(int)evt.motion.x, (int)evt.motion.y};
                        if (middleMouseHold && zoomKeyHeld) {
                            int zoomDiff = xySubtract(xy, zoomOrigin).y / zoomPixelStep;
                            if (zoomDiff != zoomInitial) {
                                zoom(zoomDiff-zoomInitial);
                                zoomInitial = zoomDiff;
                            }
                        } else {
                            RecalcMousePixelTargetPoint(xy.x, xy.y);
                            if (evt.type == SDL_EVENT_MOUSE_MOTION && middleMouseHold) {
                                canvas.panCanvas({
                                    (int)(evt.motion.xrel) * (g_shiftModifier ? 2 : 1),
                                    (int)(evt.motion.yrel) * (g_shiftModifier ? 2 : 1)
                                });
                            }
                            else if (leftMouseHold) {
                                if (currentBrush != NULL) {
                                    currentBrush->clickDrag(this, 
                                        currentBrush->wantDoublePosPrecision() ? mouseHoldPosition2xP : mouseHoldPosition,
                                        currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                                }
                                mouseHoldPosition = mousePixelTargetPoint;
                                mouseHoldPosition2xP = mousePixelTargetPoint2xP;
                            }
                            if (currentBrush != NULL) {
                                currentBrush->mouseMotion(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                            }
                        }
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    if (g_ctrlModifier && !g_config.scrollWithTouchpad) {
                        colorPicker->setColorHSV(colorPicker->currentH, fxmin(fxmax(colorPicker->currentS + 0.1 * evt.wheel.y, 0), 1), colorPicker->currentV);
                    }
                    else if (g_shiftModifier && !g_config.scrollWithTouchpad) {
                        double newH = dxmin(dxmax(colorPicker->currentH + (360.0 / 12) * evt.wheel.y, 0), 359);
                        colorPicker->setColorHSV(newH, colorPicker->currentS, colorPicker->currentV);
                    }
                    else {
                        if (g_config.scrollWithTouchpad && !g_ctrlModifier) {
                            canvas.panCanvas({
                                (int)(-(g_shiftModifier ? evt.wheel.y : evt.wheel.x) * 20),
                                (int)((g_shiftModifier ? -evt.wheel.x : evt.wheel.y) * 20)
                            });
                        }
                        else {
                            canvas.zoomFromWheelInput(evt.wheel.y);
                        }
                    }
                    break;
                case SDL_EVENT_KEY_DOWN:
                    {
                        switch (evt.key.scancode) {
                            case SDL_SCANCODE_K:
                                if (g_ctrlModifier && g_shiftModifier && getCurrentLayer()->name == "06062000") {
                                    commitStateToCurrentLayer();
                                    for (int x = 0; x < voidsprite_image_w; x++) {
                                        for (int y = 0; y < voidsprite_image_h; y++) {
                                            SetPixel({x, y}, the_creature[x + y * voidsprite_image_w]);
                                        }
                                    }
                                    g_addNotification(
                                        Notification("hiii!!!!", "hello!!", 7500, g_iconNotifTheCreature, COLOR_INFO));
                                }
                                break;
                            case SDL_SCANCODE_RCTRL:
                                middleMouseHold = !middleMouseHold;
                                break;
                        }
                    }
                    break;
                case SDL_FINGERMOTION:
                    if (!penDown) {
                        XY rel = {evt.tfinger.dx * g_windowW, evt.tfinger.dy * g_windowH};
                        canvas.panCanvas(rel);
                    }
                    break;
                case SDL_EVENT_PEN_DOWN:
                case SDL_EVENT_PEN_UP:
                    penDown = evt.ptouch.down;
                    SDL_SetWindowMouseGrab(g_wd, penDown);
                    //loginfo(frmt("new pen state: {}", penDown));
                    break;
                case SDL_EVENT_PEN_AXIS:
                    if (evt.paxis.axis == 0) {  //should always be the pressure axis
                        penPressure = evt.paxis.value;
                    }
                    break;
            }
        }
    } else {
        leftMouseHold = false;
        penDown = false;
    }
}

std::vector<std::string> MainEditor::dropEverythingYoureDoingAndSave()
{
    try {
        return { convertStringToUTF8OnWin32(createRecoveryAutosave(frmt("-crash-{}", g_crashsaveIndex++))) };
    }
    catch (std::exception& e) {
        logerr(frmt("failed to crashsave:\n {}", e.what()));
    }
    return {};
}

void MainEditor::focusOnColorInputTextBox()
{
    if (colorPicker->colorTextField != NULL) {
        wxsManager.forceFocusOn(colorPicker);
        colorPicker->subWidgets.forceFocusOn(colorPicker->colorTextField);
        colorPicker->colorTextField->setText("");
    }
}

void MainEditor::layer_clearSelectedArea()
{
    commitStateToCurrentLayer();
    getCurrentLayer()->clear(isolateEnabled ? &isolatedFragment : NULL);
    g_addNotification(Notification("Area cleared", "", 1000));
}

void MainEditor::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterID)
{
    if (evt_id == EVENT_MAINEDITOR_SAVEFILE) {
        exporterID--;
        logprintf("eventFileSaved: got file name %ls\n", name.c_str());

        bool result = false;

        FileExporter* exporter = NULL;
        if (exporterID < g_fileExporters.size()) {
            exporter = g_fileExporters[exporterID];

            result = trySaveWithExporter(name, exporter);
        }

        if (result) {
            g_tryPushLastFilePath(convertStringToUTF8OnWin32(name));
        }
    }
    else if (evt_id == EVENT_MAINEDITOR_EXPORTPALETTIZED) {
        exporterID--;

        FileExporter* exporter = NULL;
        if (exporterID < g_palettizedFileExporters.size()) {
            bool result = false;
            exporter = g_palettizedFileExporters[exporterID];

            MainEditorPalettized* rgbConvEditor = toPalettizedSession();
            if (rgbConvEditor != NULL) {
                if (exporter->exportsWholeSession()) {
                    result = exporter->exportData(name, rgbConvEditor);
                }
                else {
                    Layer* l = rgbConvEditor->flattenImageWithoutConvertingToRGB();
                    result = exporter->exportData(name, l);
                    delete l;
                }
                delete rgbConvEditor;

                if (result) {
#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
                    emDownloadFile(lastConfirmedSavePath);
#endif
                    if (lastWasSaveAs && g_config.openSavedPath) {
                        platformOpenFileLocation(lastConfirmedSavePath);
                    }
                    g_addNotification(SuccessNotification("Success", "File exported successfully."));
                }
                else {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.exportfail")));
                }
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.exportfail")));
            }
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid exporter"));
        }
    }
    else if (evt_id == EVENT_MAINEDITOR_EXPORTTILES) {
        exporterID--;

        FileExporter* exporter = g_fileExporters[exporterID];
        XY tileCounts = { canvas.dimensions.x / tileDimensions.x, canvas.dimensions.y / tileDimensions.y };
        PlatformNativePathString pathOfFile = name.substr(0, name.find_last_of(convertStringOnWin32("/\\")));

        Layer* flatImage = flattenImage();
        for (int y = 0; y < tileCounts.y; y++) {
            for (int x = 0; x < tileCounts.x; x++) {
                SDL_Rect clipRect = { x * tileDimensions.x, y * tileDimensions.y, tileDimensions.x, tileDimensions.y };
                Layer* clip = flatImage->trim(clipRect);
                if (clip != NULL) {
                    PlatformNativePathString tileName = name + convertStringOnWin32(frmt("_{}_{}{}", x, y, exporter->extension()));
                    if (!exporter->exportsWholeSession()) {
                        exporter->exportData(tileName, clip);
#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
                        emDownloadFile(tileName);
#endif
                        delete clip;
                    }
                    else {
                        MainEditor* session = new MainEditor(clip);
                        session->trySaveWithExporter(tileName, exporter);
                        delete session;
                    }
                }
                else {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("Failed to export tile {}:{}", x,y)));
                }
            }
        }
        delete flatImage;
        g_addNotification(SuccessNotification("Success", "Tiles exported"));
    }
}

void MainEditor::eventPopupClosed(int evt_id, BasePopup* p)
{
    if (evt_id == EVENT_MAINEDITOR_RESIZELAYER) {
        resizeAllLayersFromCommand(((PopupTileGeneric*)p)->result, false);
    }
    else if (evt_id == EVENT_MAINEDITOR_RESIZELAYER_BY_TILE) {
        resizeAllLayersFromCommand(((PopupTileGeneric*)p)->result, true);
    }
    else if (evt_id == EVENT_MAINEDITOR_RESIZELAYER_BY_TILECOUNT) {
        resizzeAllLayersByTilecountFromCommand(((PopupTileGeneric*)p)->result);
    }
    else if (evt_id == EVENT_MAINEDITOR_INTEGERSCALE) {
        integerScaleAllLayersFromCommand(((PopupIntegerScale*)p)->result, ((PopupIntegerScale*)p)->downscaleCheckbox->isChecked());
    } 
    else if (evt_id == EVENT_MAINEDITOR_RESCALELAYER) {
        rescaleAllLayersFromCommand(((PopupTileGeneric*)p)->result);
    }
    else if (evt_id == EVENT_MAINEDITOR_RESIZELAYER_REORDER_TILES) {
        resizeAllLayersReorderingTilesFromCommand(((PopupTileGeneric*)p)->result);
    }
    else if (evt_id == EVENT_MAINEDITOR_PRINT) {
        XY outScale = ((PopupIntegerScale*)p)->result;
        Layer* flat = flattenImage();
        if (flat != NULL) {
            Layer* flatScaled = flat->copyAllVariantsScaled({ flat->w * outScale.x, flat->h * outScale.y });
            delete flat;
            if (flatScaled != NULL) {
                platformPrintDocument(flatScaled);
                delete flatScaled;
            }
            else {
                g_addNotification(NOTIF_MALLOC_FAIL);
            }
        }
        else {
            g_addNotification(NOTIF_MALLOC_FAIL);
        }
    }
}

void MainEditor::eventColorSet(int evt_id, uint32_t color)
{
    if (evt_id == EVENT_MAINEDITOR_SETCOLORKEY) {
        Layer* l = getCurrentLayer();
        l->colorKey = color;
        l->colorKeySet = true;
        l->markLayerDirty();
    }
}

void MainEditor::eventFileOpen(int evt_id, PlatformNativePathString name, int importerId)
{
    if (evt_id == EVENT_MAINEDITOR_ADD_REFERENCE) {
        tryAddReference(name);
    }
}

void MainEditor::FillTexture() {
    Layer* l = getCurrentLayer();
    int* pixels = (int*)l->pixels32();
    //int pitch;
    //SDL_LockTexture(mainTexture, NULL, (void**)&pixels, &pitch);
    for (int x = 0; x < l->w; x++) {
        for (int y = 0; y < l->h; y++) {
            pixels[x + (y * l->w)] = 0x00000000;
        }
    }
    //SDL_UnlockTexture(mainTexture);
}

void MainEditor::SetPixel(XY position, uint32_t color, bool pushToLastColors, uint8_t symmetry) {
    if ((currentPattern->canDrawAt(position) ^ invertPattern) && (!replaceAlphaMode || (layer_getPixelAt(position) & 0xFF000000) != 0)) {
        if (!isolateEnabled || isolatedFragment.pointExists(position)) {

            u8 targetColorAlpha = eraserMode ? 0 : pickedAlpha;
            u32 colorRGB = color & 0xFFFFFF;
            u32 targetColor = (targetColorAlpha << 24) + colorRGB;

            if (blendAlphaMode) {
                if (eraserMode) {
                    targetColor = ((0xff - (targetColor >> 24)) << 24) + (targetColor & 0xffffff);
                }
                targetColor = alphaBlend(getCurrentLayer()->getPixelAt(position), targetColor);
            }
            getCurrentLayer()->setPixel(position, targetColor);
            if (pushToLastColors) {
                colorPicker->pushLastColor(color);
            }
        }
    }
    if (symmetryEnabled[0] && !(symmetry & 0b10)) {
        int symmetryXPoint = symmetryPositions.x / 2;
        bool symXPointIsCentered = symmetryPositions.x % 2;
        int symmetryFlippedX = symmetryXPoint + (symmetryXPoint - position.x) - (symXPointIsCentered ? 0 : 1);
        SetPixel(XY{symmetryFlippedX, position.y}, color, pushToLastColors, symmetry | 0b10);
    }
    if (symmetryEnabled[1] && !(symmetry & 0b1)) {
        int symmetryYPoint = symmetryPositions.y / 2;
        bool symYPointIsCentered = symmetryPositions.y % 2;
        int symmetryFlippedY = symmetryYPoint + (symmetryYPoint - position.y) - (symYPointIsCentered ? 0 : 1);
        SetPixel(XY{position.x, symmetryFlippedY}, color, pushToLastColors, symmetry | 0b1);
    }
}

void MainEditor::DrawLine(XY from, XY to, uint32_t color) {
    rasterizeLine(from, to, [&](XY a)->void {
        SetPixel(a, color);
    });
}

void MainEditor::copyImageToClipboard()
{
    Layer* flat = flattenImage();
    if (flat != NULL) {
        copyLayerToClipboard(flat);
        delete flat;
    } 
}

void MainEditor::copyLayerToClipboard(Layer* l)
{
    Layer* copyTarget = l;
    if (isolateEnabled) {
        copyTarget = visualClipLayer(l, &isolatedFragment);
        if (copyTarget == NULL) {
            return;
        }
    }
    if (platformPutImageInClipboard(copyTarget)) {
        g_addNotification(SuccessNotification(TL("vsp.cmn.copiedtoclipboard"), ""));
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.clipboardcopy")));
    }
    if (copyTarget != l) {
        delete copyTarget;
    }
}

void MainEditor::trySaveImage()
{
    bool result = false;
    if (splitSessionData.set) {
        if (saveSplitSession(lastConfirmedSavePath, this)) {
            result = true;
            timerSinceLastSave.start();
            changesSinceLastSave = NO_UNSAVED_CHANGES;
        }
    }
    else {
        lastWasSaveAs = false;
        if (!lastConfirmedSave) {
            trySaveAsImage();
        }
        else {
            result = trySaveWithExporter(lastConfirmedSavePath, lastConfirmedExporter);
        }
    }
    if (result) {
        g_tryPushLastFilePath(convertStringToUTF8OnWin32(lastConfirmedSavePath));
    }
}

bool MainEditor::trySaveWithExporter(PlatformNativePathString name, FileExporter* exporter)
{
    bool result = false;
    if (exporter->exportsWholeSession()) {
        result = exporter->exportData(name, this);
    }
    else {
        Layer* flat = flattenImage();
        flat->importExportExtdata = makeSingleLayerExtdata();
        result = exporter->exportData(name, flat);
        delete flat;
    }

    if (result) {
        lastConfirmedSave = true;
        lastConfirmedSavePath = name;
        lastConfirmedExporter = exporter;
        timerSinceLastSave.start();
        changesSinceLastSave = NO_UNSAVED_CHANGES;
#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
        emDownloadFile(lastConfirmedSavePath);
#endif
        if (lastWasSaveAs && g_config.openSavedPath) {
            platformOpenFileLocation(lastConfirmedSavePath);
        }
        g_addNotification(SuccessNotification("File saved", "Save successful!"));
    }
    else {
        g_addNotification(ErrorNotification("File not saved", "Save failed!"));
    }

    return result;
}

void MainEditor::trySaveAsImage()
{
    if (splitSessionData.set) {
        if (saveSplitSession(lastConfirmedSavePath, this)) {
            g_tryPushLastFilePath(convertStringToUTF8OnWin32(lastConfirmedSavePath));
        }
    }
    else {
        lastWasSaveAs = true;
        std::vector<std::pair<std::string, std::string>> formats;
        for (auto f : g_fileExporters) {
            formats.push_back({ f->extension(), f->name() });
        }
        platformTrySaveOtherFile(this, formats, TL("vsp.popup.saveimage"), EVENT_MAINEDITOR_SAVEFILE);
    }
}

std::map<std::string, std::string> MainEditor::makeSingleLayerExtdata()
{
    std::map<std::string, std::string> ret;
    ret["TileX"] = std::to_string(tileDimensions.x);
    ret["TileY"] = std::to_string(tileDimensions.y);
    ret["TilePadRX"] = std::to_string(tileGridPaddingBottomRight.x);
    ret["TilePadBY"] = std::to_string(tileGridPaddingBottomRight.y);
    ret["SymX"] = std::to_string(symmetryPositions.x);
    ret["SymY"] = std::to_string(symmetryPositions.y);
    ret["SymEnabledX"] = symmetryEnabled[0] ? "1" : "0";
    ret["SymEnabledY"] = symmetryEnabled[1] ? "1" : "0";
    ret["CommentData"] = makeCommentDataString(getCurrentFrame());
    ret["UsingAltBG"] = usingAltBG() ? "1" : "0";
    return ret;
}

void MainEditor::loadSingleLayerExtdata(Layer* l) {
    try {
        auto kvmap = l->importExportExtdata;
        if (kvmap.contains("TileX")) { tileDimensions.x = std::stoi(kvmap["TileX"]); }
        if (kvmap.contains("TileY")) { tileDimensions.y = std::stoi(kvmap["TileY"]); }
        if (kvmap.contains("TilePadRX")) { tileGridPaddingBottomRight.x = std::stoi(kvmap["TilePadRX"]); }
        if (kvmap.contains("TilePadBY")) { tileGridPaddingBottomRight.y = std::stoi(kvmap["TilePadBY"]); }
        if (kvmap.contains("SymX")) { symmetryPositions.x = std::stoi(kvmap["SymX"]); }
        if (kvmap.contains("SymY")) { symmetryPositions.y = std::stoi(kvmap["SymY"]); }
        if (kvmap.contains("SymEnabledX")) { symmetryEnabled[0] = kvmap["SymEnabledX"] == "1"; }
        if (kvmap.contains("SymEnabledY")) { symmetryEnabled[1] = kvmap["SymEnabledY"] == "1"; }
        if (kvmap.contains("CommentData")) { frames.front()->comments = parseCommentDataString(kvmap["CommentData"]); }
        if (kvmap.contains("UsingAltBG")) { setAltBG(kvmap["UsingAltBG"] == "1"); }
    }
    catch (std::exception e) {}
}

std::string MainEditor::makeCommentDataString(Frame* f)
{
    std::string commentsData = "";
    commentsData += std::to_string(f->comments.size()) + ';';
    for (CommentData& c : f->comments) {
        commentsData += std::to_string(c.position.x) + ';';
        commentsData += std::to_string(c.position.y) + ';';
        std::string sanitizedData = c.data;
        std::replace(sanitizedData.begin(), sanitizedData.end(), ';', '\1');
        commentsData += sanitizedData + ';';
    }
    return commentsData;
}

std::vector<CommentData> MainEditor::parseCommentDataString(std::string data)
{
    std::vector<CommentData> ret;
    std::string commentsData = data;
    int nextSC = commentsData.find_first_of(';');
    int commentsCount = std::stoi(commentsData.substr(0, nextSC));
    commentsData = commentsData.substr(nextSC + 1);
    for (int x = 0; x < commentsCount; x++) {
        CommentData newComment;
        nextSC = commentsData.find_first_of(';');
        newComment.position.x = std::stoi(commentsData.substr(0, nextSC));
        commentsData = commentsData.substr(nextSC + 1);
        nextSC = commentsData.find_first_of(';');
        newComment.position.y = std::stoi(commentsData.substr(0, nextSC));
        commentsData = commentsData.substr(nextSC + 1);
        nextSC = commentsData.find_first_of(';');
        newComment.data = commentsData.substr(0, nextSC);
        std::replace(newComment.data.begin(), newComment.data.end(), '\1', ';');
        commentsData = commentsData.substr(nextSC + 1);
        ret.push_back(newComment);
    }
    return ret;
}

void MainEditor::recenterCanvas()
{
    canvas.recenter();
}

void MainEditor::discardEndOfUndoStack() {
    if (!undoStack.empty()) {
        auto*& e = undoStack.front();
        e->discardFromRedo = false;
        delete e;

        undoStack.erase(undoStack.begin());
    }
}

void MainEditor::checkAndDiscardEndOfUndoStack()
{
    while (undoStack.size() > g_config.maxUndoHistory) {
        discardEndOfUndoStack();
    }
}

void MainEditor::commitStateToLayer(Layer* l)
{
    addToUndoStack(UndoLayerModified::fromCurrentState(l));
    networkCanvasStateUpdated(activeFrame, indexOfLayer(l));
}

void MainEditor::commitStateToCurrentLayer()
{
    if (!getLayerStack().empty()) {
        commitStateToLayer(getCurrentLayer());
    }
}

uint32_t MainEditor::pickColorFromAllLayers(XY pos)
{
    uint32_t c = 0;
    auto& layerStack = getLayerStack();
    for (int x = layerStack.size() - 1; x >= 0; x--) {
        if (layerStack[x]->hidden) {
            continue;
        }
        uint32_t nextC = layerStack[x]->getPixelAt(pos, false);
        if ((c & 0xff000000) == 0 && (nextC & 0xff000000) == (0xff<<24)) {
            return nextC;
        }
        else {
            c = alphaBlend(nextC, c);
        }
    }
    return c;
}

void MainEditor::addToUndoStack(UndoStackElementV2* undo)
{
    lastUndoWasRedo = true;
    undoTimer.start();
    discardRedoStack();
    undoStack.push_back(undo);
    checkAndDiscardEndOfUndoStack();
    changesSinceLastSave = HAS_UNSAVED_CHANGES;
    networkCanvasStateUpdated(activeFrame, indexOfLayer(undo->getAffectedLayer()));
}

void MainEditor::discardUndoStack()
{
    while (!undoStack.empty()) {
        discardEndOfUndoStack();
    }
}

void MainEditor::discardRedoStack()
{
    //clear redo stack
    for (auto& e : redoStack) {
        e->discardFromRedo = true;
        delete e;
    }
    redoStack.clear();
}

void MainEditor::undo()
{
    if (!undoStack.empty()) {
        undoTimer.start();
        lastUndoWasRedo = false;
        auto*& l = undoStack.back();
        l->undo(this);
        undoStack.pop_back();
        changesSinceLastSave = HAS_UNSAVED_CHANGES;
        redoStack.push_back(l);
        networkCanvasStateUpdated(activeFrame, indexOfLayer(l->getAffectedLayer()));
    }
    else {
        g_addNotification(ErrorNotification("Nothing to undo", ""));
    }
}

void MainEditor::redo()
{
    if (!redoStack.empty()) {
        undoTimer.start();
        lastUndoWasRedo = true;
        auto*& l = redoStack.back();
        l->redo(this);
        redoStack.pop_back();
        changesSinceLastSave = HAS_UNSAVED_CHANGES;
        undoStack.push_back(l);
        networkCanvasStateUpdated(activeFrame, indexOfLayer(l->getAffectedLayer()));
    }
    else {
        g_addNotification(ErrorNotification("Nothing to redo", ""));
    }
}

void MainEditor::newFrame()
{
    std::lock_guard<std::recursive_mutex> lock(framesMutex);
    if (splitSessionData.set) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Frames not supported in split session mode"));
        return;
    }
    Layer* nlayer = isPalettized ? LayerPalettized::tryAllocIndexedLayer(canvas.dimensions.x, canvas.dimensions.y)
                                 : Layer::tryAllocLayer(canvas.dimensions.x, canvas.dimensions.y);
    if (nlayer != NULL) {
        if (isPalettized) {
            ((LayerPalettized*)nlayer)->palette = (((MainEditorPalettized*)this)->palette);
        }
        Frame* nFrame = new Frame();
        nFrame->layers.push_back(nlayer);
        frames.insert(frames.begin() + activeFrame + 1, nFrame);
        framePicker->enabled = true;
        //loginfo("new frame added");
        framePicker->createFrameButtons();
        addToUndoStack(new UndoFrameCreated(nFrame, activeFrame + 1));
        switchFrame(activeFrame + 1);
        framePicker->flashFrame(activeFrame);
    }
    else {
        g_addNotification(NOTIF_MALLOC_FAIL);
    }
}

void MainEditor::duplicateFrame(int index)
{
    std::lock_guard<std::recursive_mutex> lock(framesMutex);
    if (splitSessionData.set) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Frames not supported in split session mode"));
        return;
    }
    Frame* nFrame = getCurrentFrame()->copy();
    frames.insert(frames.begin() + index + 1, nFrame);
    framePicker->createFrameButtons();
    framePicker->enabled = true;
    addToUndoStack(new UndoFrameCreated(nFrame, index + 1));
    switchFrame(index + 1);
    framePicker->flashFrame(activeFrame);
}

void MainEditor::deleteFrame(int index)
{
    std::lock_guard<std::recursive_mutex> lock(framesMutex);
    if (index >= 0 && index < frames.size()) {
        if (frames.size() != 1) {
            Frame* target = frames[index];
            frames.erase(frames.begin() + index);
            framePicker->createFrameButtons();
            if (activeFrame >= frames.size()) {
                switchFrame(frames.size() - 1);
            }
            addToUndoStack(new UndoFrameRemoved(target, index));
        } else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Cannot delete last frame"));
            return;
        }
    }
}

void MainEditor::switchFrame(int index)
{
    if (activeFrame >= 0 && activeFrame < frames.size()) {
        getCurrentFrame()->activeLayer = selLayer;
    }
    activeFrame = ixmax(0, ixmin(frames.size()-1, index));
    //loginfo(frmt("switching to frame {}", index));
    selLayer = ixmin(getCurrentFrame()->layers.size()-1, getCurrentFrame()->activeLayer);
    layerPicker->updateLayers();
    framePicker->createFrameButtons();
}

void MainEditor::moveFrameLeft(int index)
{
    std::lock_guard<std::recursive_mutex> lock(framesMutex);
    if (index > 0 && index <= frames.size() - 1) {
        Frame* target = frames[index];
        frames.erase(frames.begin() + index);
        frames.insert(frames.begin() + index - 1, target);
        framePicker->createFrameButtons();
        addToUndoStack(new UndoFrameReordered(target, index, index - 1));
        if (index == activeFrame) {
            switchFrame(index - 1);
        }
    }
}

void MainEditor::moveFrameRight(int index)
{
    std::lock_guard<std::recursive_mutex> lock(framesMutex);
    if (index >= 0 && index < frames.size() - 1) {
        Frame* target = frames[index];
        frames.erase(frames.begin() + index);
        frames.insert(frames.begin() + index + 1, target);
        framePicker->createFrameButtons();
        addToUndoStack(new UndoFrameReordered(target, index, index + 1));
        if (index == activeFrame) {
            switchFrame(index + 1);
        }
    }
}

void MainEditor::toggleFrameAnimation()
{
    frameAnimationPlaying = !frameAnimationPlaying;
    frameAnimationStartTimer.start();
}

void MainEditor::setMSPerFrame(int ms)
{
    framePicker->msPerFrameInput->setText(std::to_string(ms));
}

void MainEditor::renderFrameTo(Frame* f, SDL_Texture* target, bool clear)
{
    g_pushRenderTarget(target);

    if (clear) {
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0);
        SDL_RenderClear(g_rd);
    }

    for (auto& l : f->layers) {
        if (!l->hidden) {
            l->render({ 0,0,l->w,l->h }, l->layerAlpha);
        }
    }

    g_popRenderTarget();
}

Layer* MainEditor::layerAt(int index)
{
    return getLayerStack().size() > index ? getLayerStack()[index] : NULL;
}

Layer* MainEditor::newLayer()
{
    Layer* nl = Layer::tryAllocLayer(canvas.dimensions.x, canvas.dimensions.y);
    if (nl != NULL) {
        auto& layerStack = getLayerStack();
        nl->name = frmt("New Layer {}", layerStack.size() + 1);
        int insertAtIdx = std::find(layerStack.begin(), layerStack.end(), getCurrentLayer()) - layerStack.begin() + 1;
        layerStack.insert(layerStack.begin() + insertAtIdx, nl);
        switchActiveLayer(insertAtIdx);

        addToUndoStack(new UndoLayerCreated(getCurrentFrame(), nl, insertAtIdx));
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.mallocfail")));
    }
    return nl;
}

void MainEditor::deleteLayer(int index) {
    if (getLayerStack().size() <= 1) {
        g_addNotification(ErrorNotification("Can't delete the last layer", ""));
        return;
    }

    auto& layers = getLayerStack();
    Layer* layerAtPos = layers[index];
    layers.erase(layers.begin() + index);
    if (selLayer >= layers.size()) {
        switchActiveLayer(layers.size() - 1);
    }

    addToUndoStack(new UndoLayerRemoved(getCurrentFrame(), layerAtPos, index));
}

void MainEditor::regenerateLastColors()
{
    if (getLayerStack().empty()) {
        return;
    }
    colorPicker->lastColors.clear();
    colorPicker->lastColorsChanged = true;
    Layer* flatLayer = flattenImage();
    auto colorPalette = flatLayer->get256MostUsedColors();
    delete flatLayer;
    for (auto& c : colorPalette) {
        colorPicker->pushLastColor(c);
    }
}

void MainEditor::setActiveColor(uint32_t col)
{
    colorPicker->setColorRGB(col);
}

void MainEditor::setActiveAlpha(uint8_t alpha)
{
    if (!isPalettized) {
        pickedAlpha = alpha;
        colorPicker->updateAlphaSlider();
    }
}

uint32_t MainEditor::getActiveColor()
{
    return pickedColor;
}

void MainEditor::playColorPickerVFX(bool inward)
{
    g_newVFX(VFX_COLORPICKER, 500, pickedColor, { g_mouseX, g_mouseY,-1,-1 }, { inward ? 1u : 0u });
}

void MainEditor::setActiveBrush(BaseBrush* b)
{
    if (currentBrush != NULL) {
        currentBrush->resetState();
    }
    currentBrush = b;
    for (auto& prop : currentBrush->getProperties()) {
        if (!toolProperties.contains(prop.first)) {
            toolProperties[prop.first] = prop.second.defaultValue;
        }
    }
    brushPicker->updateActiveBrushButton(b);
    initToolParameters();
}

void MainEditor::tickAutosave()
{
    if (!autosaveTimer.started) {
        autosaveTimer.start();
    }

    if (g_config.autosaveInterval > 0 && changesSinceLastSave == HAS_UNSAVED_CHANGES) {
        if (autosaveTimer.elapsedTime() > g_config.autosaveInterval * 1000 * 60) {
            autosaveTimer.start();
            createRecoveryAutosave();
        }
    }
}

PlatformNativePathString MainEditor::createRecoveryAutosave(std::string insertIntoFilename)
{
    time_t now = time(NULL);
    tm ltm;
    // todo: make a platform-specific localtime function
#if defined(__unix__) || defined(__APPLE__)
    localtime_r(&now, &ltm);
#elif _WIN32
    localtime_s(&ltm, &now);
#else
    ltm = *std::localtime(&now);
#endif
    std::string autosaveName = "autosave" + frmt("{}_{}-{:02}-{:02}--{:02}-{:02}-{:02}", insertIntoFilename, ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec) + ".voidsn";
    try {
        std::filesystem::path confPath = platformEnsureDirAndGetConfigFilePath();
        auto autosavePath = confPath.parent_path() / "autosaves" / autosaveName;
        if (voidsnExporter->exportData(autosavePath, this)) {
            g_addNotification(SuccessNotification("Recovery autosave", "Autosave successful"));
            changesSinceLastSave = CHANGES_RECOVERY_AUTOSAVED;
            return autosavePath;
        }
        else {
            g_addNotification(ErrorNotification("Recovery autosave", "Autosave failed"));
        }
    }
    catch (std::exception e) {
        g_addNotification(ErrorNotification("Recovery autosave", "Exception during autosave"));
    }
    return PlatformNativePathString();
}

//todo: clean all of that up
bool MainEditor::usingAltBG()
{
    return sdlcolorToUint32(backgroundColor) != 0xFF000000;
}
void MainEditor::setAltBG(bool useAltBG)
{
    backgroundColor = useAltBG ? SDL_Color{ 255, 255, 255, 255 } : SDL_Color{0, 0, 0, 255};
}

bool MainEditor::tryAddReference(PlatformNativePathString path)
{
    MainEditor* ssn = loadAnyIntoSession(convertStringToUTF8OnWin32(path));
    if (ssn != NULL) {
        Layer* flat = ssn->flattenImage();
        PanelReference* referencePanel = new PanelReference(flat, this);
        openReferencePanels.push_back(referencePanel);
        addWidget(referencePanel);
        referencePanel->playPanelOpenVFX();
        delete ssn;
        return true;
    }
    return false;
}

void MainEditor::openPreviewPanel()
{
    Panel* previewPanel = new PanelPreview(this);
    addWidget(previewPanel);
    previewPanel->playPanelOpenVFX();
}

void MainEditor::tryToggleTilePreviewLockAtMousePos() 
{
    if (lockedTilePreview.x != -1 && lockedTilePreview.y != -1) {
        lockedTilePreview = {-1, -1};
    } else {
        if (tileDimensions.x != 0 && tileDimensions.y != 0) {
            XY tileToLock = canvas.getTilePosAt({g_mouseX, g_mouseY}, tileDimensions);
            if (g_config.isolateRectOnLockTile) {
                isolateEnabled = true;
                isolatedFragment.clear();
                isolatedFragment.addRect({tileToLock.x * tileDimensions.x,
                                          tileToLock.y * tileDimensions.y, tileDimensions.x,
                                          tileDimensions.y});
                shouldUpdateRenderedIsolatedFragmentPoints = true;
            }
            if (tileToLock.x >= 0 && tileToLock.y >= 0) {
                lockedTilePreview = tileToLock;
                tileLockTimer.start();
            } else {
                g_addNotification(
                    ErrorNotification(TL("vsp.cmn.error"), "Tile position out of bounds"));
            }
        } else {
            lockedTilePreview = {0, 0};
            tileLockTimer.start();
        }
    }
}

void MainEditor::promptPasteImageFromClipboard()
{
    Layer* p = platformGetImageFromClipboard();
    if (p != NULL) {
        promptPasteImage(p);
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.launchpad.error.clipboard_no_image")));
    }
}

void MainEditor::promptPasteImage(Layer* l)
{
    g_addPopup(new PopupFreeformTransform(this, l));
}

void MainEditor::moveLayerUp(int index) {
    auto& layers = getLayerStack();
    if (index >= layers.size()-1) {
        return;
    }

    Layer* clayer = layers[index];
    layers.erase(layers.begin() + index);
    layers.insert(layers.begin() + index + 1, clayer);

    if (index == selLayer) {
        switchActiveLayer(selLayer + 1);
    }

    addToUndoStack(new UndoLayerReordered(getCurrentFrame(), clayer, index, index + 1));
}

void MainEditor::moveLayerDown(int index) {
    if (index  <= 0) {
        return;
    }
    auto& layers = getLayerStack();

    Layer* clayer = layers[index];
    layers.erase(layers.begin() + index);
    layers.insert(layers.begin() + index - 1, clayer);

    if (index == selLayer) {
        switchActiveLayer(selLayer-1);
    }

    addToUndoStack(new UndoLayerReordered(getCurrentFrame(), clayer, index, index - 1));
}

void MainEditor::mergeLayerDown(int index)
{
    if (index == 0) {
        return;
    }
    auto& layers = getLayerStack();

    Layer* topLayer = layers[index];
    Layer* bottomLayer = layers[index - 1];
    deleteLayer(index);
    commitStateToLayer(bottomLayer);
    Layer* merged = mergeLayers(bottomLayer, topLayer);
    memcpy(bottomLayer->pixels32(), merged->pixels32(), bottomLayer->w * bottomLayer->h * 4);
    bottomLayer->markLayerDirty();
    delete merged;
}

void MainEditor::duplicateLayer(int index)
{
    auto& layers = getLayerStack();
    Layer* currentLayer = layers[index];
    Layer* newL = newLayer();
    memcpy(newL->pixels32(), currentLayer->pixels32(), currentLayer->w * currentLayer->h * 4);
    newL->name = "Copy:" + currentLayer->name;
    newL->markLayerDirty();
}

void MainEditor::layer_flipHorizontally()
{
    commitStateToCurrentLayer();
    getCurrentLayer()->flipHorizontally();
}
void MainEditor::layer_flipVertically()
{
    commitStateToCurrentLayer();
    getCurrentLayer()->flipVertically();
}

void MainEditor::layer_newVariant()
{
    Layer* clayer = getCurrentLayer();
    clayer->newLayerVariant();
    addToUndoStack(new UndoLayerVariantCreated(clayer, clayer->currentLayerVariant));
    layerPicker->updateLayers();
}

void MainEditor::layer_duplicateActiveVariant()
{
    layer_duplicateActiveVariant(getCurrentLayer());
}

void MainEditor::layer_duplicateActiveVariant(Layer* clayer)
{
    layer_duplicateVariant(clayer, clayer->currentLayerVariant);
}

void MainEditor::layer_duplicateVariant(Layer* clayer, int variantIndex)
{
    if (clayer != NULL) {
        clayer->duplicateVariant(variantIndex);
        addToUndoStack(new UndoLayerVariantCreated(clayer, clayer->layerData.size() - 1));
        layerPicker->updateLayers();
    }
}

void MainEditor::layer_removeVariant(Layer* layer, int variantIndex)
{
    if (layer->layerData.size() > 1) {
        LayerVariant v = layer->layerData[variantIndex];
        layer->layerData.erase(layer->layerData.begin() + variantIndex);
        addToUndoStack(new UndoLayerVariantRemoved(layer, variantIndex, v));
        if (variantIndex <= layer->currentLayerVariant) {
            layer_switchVariant(layer, ixmin(variantIndex, layer->layerData.size() - 1));
        }
        layerPicker->updateLayers();
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.maineditor.error.dellastvariant")));
    }
}

void MainEditor::layer_switchVariant(Layer* layer, int variantIndex)
{
    int vidxNow = layer->currentLayerVariant;
    if (layer->switchVariant(variantIndex)) {
        networkCanvasStateUpdated(activeFrame, indexOfLayer(layer));
        layerPicker->updateLayers();
        if (layer == getCurrentLayer()) {
            variantSwitchTimer.start();
            lastVariantSwitchWasRight = vidxNow < variantIndex;
        }
    }
}

void MainEditor::layer_promptRenameCurrentVariant()
{
    Layer* clayer = getCurrentLayer();
    int layerVariantIndex = clayer->currentLayerVariant;
    std::string nameNow = clayer->layerData[layerVariantIndex].name;
    PopupTextBox* ninput = new PopupTextBox("Rename layer variant", "Enter the new layer variant name:", nameNow);
    ninput->onTextInputConfirmedCallback = [this,clayer,layerVariantIndex](PopupTextBox* p, std::string newName) {
        clayer->layerData[layerVariantIndex].name = newName;
        layerPicker->updateLayers();
    };
    g_addPopup(ninput);
}

int MainEditor::indexOfLayer(Layer* l)
{
    auto& layers = getLayerStack();
    auto it = std::find(layers.begin(), layers.end(), l);
    if (it != layers.end()) {
        return std::distance(layers.begin(), it);
    }
    return -1;
}

void MainEditor::layer_setOpacity(uint8_t opacity) {
    Layer* clayer = getCurrentLayer();
    addToUndoStack(new UndoLayerOpacityChanged(clayer, clayer->lastConfirmedlayerAlpha, opacity));
    clayer->layerAlpha = opacity;
    clayer->lastConfirmedlayerAlpha = clayer->layerAlpha;
}

void MainEditor::layer_promptRename(int index)
{
    auto& layers = getLayerStack();
    if (!layers.empty() && layers.size() > index) {
        Layer* target = layers[index];
        PopupTextBox* ninput = new PopupTextBox("Rename layer", "Enter the new layer name:", target->name);
        ninput->onTextInputConfirmedCallback = [this, target](PopupTextBox* p, std::string newName) {
            target->name = newName;
            layerPicker->updateLayers();
            networkCanvasStateUpdated(activeFrame, indexOfLayer(target));
        };
        g_addPopup(ninput);
    }
}

void MainEditor::layer_promptRenameCurrent()
{
    layer_promptRename(selLayer);
}

uint32_t MainEditor::layer_getPixelAt(XY pos)
{
    return getCurrentLayer()->getPixelAt(pos);
}

void MainEditor::switchActiveLayer(int index)
{
    selLayer = index;
    layerSwitchTimer.start();
}

void MainEditor::layer_setAllAlpha255()
{
    commitStateToCurrentLayer();
    getCurrentLayer()->setAllAlpha255();
}

Layer* MainEditor::flattenImage()
{
    return flattenFrame(getCurrentFrame());
}

Layer* MainEditor::flattenFrame(Frame* target)
{
    Layer* ret = Layer::tryAllocLayer(canvas.dimensions.x, canvas.dimensions.y);
    if (ret != NULL) {
        int x = 0;
        uint32_t* retppx = ret->pixels32();
        for (Layer*& l : target->layers) {
            if (l->hidden) {
                continue;
            }
            uint32_t* ppx = l->pixels32();
            if (x++ == 0) {
                if (l->layerAlpha == 255) {
                    memcpy(ret->pixels32(), l->pixels32(), l->w * l->h * 4ull);
                }
                else {
                    for (uint64_t p = 0; p < l->w * l->h; p++) {
                        uint32_t pixel = ppx[p];
                        uint8_t alpha = pixel >> 24;
                        alpha = (uint8_t)((alpha / 255.0f) * (l->layerAlpha / 255.0f) * 255);
                        pixel = (pixel & 0x00ffffff) | (alpha << 24);

                        retppx[p] = pixel;
                    }
                }
            }
            else {
                for (uint64_t p = 0; p < l->w * l->h; p++) {
                    uint32_t pixel = ppx[p];
                    uint8_t alpha = pixel >> 24;
                    alpha = (uint8_t)((alpha / 255.0f) * (l->layerAlpha / 255.0f) * 255);
                    pixel = (pixel & 0x00ffffff) | (alpha << 24);

                    uint32_t srcPixel = retppx[p];
                    uint8_t alpha2 = srcPixel >> 24;
                    alpha2 = (uint8_t)((alpha2 / 255.0f) * (ret->layerAlpha / 255.0f) * 255);
                    srcPixel = (srcPixel & 0x00ffffff) | (alpha2 << 24);

                    retppx[p] = alphaBlend(srcPixel, pixel);
                }
            }
        }
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.mallocfail")));
    }
    return ret;
}

Layer* MainEditor::mergeLayers(Layer* bottom, Layer* top)
{
    return Layer::mergeLayers(bottom, top);
}

Layer* MainEditor::visualClipLayer(Layer* l, ScanlineMap* map)
{
    SDL_Rect clipRect = map->getMinMaxRect();
    XY copyOrigin = { clipRect.x, clipRect.y };
    Layer* copyTarget = Layer::tryAllocLayer(clipRect.w, clipRect.h);
    if (copyTarget == NULL) {
        g_addNotification(NOTIF_MALLOC_FAIL);
        return NULL;
    }
    map->forEachPoint([&](XY p) {
        copyTarget->setPixel(xySubtract(p, copyOrigin), l->getVisualPixelAt(p));
    });
    return copyTarget;
}

void MainEditor::flipAllLayersOnX()
{
    std::vector<UndoStackElementV2*> undos;
    for (auto* l : getLayerStack()) {
        undos.push_back(UndoLayerModified::fromCurrentState(l));
        l->flipHorizontally();
    }

    addToUndoStack(new UndoStackComposite(undos));
}

void MainEditor::flipAllLayersOnY()
{
    std::vector<UndoStackElementV2*> undos;
    for (auto* l : getLayerStack()) {
        undos.push_back(UndoLayerModified::fromCurrentState(l));
        l->flipVertically();
    }

    addToUndoStack(new UndoStackComposite(undos));
}

void MainEditor::rescaleAllLayersFromCommand(XY size) {
    if (xyEqual(canvas.dimensions, size)) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Size must be different to rescale."));
        return;
    }

    UndoLayersResized* undoData = new UndoLayersResized(canvas.dimensions, size, tileDimensions, tileDimensions);
    std::vector<Layer*> layers = getAllLayers();

    //todo: detect if copyscaled or malloc fails
    int nElements = layers.size();
    for (Layer* l : layers) {
        undoData->storedLayerData[l] = l->layerData;
        Layer* sc = l->copyAllVariantsScaled(size);
        l->layerData = sc->layerData;
        sc->layerData = {};
        delete sc;
        l->w = size.x;
        l->h = size.y;
        l->markLayerDirty();
    }
    canvas.dimensions = size;

    addToUndoStack(undoData);
}

std::vector<Layer*> MainEditor::getAllLayers()
{
    std::vector<Layer*> ret = {};
    for (auto& f : frames) {
        ret = joinVectors({ ret, f->layers });
    }
    return ret;
}

void MainEditor::resizeAllLayersFromCommand(XY size, bool byTile)
{
    if (byTile) {
        if (xyEqual(tileDimensions, size)) {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Tile size must be different to resize."));
            return;
        }
    }
    else {
        if (xyEqual(canvas.dimensions, size)) {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Size must be different to resize."));
            return;
        }
    }
    std::vector<Layer*> layers = getAllLayers();

    std::map<Layer*, LayerScaleData> createdVariants;

    for (Layer* target : layers) {
        LayerScaleData scaleResult = byTile ? target->resizeByTileSizes(tileDimensions, size) : target->resize(size);
        if (scaleResult.success) {
            createdVariants[target] = scaleResult;
        }
        else {
            for (auto& [layer, variants] : createdVariants) {
                for (auto& variant : variants.scaledVariants) {
                    tracked_free(variant.pixelData);
                }
            }
            return;
        }
        
        target->markLayerDirty();
    }

    XY newSize = createdVariants[layers[0]].newSize;

    UndoLayersResized* undoData = new UndoLayersResized(canvas.dimensions, newSize, tileDimensions, byTile ? size : tileDimensions);
    for (Layer*& l : layers) {
        undoData->storedLayerData[l] = l->layerData;
    }

    for (auto& [layer, variants] : createdVariants) {
        layer->setLayerData(variants.scaledVariants, variants.newSize);
    }

    canvas.dimensions = newSize;
    if (byTile) {
        tileDimensions = size;
    }
    
    addToUndoStack(undoData);

}

void MainEditor::resizzeAllLayersByTilecountFromCommand(XY size)
{
    std::vector<Layer*> layers = getAllLayers();

    XY newSize = { size.x * tileDimensions.x, size.y * tileDimensions.y };
    UndoLayersResized* undoData = new UndoLayersResized(canvas.dimensions, newSize, tileDimensions, tileDimensions);

    int nLayers = layers.size();
    for (Layer* l : layers) {
        undoData->storedLayerData[l] = l->resizeByTileCount(tileDimensions, size);
        l->markLayerDirty();
    }
    canvas.dimensions = newSize;

    addToUndoStack(undoData);
}

void MainEditor::resizeAllLayersReorderingTilesFromCommand(XY size)
{
    if (tileDimensions.x == 0 || tileDimensions.y == 0) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Tile dimensions must be set."));
        return;
    }
    XY oldTileCount = { 
        (canvas.dimensions.x + (tileDimensions.x - 1)) / tileDimensions.x, 
        (canvas.dimensions.y + (tileDimensions.y - 1)) / tileDimensions.y
    };
    resizeAllLayersFromCommand(size, false);
    XY newTileCount = {
        (canvas.dimensions.x + (tileDimensions.x - 1)) / tileDimensions.x,
        (canvas.dimensions.y + (tileDimensions.y - 1)) / tileDimensions.y
    };
    if (!xyEqual(oldTileCount, newTileCount)) {
        std::vector<Layer*> targetLayers = getAllLayers();

        for (Layer* l : targetLayers) {
            SDL_Rect sourceRect = { 0,0, tileDimensions.x, tileDimensions.y };
            Layer* temp = l->isPalettized ? LayerPalettized::tryAllocLayer(l->w, l->h)
                          : Layer::tryAllocLayer(l->w, l->h);
            if (temp != NULL) {
                for (int tileInd = 0; tileInd < newTileCount.x * newTileCount.y; tileInd++) {
                    XY srcTilePos = {
                        tileInd % oldTileCount.x,
                        tileInd / oldTileCount.x
                    };
                    XY dstTilePos = {
                        tileInd % newTileCount.x,
                        tileInd / newTileCount.x
                    };
                    SDL_Rect sourceRect = canvas.getTileRectAt(srcTilePos, tileDimensions);
                    SDL_Rect destRect = canvas.getTileRectAt(dstTilePos, tileDimensions);

                    temp->blit(l, { destRect.x, destRect.y }, sourceRect, true);
                }

                memcpy(l->pixels32(), temp->pixels32(), l->w * l->h * 4);

                delete temp;
            }
            else {
                logerr("layer malloc failed");
            }
        }
    }
}

void MainEditor::integerScaleAllLayersFromCommand(XY scale, bool downscale)
{
    if (scale.x == 0 || scale.y == 0 || (scale.x == 1 && scale.y == 1)) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid scale."));
        return;
    }
    else if (downscale && (canvas.dimensions.x % scale.x != 0 || canvas.dimensions.y % scale.y != 0)) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Dimensions not divisible."));
        return;
    }
    std::vector<Layer*> layers = getAllLayers();

    XY newSize = downscale ? XY{ canvas.dimensions.x / scale.x, canvas.dimensions.y / scale.y }
                           : XY{ canvas.dimensions.x * scale.x, canvas.dimensions.y * scale.y };
    XY newTileSize = downscale ? XY{ tileDimensions.x / scale.x, tileDimensions.y / scale.y }
                               : XY{ tileDimensions.x * scale.x, tileDimensions.y * scale.y };

    std::map<Layer*, LayerScaleData> createdVariants;

    for (Layer*& target : layers) {
        LayerScaleData scaleResult = downscale ? target->integerDownscale(scale) : target->integerScale(scale);
        if (scaleResult.success) {
            createdVariants[target] = scaleResult;
        }
        else {
            for (auto& [layer, variants] : createdVariants) {
                for (auto& variant : variants.scaledVariants) {
                    tracked_free(variant.pixelData);
                }
            }
            return;
        }

        target->markLayerDirty();
    }

    UndoLayersResized* undoData = new UndoLayersResized(canvas.dimensions, newSize, tileDimensions, newTileSize);
    for (Layer*& l : layers) {
        undoData->storedLayerData[l] = l->layerData;
    }

    for (auto& [layer, variants] : createdVariants) {
        layer->setLayerData(variants.scaledVariants, variants.newSize);
    }

    canvas.dimensions = { layers[0]->w, layers[0]->h };
    tileDimensions = newTileSize;

    addToUndoStack(undoData);
}

MainEditorPalettized* MainEditor::toPalettizedSession()
{
    if (isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "?????"));
        return NULL;
    }
    else {
        auto& layers = getLayerStack();

        std::map<uint32_t, bool> usedColors;
        for (Layer*& l : layers) {
            std::vector<uint32_t> cols = l->getUniqueColors();
            for (uint32_t c : cols) {
                usedColors[c] = true;
            }
        }

        if (usedColors.size() <= 256) {

            std::vector<uint32_t> palette;
            for (auto& c : usedColors) {
                palette.push_back(c.first);
            }

            std::vector<LayerPalettized*> outlayers;
            for (Layer*& l : layers) {
                LayerPalettized* newLayer = new LayerPalettized(l->w, l->h);
                newLayer->name = l->name;
                newLayer->hidden = l->hidden;
                newLayer->palette = palette;

                uint32_t* ppx = l->pixels32();
                uint32_t* outpx = newLayer->pixels32();
                for (uint64_t p = 0; p < l->w * l->h; p++) {
                    outpx[p] = std::find(palette.begin(), palette.end(), ppx[p]) - palette.begin();
                }
                outlayers.push_back(newLayer);
            }
            MainEditorPalettized* ret = new MainEditorPalettized(outlayers);
            ret->palette = palette;
            ret->tileDimensions = tileDimensions;
            return ret;
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Too many colors in current session"));
            return NULL;
        }
    }
}

void MainEditor::tryExportPalettizedImage()
{
    std::vector<std::pair<std::string, std::string>> formats;
    for (auto f : g_palettizedFileExporters) {
        formats.push_back({ f->extension(), f->name() });
    }
    platformTrySaveOtherFile(this, formats, "export image as palettized", EVENT_MAINEDITOR_EXPORTPALETTIZED);
}

void MainEditor::exportTilesIndividually()
{
    if (tileDimensions.x != 0 && tileDimensions.y != 0) {
        std::vector<std::pair<std::string, std::string>> formats;
        for (auto f : g_fileExporters) {
            formats.push_back({ f->extension(), f->name() });
        }
        platformTrySaveOtherFile(this, formats, "export tiles", EVENT_MAINEDITOR_EXPORTTILES);
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Set the pixel grid first."));
    }
}

std::string MainEditor::networkGetSocketAddress(NET_StreamSocket* sock)
{
#if VSP_NETWORKING
    NET_Address* addr = NET_GetStreamSocketAddress(sock);
    DoOnReturn a([&]() { NET_UnrefAddress(addr); });
    NET_GetAddressString(addr);
    int resolved = NET_WaitUntilResolved(addr, -1);
    if (resolved == 1) {
        const char* clientAddressStr = NET_GetAddressString(addr);
        return clientAddressStr != NULL ? std::string(clientAddressStr) : "<address unknown>";
    }
    return "<address unknown>";
#endif
    return "";
}

void MainEditor::promptStartNetworkSession()
{
    if (networkCanvasThread != NULL) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Network session already started"));
        return;
    }
    bool showRPCSettings = platformSupportsFeature(VSP_FEATURE_DISCORD_RPC) && g_config.useDiscordRPC;
    PopupSetupNetworkCanvas* p = new PopupSetupNetworkCanvas(TL("vsp.maineditor.startcollab"), TL("vsp.collabeditor.popup.host.desc"), false, true, showRPCSettings);
    p->onInputConfirmCallback = [this, showRPCSettings](PopupSetupNetworkCanvas* p, PopupSetNetworkCanvasData d) {
        networkRunning = true;

        networkCanvasPassword = d.password;
        networkCanvasPort = d.port;

        networkCanvasCurrentChatState = new NetworkCanvasChatHostState;
        networkCanvasChatPanel = new EditorNetworkCanvasChatPanel(this);
        networkCanvasChatPanel->position.y = 64;
        networkCanvasChatPanel->position.x = 730;
        wxsManager.addDrawable(networkCanvasChatPanel);

        
        networkCanvasHostPanel = new EditorNetworkCanvasHostPanel(this);
        networkCanvasHostPanel->position.y = 64;
        networkCanvasHostPanel->position.x = 420;
        wxsManager.addDrawable(networkCanvasHostPanel);

        thisClientInfo = new NetworkCanvasClientInfo;
        thisClientInfo->clientIP = "localhost";

        if (showRPCSettings && p->rpcEnabled && g_lockRPCLobbyInfo()) {
            networkCanvasBroadcastRPC = true;
            networkCanvasLobbyID = randomUUID();
            networkCanvasRPCAddress = p->textboxExternalIP->getText();
            networkCanvasRPCPrivate = p->rpcLobbyPrivate;
        }

        networkCanvasThread = new std::thread(&MainEditor::networkCanvasServerThread, this, d);

        g_addNotification(SuccessNotification("Network canvas started", ""));
    };
    g_addPopup(p);
}

void MainEditor::networkCanvasStateUpdated(int whichFrame, int whichLayer)
{
    canvasStateID++;
}

void MainEditor::networkCanvasServerThread(PopupSetNetworkCanvasData startData)
{
#if VSP_NETWORKING
    NET_Server* server = NET_CreateServer(NULL, startData.port);

    thisClientInfo->uid = nextClientUID++;
    thisClientInfo->clientName = startData.username;
    thisClientInfo->clientColor = startData.userColor;
    thisClientInfo->cursorPosition = { 0, 0 };

    if (server == NULL) {
        g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), "Failed to create network server"));
        return;
    }

    networkClientsListMutex.lock();
    networkClients.clear();
    networkClients.push_back(thisClientInfo);
    networkClientsListMutex.unlock();

    mainThreadOps.add([this]() {
        networkCanvasHostPanel->updateClientList();
    });
    

    while (networkRunning) {
        NET_StreamSocket* clientSocket = NULL;
        NET_AcceptClient(server, &clientSocket);
        if (clientSocket == NULL) {
            SDL_Delay(100);
        }
        else {
            loginfo(frmt("New client connected: {}", networkGetSocketAddress(clientSocket)));
            std::thread* responderThread = new std::thread(&MainEditor::networkCanvasServerResponderThread, this, clientSocket);
            g_startNewMainThreadOperation([this, responderThread]() {
                this->networkCanvasResponderThreads.push_back(responderThread);
            });
        }
    }
    NET_DestroyServer(server);
    networkClientsListMutex.lock();
    networkClients.clear();
    networkClientsListMutex.unlock();
#else
    logerr("Attempted to run network thread in non-network build");
#endif
}

void MainEditor::networkCanvasServerResponderThread(NET_StreamSocket* clientSocket)
{
#if VSP_NETWORKING
    std::string authCommand = networkReadCommand(clientSocket);
    if (authCommand != "AUTH" || !networkCanvasProcessAUTHCommand(networkReadString(clientSocket))) {
        networkSendCommand(clientSocket, "HSDC");
        networkSendString(clientSocket, "Authentication failed");
        NET_DestroyStreamSocket(clientSocket);
        return;
    }

    //todo: delete this thread from the responder threads list when done
    NetworkCanvasClientInfo clientInfo;
    clientInfo.uid = nextClientUID++;
    clientInfo.clientIP = networkGetSocketAddress(clientSocket);
    bool receivedName = false;

    networkClientsListMutex.lock();
    networkClients.push_back(&clientInfo);
    networkClientsListMutex.unlock();

    while (networkRunning && !clientInfo.hostKick) {
        try {
            std::string commandStr = networkReadCommand(clientSocket);
            networkCanvasProcessCommandFromClient(commandStr, clientSocket, &clientInfo);
            if (!receivedName && commandStr == "INFO") {
                receivedName = true;
                networkCanvasSystemMessage(frmt("{} connected", clientInfo.clientName));
            }
        }
        catch (std::exception& e) {
            logerr(frmt("Error processing command from client: {}", e.what()));
            break;
        }
    }
    if (clientInfo.hostKick) {
        networkSendCommand(clientSocket, "HSDC");
        networkSendString(clientSocket, "Kicked by host");
    }

    networkClientsListMutex.lock();
    networkClients.erase(std::remove(networkClients.begin(), networkClients.end(), &clientInfo), networkClients.end());
    networkClientsListMutex.unlock();

    NET_DestroyStreamSocket(clientSocket);
    mainThreadOps.add([this]() {
        networkCanvasHostPanel->updateClientList();
    });
    g_addNotificationFromThread(Notification("User disconnected", clientInfo.clientName));
    networkCanvasSystemMessage(frmt("{} disconnected", clientInfo.clientName));
    logerr(frmt("Disconnected from {}", clientInfo.clientName));
#endif
    
}

void MainEditor::networkCanvasProcessCommandFromClient(std::string command, NET_StreamSocket* clientSocket, NetworkCanvasClientInfo* clientInfo)
{
    clientInfo->lastReportTime = SDL_GetTicks();
    if (command == "INFO") {
        bool anyDataUpdated = false;
        std::string inputString = networkReadString(clientSocket);
        json inputJson = json::parse(inputString);

        std::string newClientName = inputJson.value("clientName", "Unknown Client");
        anyDataUpdated |= (newClientName != clientInfo->clientName);
        clientInfo->clientName = newClientName;

        clientInfo->activeFrame = inputJson.value("activeFrame", 0);

        clientInfo->cursorPosition = XY{ inputJson.value("cursorX", 0), inputJson.value("cursorY", 0)};
        try {
            u32 newClientColor = std::stoi(inputJson.value("clientColor", "C0E1FF"), 0, 16);
            anyDataUpdated |= (newClientColor != clientInfo->clientColor);
            clientInfo->clientColor = newClientColor;
        }
        catch (std::exception&) {
            clientInfo->clientColor = 0xC0E1FF; // default color if parsing fails
        }

        json infoJson = {
            {"yourUserIDIs", clientInfo->uid},
            {"serverName", "---"},
            {"canvasWidth", canvas.dimensions.x},
            {"canvasHeight", canvas.dimensions.y},
            {"tileGridWidth", tileDimensions.x},
            {"tileGridHeight", tileDimensions.y},
            {"chatState", (u32)networkCanvasCurrentChatState->messagesState},
            {"frameTime", frameAnimMSPerFrame},
            {"frames", json::array()},
            {"clients", json::array()}
        };
        framesMutex.lock();
        for (Frame*& f : frames) {
            json layersJson = json::array();
            for (Layer*& l : f->layers) {
                json layerJson = {
                    {"name", l->name},
                    {"hidden", l->hidden},
                    {"opacity", l->layerAlpha}
                };
                layersJson.push_back(layerJson);
            }
            infoJson["frames"].push_back({
                {"layers", layersJson}
            });
        }
        framesMutex.unlock();
        
        networkClientsListMutex.lock();
        thisClientInfo->cursorPosition = mousePixelTargetPoint;
        thisClientInfo->lastReportTime = SDL_GetTicks();
        for (NetworkCanvasClientInfo* c : networkClients) {
            json clientJson = {
                {"uid", c->uid},
                {"clientName", c->clientName},
                {"cursorX", c->cursorPosition.x},
                {"cursorY", c->cursorPosition.y},
                {"clientColor", frmt("{:06X}", 0xFFFFFF&c->clientColor)},
                {"lastReportTime", (SDL_GetTicks() - c->lastReportTime)},
                {"activeFrame", c->activeFrame}
            };
            infoJson["clients"].push_back(clientJson);
        }
        networkClientsListMutex.unlock();

        if (anyDataUpdated) {
            mainThreadOps.add([this]() {
                networkCanvasHostPanel->updateClientList();
            });
        }

        networkSendCommand(clientSocket, "INFO");
        networkSendString(clientSocket, infoJson.dump());
        
    }
    //layer data request
    else if (command == "LRRQ") {
        u32 frameIndex;
        u32 index;
        networkReadBytes(clientSocket, (u8*)&frameIndex, 4);
        networkReadBytes(clientSocket, (u8*)&index, 4);

        if (frameIndex < frames.size() && index < frames[frameIndex]->layers.size()) {
            Layer* l = frames[frameIndex]->layers[index];
            networkCanvasSendLRDT(clientSocket, frameIndex, index, l);
        }
    }
    //layer pixel data update request
    else if (command == "LRDT") {
        u32 frameIndex;
        u32 index;
        u64 dataSize;
        networkReadBytes(clientSocket, (u8*)&frameIndex, 4);
        networkReadBytes(clientSocket, (u8*)&index, 4);
        networkReadBytes(clientSocket, (u8*)&dataSize, 8);
        u8* dataBuffer = (u8*)tracked_malloc(dataSize);
        if (dataBuffer != NULL) {
            networkReadBytes(clientSocket, dataBuffer, dataSize);
            Layer* l = frames[frameIndex]->layers[index];
            auto decompressed = decompressZlibWithoutUncompressedSize(dataBuffer, dataSize);
            if (decompressed.size() != l->w * l->h * 4) {
                logerr(frmt("Decompressed data size mismatch: expected {}, got {}", l->w * l->h * 4, decompressed.size()));
            }
            else {
                if (index != selLayer || !leftMouseHold) {
                    memcpy(l->pixels8(), decompressed.data(), 4ull * l->w * l->h);
                    l->markLayerDirty();
                    changesSinceLastSave = HAS_UNSAVED_CHANGES;
                    networkCanvasStateUpdated(frameIndex, index);
                }
            }
            tracked_free(dataBuffer);
        }
        else {
            logerr("Failed to allocate memory for layer pixel data update");
        }
    }
    else if (command == "UPDD") {
        networkSendCommand(clientSocket, "UPDD");
        networkSendBytes(clientSocket, (u8*)&canvasStateID, 4);
    }
    else if (command == "CHTS") {
        std::string chatState = ((NetworkCanvasChatHostState*)(networkCanvasCurrentChatState))->toJson();
        networkSendCommand(clientSocket, "CHTD");
        networkSendString(clientSocket, chatState);
    }
    else if (command == "CHTQ") {
        std::string message = networkReadString(clientSocket);
        if (message.size() < 720) {
            NetworkCanvasChatMessage msg;
            msg.fromColor = clientInfo->clientColor;
            msg.fromName = clientInfo->clientName;
            msg.message = message;
            msg.messageColor = 0xFFFFFFFF;
            msg.timestamp = std::time(nullptr);
            ((NetworkCanvasChatHostState*)(networkCanvasCurrentChatState))->newMessage(msg);
            g_startNewMainThreadOperation([this]() {
                if (networkCanvasChatPanel != NULL) {
                    networkCanvasChatPanel->updateChat();
                }
            });
        }
    }
    else {
        logerr(frmt("Unknown command from client: {}", command));
    }
}

bool MainEditor::networkCanvasProcessAUTHCommand(std::string request)
{
    try {
        json inputJson = json::parse(request);
        int version = inputJson.value("version", 1);
        if (version != 2) {
            logerr(frmt("client version mismatch {}", version));
            return false;
        }
        std::string password = inputJson.value("password", "");
        return (networkCanvasPassword == "" || password == networkCanvasPassword);
        
    } catch (std::exception&) {
        return false;
    }
    return false;
}

std::string MainEditor::networkReadCommand(NET_StreamSocket* socket)
{
    std::string commandName;
    commandName.resize(8);
    networkReadBytes(socket, (u8*)&commandName[0], 8);
    if (commandName.substr(0, 4) != "VDNC") {
        throw std::runtime_error(frmt("Invalid command prefix: {}", commandName));
    }
    else {
        return commandName.substr(4);
    }
}

bool MainEditor::networkReadCommandIfAvailable(NET_StreamSocket* socket, std::string& outCommand)
{
#if VSP_NETWORKING
    int bytesToRead = 8 - outCommand.size();
    char buffer[9];
    memset(buffer, 0, 9);
    int bytesRead = NET_ReadFromStreamSocket(socket, buffer, bytesToRead);
    if (bytesRead == -1) {
        throw std::runtime_error("Failed to read from network socket");
    }
    outCommand += std::string(buffer, bytesRead);
    if (outCommand.size() == 8) {
        std::string cmd = outCommand;
        outCommand = "";

        if (cmd.substr(0,4) != "VDNC") {
            logerr("Invalid command prefix: " + cmd);
            return false;
        }
        outCommand = cmd.substr(4);
        return true;
    }

    return false;
#else
    return false;
#endif
}

void MainEditor::networkSendCommand(NET_StreamSocket* socket, std::string commandName)
{
    std::string fullCommand = "VDNC" + commandName;
    if (fullCommand.size() > 8) {
        throw std::runtime_error("Command name too long");
    }
    networkSendBytes(socket, (u8*)&fullCommand[0], 8);
}

bool MainEditor::networkReadBytes(NET_StreamSocket* socket, u8* buffer, u32 count)
{
#if VSP_NETWORKING
    int read = 0;
    while (read < count) {
        int bytesRead = NET_ReadFromStreamSocket(socket, buffer + read, count - read);
        if (bytesRead == -1) {
            return false;
        }
        else {
            read += bytesRead;
        }
    }
    return true;
#else
    return false;
#endif
}

void MainEditor::networkSendBytes(NET_StreamSocket* socket, u8* buffer, u32 count)
{
#if VSP_NETWORKING
    NET_WriteToStreamSocket(socket, buffer, count);
#endif
}

void MainEditor::networkSendString(NET_StreamSocket* socket, std::string s)
{
    u32 size = s.size();
    networkSendBytes(socket, (u8*)&size, 4);
    if (size < 8000) {
        networkSendBytes(socket, (u8*)&s[0], size);
    }
    else {
        throw std::runtime_error("networkSendString: size too big");
    }
}

void MainEditor::networkCanvasSendLRDT(NET_StreamSocket* socket, int frameIndex, int index, Layer* l)
{
    auto compressedData = compressZlib(l->pixels8(), 4ull * l->w * l->h);
    u64 dataSize = compressedData.size();
    networkSendCommand(socket, "LRDT");
    networkSendBytes(socket, (u8*)&frameIndex, 4);
    networkSendBytes(socket, (u8*)&index, 4);
    networkSendBytes(socket, (u8*)&dataSize, 8);
    networkSendBytes(socket, (u8*)&compressedData[0], dataSize);
}

std::string MainEditor::networkReadString(NET_StreamSocket* socket)
{
    u32 size;
    networkReadBytes(socket, (u8*)&size, 4);
    if (size < 128000) {
        std::string ret;
        ret.resize(size);
        networkReadBytes(socket, (u8*)&ret[0], size);
        return ret;
    }
    else {
        throw std::runtime_error("networkReadString: size too big");
    }
}

void MainEditor::networkCanvasKickUID(u32 uid)
{
    if (thisClientInfo != NULL && thisClientInfo->uid == uid) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.collabeditor.error.kickhost")));
        return;
    }
    networkClientsListMutex.lock();
    for (auto& client : networkClients) {
        if (client->uid == uid) {
            networkCanvasSystemMessage(frmt("{} kicked", client->clientName));
            client->hostKick = true;
            break;
        }
    }
    networkClientsListMutex.unlock();
}

void MainEditor::networkCanvasSystemMessage(std::string msg)
{
    NetworkCanvasChatMessage msgEntry;
    msgEntry.fromColor = 0xFF;
    msgEntry.fromName = "";
    msgEntry.message = frmt(UTF8_DIAMOND "{}", msg);
    if (msgEntry.message.size() > 720) {
        msgEntry.message = msgEntry.message.substr(0, 717);
        msgEntry.message += "...";
    }
    msgEntry.messageColor = 0xFFFFF682;
    msgEntry.timestamp = std::time(nullptr);
    ((NetworkCanvasChatHostState*)(networkCanvasCurrentChatState))->newMessage(msgEntry);
    mainThreadOps.add([this]() {
        if (networkCanvasChatPanel != NULL) {
            networkCanvasChatPanel->updateChat();
        }
    });
}

void MainEditor::networkCanvasChatSendCallback(std::string content)
{
    if (content.size() < 720) {
        NetworkCanvasChatMessage msg;
        msg.fromName = thisClientInfo->clientName;
        msg.fromColor = thisClientInfo->clientColor;
        msg.message = content;
        msg.messageColor = 0xFFFFFFFF;
        msg.timestamp = std::time(nullptr);
        ((NetworkCanvasChatHostState*)networkCanvasCurrentChatState)->newMessage(msg);
        networkCanvasChatPanel->updateChat();
    }
}

void MainEditor::networkCanvasBroadcastToLAN()
{
#if VSP_NETWORKING
    auto adapters = platformGetNetworkAdapters();

    for (auto& adapter : adapters) {
        NET_Address* addr = NET_ResolveHostname(adapter.broadcastAddress.c_str());
        NET_Address* addr2 = NET_ResolveHostname(adapter.thisMachineAddress.c_str());
        NET_WaitUntilResolved(addr, -1);
        NET_WaitUntilResolved(addr2, -1);
        //loginfo(frmt("Broadcasting to {}...", NET_GetAddressString(addr)));
        if (addr != NULL) {
            DoOnReturn unrefAddr([addr]() {NET_UnrefAddress(addr); });
            DoOnReturn unrefAddr2([addr2]() {NET_UnrefAddress(addr2); });
            NET_DatagramSocket* sock = NET_CreateDatagramSocket(addr2, LAN_BROADCAST_PORT);
            if (sock != NULL) {
                DoOnReturn freeSock([sock]() { NET_DestroyDatagramSocket(sock); });
                std::string str = frmt("vspcollab:{}", networkCanvasPort);
                bool r = NET_SendDatagram(sock, addr, LAN_BROADCAST_PORT, (u8*)str.c_str(), str.size());
            }
        }
    }
    networkCanvasLastLANBroadcast.start();
#endif
}

void MainEditor::endNetworkSession()
{
    networkRunning = false;

    if (networkCanvasBroadcastRPC) {
        g_clearRPCLobbyInfo();
        networkCanvasBroadcastRPC = false;
    }

    for (std::thread* responderThread : networkCanvasResponderThreads) {
        responderThread->join();
        delete responderThread;
    }
    if (networkCanvasThread != NULL) {
        networkCanvasThread->join();
        delete networkCanvasThread;
        networkCanvasThread = NULL;
    }
    if (thisClientInfo != NULL) {
        delete thisClientInfo;
        thisClientInfo = NULL;
    }
    //are these even required
    //yes
    if (networkCanvasHostPanel != NULL) {
        removeWidget(networkCanvasHostPanel);
        networkCanvasHostPanel = NULL;
    }
    if (networkCanvasChatPanel != NULL) {
        removeWidget(networkCanvasChatPanel);
        networkCanvasChatPanel = NULL;
    }

    if (networkCanvasCurrentChatState != NULL) {
        delete networkCanvasCurrentChatState;
        networkCanvasCurrentChatState = NULL;
    }
}

bool MainEditor::canAddCommentAt(XY a)
{
    for (CommentData& c : getCommentStack()) {
        if (xyEqual(c.position, a)) {
            return false;
        }
    }
    return true;
}

void MainEditor::addComment(CommentData c)
{
    if (canAddCommentAt(c.position)) {
        getCommentStack().push_back(c);
        addToUndoStack(new UndoCommentAdded(getCurrentFrame(), c));
    }
}

void MainEditor::addCommentAt(XY a, std::string c)
{
    CommentData newComment = { a, c };
    addComment(newComment);
}

void MainEditor::removeCommentAt(XY a)
{
    CommentData c = _removeCommentAt(a);
    if (c.data[0] != '\1') {
        addToUndoStack(new UndoCommentRemoved(getCurrentFrame(), c));
    }
}

CommentData MainEditor::_removeCommentAt(XY a)
{
    auto& comments = getCommentStack();
    for (int x = 0; x < comments.size(); x++) {
        if (xyEqual(comments[x].position, a)) {
            CommentData c = comments[x];
            comments.erase(comments.begin() + x);
            return c;
        }
    }
    logerr("_removeComment NOT FOUND");
    return { {0,0}, "\1" };
}

void MainEditor::layer_replaceColor(uint32_t from, uint32_t to)
{
    //commitStateToCurrentLayer();
    getCurrentLayer()->replaceColor(from, to, isolateEnabled ? &isolatedFragment : NULL);
}

void MainEditor::layer_hsvShift(hsv shift)
{
    commitStateToCurrentLayer();
    getCurrentLayer()->shiftLayerHSV(shift);
}

void MainEditor::layer_outline(bool wholeImage)
{

    Layer* l = getCurrentLayer();
    uint8_t* placePixelData = (uint8_t*)tracked_malloc(l->w * l->h);
    if (placePixelData == NULL) {
        g_addNotification(NOTIF_MALLOC_FAIL);
        return;
    }
    commitStateToCurrentLayer();

    for (int y = 0; y < l->h; y++) {
        for (int x = 0; x < l->w; x++) {
            ARRAY2DPOINT(placePixelData, x, y, l->w) =
                wholeImage ? 0
                : ((l->isPalettized ? (l->getPixelAt({ x,y }) == -1) : (l->getPixelAt({ x,y }) & 0xFF000000) == 0) ? 0 : 1);
        }
    }

    XY neighbors[4] = { {1,0}, {0,1}, {-1,0}, {0,-1} };

    for (int y = 0; y < l->h; y++) {
        for (int x = 0; x < l->w; x++) {
            XY newPos = { x,y };
            if (ARRAY2DPOINT(placePixelData, newPos.x, newPos.y, l->w) == 1) {
                continue;
            }

            int neighborCount = 0;
            for (XY& neighbor : neighbors) {
                XY checkPos = xyAdd(newPos, neighbor);
                if (pointInBox(checkPos, { 0,0,l->w,l->h })) {
                    if (ARRAY2DPOINT(placePixelData, checkPos.x, checkPos.y, l->w)) {
                        neighborCount++;
                    }
                }
            }
            if (neighborCount > 0) {
                l->setPixel(newPos, getActiveColor());
            }
        }
    }

    tracked_free(placePixelData);
}

void MainEditor::layer_selectCurrentAlpha()
{
    Layer* l = getCurrentLayer();
    isolatedFragment.clear();
    int p = 0;
    for (int y = 0; y < l->h; y++) {
        for (int x = 0; x < l->w; x++) {
            u32 px = l->getPixelAt({ x,y });
            if (l->isPalettized ? (px != -1) : ((px & 0xFF000000) != 0)) {
                isolatedFragment.addPoint({ x,y });
                p++;
            }
        }
    }
    shouldUpdateRenderedIsolatedFragmentPoints = true;
    isolateEnabled = p > 0;
}

void MainEditor::layer_fillActiveColor()
{
    commitStateToCurrentLayer();
    g_startNewOperation([this]() {
        if (!isolateEnabled) {
            for (int x = 0; x < canvas.dimensions.x; x++) {
                for (int y = 0; y < canvas.dimensions.y; y++) {
                    SetPixel({ x,y }, pickedColor, false);
                }
            }
        }
        else {
            Layer* currentLayer = getCurrentLayer();
            isolatedFragment.forEachPoint([this, currentLayer](XY p) {
                SetPixel(p, pickedColor, false);
            });
        }
    });
}

EditorNetworkCanvasHostPanel::EditorNetworkCanvasHostPanel(MainEditor* caller, bool clientSide)
{
    this->clientSide = clientSide;
    parent = caller;
    wxWidth = 300;
    wxHeight = 200;

    setupDraggable();
    setupCollapsible();
    addTitleText(TL("vsp.maineditor.panel.netcollab.host.title"));

    clientList = new ScrollingPanel();
    clientList->position = { 5, 30 };
    clientList->wxWidth = wxWidth - 10;
    clientList->wxHeight = wxHeight - 35;
    wxsTarget().addDrawable(clientList);

}

void EditorNetworkCanvasHostPanel::updateClientList()
{
    clientList->subWidgets.freeAllDrawables();
    int clientY = 0;
    parent->networkClientsListMutex.lock();
    for (auto*& client : parent->networkClients) {
        UIButton* clientButton = new UIButton();
        clientButton->text = std::string((clientSide ? (client->uid == parent->thisClientInfo->uid) : (client == parent->thisClientInfo)) ? UTF8_DIAMOND : "") + client->clientName;
        clientButton->colorTextFocused = clientButton->colorTextUnfocused = uint32ToSDLColor(0xFF000000|client->clientColor);
        clientButton->position = { 0, clientY };
        clientButton->onClickCallback = [this, client](UIButton* b) {
            parent->canvas.centerOnPoint(client->cursorPosition);
        };
        u32 uid = client->uid;
        if (!clientSide) {
            clientButton->tooltip = client->clientIP;
            clientButton->onRightClickCallback = [this, uid](UIButton* b) {
                g_openContextMenu({
                    {TL("vsp.collabeditor.ctx.kickuser"),
                        [this, uid]() {
                            parent->networkCanvasKickUID(uid);
                        }
                    }
                    });
                };
        }
        clientList->subWidgets.addDrawable(clientButton);

        clientY += 30;
    }
    parent->networkClientsListMutex.unlock();
}

void NetworkCanvasChatHostState::newMessage(NetworkCanvasChatMessage msg) {
    messagesMutex.lock();
    messages.push_back(msg);
    if (messages.size() > 100) {
        messages.erase(messages.begin(), messages.begin() + (messages.size() - 100));
    }
    nextState();
    messagesMutex.unlock();
}

std::string NetworkCanvasChatHostState::toJson() {
    std::lock_guard<std::mutex> guard(messagesMutex);
    try {
        json jmsgs = json::array();
        for (auto& msg : messages) {
            jmsgs.push_back(json::object({
                { "from", msg.fromName },
                { "fromColor", frmt("{:06X}", msg.fromColor & 0xFFFFFF) },
                { "message", msg.message },
                { "messageColor", frmt("{:06X}", msg.messageColor & 0xFFFFFF) },
                { "timestamp", msg.timestamp }
                }));
        }
        json j = json::object({
            { "messages", jmsgs },
            { "state", (u32)messagesState }
        });
        return j.dump();
    }
    catch (std::exception&) {
        return "{}";
    }
}

void NetworkCanvasChatState::fromJson(std::string jsonStr)
{
    std::lock_guard<std::mutex> guard(messagesMutex);
    try {
        json j = json::parse(jsonStr);
        messages.clear();
        for (auto& jmsg : j["messages"]) {
            NetworkCanvasChatMessage msg;
            msg.fromName = frmt("{}", jmsg.value("from", "???"));
            try {
                msg.fromColor = std::stoi(jmsg.value("fromColor", "C0E1FF"), 0, 16);
            }
            catch (std::exception&) {
                msg.fromColor = 0xC0E1FF;
            }
            msg.message = jmsg.value("message", "");
            try {
                msg.messageColor = std::stoi(jmsg.value("messageColor", "000000"), 0, 16);
            }
            catch (std::exception&) {
                msg.messageColor = 0x000000;
            }
            msg.timestamp = jmsg.value("timestamp", 0);
            messages.push_back(msg);
        }
        messagesState = j.value("state", 0);
    }
    catch (std::exception&) {
        messages.clear();
        messagesState = 0;
    }
}

EditorNetworkCanvasChatPanel::EditorNetworkCanvasChatPanel(MainEditor* caller, bool clientSide)
{
    parent = caller;
    this->clientSide = clientSide;
    wxWidth = 300;
    wxHeight = 250;

    setupDraggable();
    setupCollapsible();
    addTitleText(TL("vsp.maineditor.panel.netcollab.chat.title"));

    chatMsgPanel = new ScrollingPanel();
    chatMsgPanel->position = { 5, 30 };
    chatMsgPanel->wxWidth = wxWidth - 10;
    chatMsgPanel->wxHeight = wxHeight - 35 - 35;
    wxsTarget().addDrawable(chatMsgPanel);

    UITextField* inputField = new UITextField();
    inputField->position = { 5, wxHeight - 35 };
    inputField->wxWidth = wxWidth - 10;
    inputField->wxHeight = 30;

    inputField->onTextChangedConfirmCallback = [this, inputField](UITextField*, std::string text) {
        parent->networkCanvasChatSendCallback(text);
        inputField->clearText();
    };

    wxsTarget().addDrawable(inputField);
}

void EditorNetworkCanvasChatPanel::updateChat()
{ 
    chatMsgPanel->subWidgets.freeAllDrawables();
    int msgY = 0;
    parent->networkCanvasCurrentChatState->messagesMutex.lock();
    for (auto& msg : parent->networkCanvasCurrentChatState->messages) {
        UILabel* nameText = new UILabel();
        nameText->setText(msg.fromName.empty() ? std::string("") : frmt("<{}>: ", msg.fromName));
        nameText->color = uint32ToSDLColor(0xFF000000 | msg.fromColor);
        nameText->position = { 0, msgY };
        nameText->fontsize = 14;
        chatMsgPanel->subWidgets.addDrawable(nameText);

        UILabel* msgText = new UILabel();
        msgText->setText(msg.message);
        msgText->color = uint32ToSDLColor(0xFF000000 | msg.messageColor);
        msgText->position = { nameText->calcEndpoint().x, msgY};
        msgText->fontsize = 14;
        if (msgText->calcEndpoint().x > chatMsgPanel->wxWidth) {
            std::string remainingText = msg.message;
            UILabel* labelNow = msgText;
            int xNow = nameText->calcEndpoint().x;
            while (!remainingText.empty()) {
                int chrsInLine = 1;
                while (remainingText.size() > 1 && chrsInLine < remainingText.size() && g_fnt->StatStringEndpoint(remainingText.substr(0, chrsInLine + 1), xNow, 0, 14).x < chatMsgPanel->wxWidth) {
                    chrsInLine++;
                }
                labelNow->setText(remainingText.substr(0, chrsInLine));
                chatMsgPanel->subWidgets.addDrawable(labelNow);
                remainingText = remainingText.substr(chrsInLine);

                chrsInLine = ixmax(1, chrsInLine);
                xNow = 0;
                labelNow = new UILabel();
                labelNow->position = { 0, msgY += 22 };
                labelNow->fontsize = 14;
                labelNow->color = uint32ToSDLColor(0xFF000000 | msg.messageColor);
            }
        }
        else {
            chatMsgPanel->subWidgets.addDrawable(msgText);
            msgY += 22;
        }
    }
    parent->networkCanvasCurrentChatState->messagesMutex.unlock();
    chatMsgPanel->scrollToBottom();
}
