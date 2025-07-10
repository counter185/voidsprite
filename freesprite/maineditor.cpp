#include "maineditor.h"
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

#include "TilemapPreviewScreen.h"
#include "MinecraftSkinPreviewScreen.h"
#include "RPG2KTilemapPreviewScreen.h"
#include "SpritesheetPreviewScreen.h"
#include "NineSegmentPatternEditorScreen.h"
#include "MinecraftBlockPreviewScreen.h"

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

#if defined(__unix__)
#include <time.h>
#elif defined(_WIN32)
#include <time.h>
#else
#include <ctime>
#endif

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
    layers.push_back(new Layer(canvas.dimensions.x, canvas.dimensions.y));
    FillTexture();

    setUpWidgets();
    recenterCanvas();
    initLayers();
}
MainEditor::MainEditor(SDL_Surface* srf) {

    //todo i mean just use MainEditor(Layer*) here
    canvas.dimensions = { srf->w, srf->h };

    Layer* nlayer = new Layer(canvas.dimensions.x, canvas.dimensions.y);
    layers.push_back(nlayer);
    SDL_ConvertPixels(srf->w, srf->h, srf->format, srf->pixels, srf->pitch, SDL_PIXELFORMAT_ARGB8888, nlayer->pixels32(), canvas.dimensions.x*4);

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditor::MainEditor(Layer* layer)
{
    canvas.dimensions = { layer->w, layer->h };
    layers.push_back(layer);
    loadSingleLayerExtdata(layer);

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditor::MainEditor(std::vector<Layer*> layers)
{
    canvas.dimensions = { layers[0]->w, layers[0]->h };
    this->layers = layers;

    setUpWidgets();
    recenterCanvas();
    initLayers();
    
}

MainEditor::~MainEditor() {
    wxsManager.freeAllDrawables();
    discardUndoStack();
    discardRedoStack();
    for (Layer*& imgLayer : layers) {
        delete imgLayer;
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
    for (int x = 0; x < layers.size(); x++) {
        Layer* imgLayer = layers[x];
        if (!imgLayer->hidden) {
            uint8_t alpha = imgLayer->layerAlpha;
            imgLayer->render(canvasRenderRect, (layerSwitchTimer.started && x == selLayer) ? (uint8_t)(alpha * XM1PW3P1(layerSwitchTimer.percentElapsedTime(1300))) : alpha);
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

                    for (int x = 0; x < layers.size(); x++) {
                        Layer* imgLayer = layers[x];
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

    if (g_windowFocused) {
        u64 timestampNow = SDL_GetTicks64() / 1000;
        if (lastTimestamp != timestampNow) {
            lastTimestamp = timestampNow;
            editTime++;
        }
    }

    if (abs(g_gamepad->gamepadLSX) > 0.05f || abs(g_gamepad->gamepadLSY) > 0.05f) {
        canvas.panCanvas({
            (int)(g_gamepad->gamepadLSX * 10),
            (int)(g_gamepad->gamepadLSY * 10)
        });
        RecalcMousePixelTargetPoint(g_windowW / 2, g_windowH / 2);
        currentBrush->mouseMotion(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
    }

    canvas.lockToScreenBounds();

    //fuck it we ball
    if (layerPicker != NULL) {
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

    if (closeNextTick) {
        g_closeScreen(this);
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
                localTtp.addTooltip(Tooltip{ {ixmax(0,r.x), ixmax(30, r.y - 30)},
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

    g_fnt->RenderString(std::format("{}x{} ({}%)", canvas.dimensions.x, canvas.dimensions.y, canvas.scale * 100), 2, g_windowH - 28, SDL_Color{255,255,255,0xa0});

    XY endpoint = g_fnt->RenderString(std::format("{}:{}", mousePixelTargetPoint.x, mousePixelTargetPoint.y), 200, g_windowH - 28, SDL_Color{255,255,255,0xd0});
    if (tileDimensions.x != 0 && tileDimensions.y != 0) {
        std::string s = std::format("(t{}:{})", (int)floor(mousePixelTargetPoint.x / (float)tileDimensions.x), (int)floor(mousePixelTargetPoint.y / (float)tileDimensions.y));
        endpoint = g_fnt->RenderString(s, endpoint.x + 5, endpoint.y, SDL_Color{ 255,255,255,0x90 });
    }

    if (currentBrush != NULL) {
        static std::string eraserModeText = TL("vsp.maineditor.erasermode");
        g_fnt->RenderString(std::format("{} {}", currentBrush->getName(), eraserMode ? eraserModeText : ""), ixmax(endpoint.x + 10, 370), g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
    }

    if (currentPattern != NULL) {
        g_fnt->RenderString(std::format("{}", currentPattern->getName()), 620, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
    }

    g_fnt->RenderString(secondsTimeToHumanReadable(editTime), 2, g_windowH - 28 * 2, SDL_Color{ (u8)(255 - backgroundColor.r), (u8)(255 - backgroundColor.g), (u8)(255 - backgroundColor.b), (u8)(g_windowFocused ? 0x40 : 0x30) });

    if (changesSinceLastSave != NO_UNSAVED_CHANGES) {
        std::string unsavedSymbol = changesSinceLastSave == CHANGES_RECOVERY_AUTOSAVED ? UTF8_EMPTY_DIAMOND : UTF8_DIAMOND;
        int fw = g_fnt->StatStringDimensions(unsavedSymbol).x;
        g_fnt->RenderString(unsavedSymbol, g_windowW - fw - 2, g_windowH - 70, SDL_Color{ 255,255,255,0x70 });
    }
}

void MainEditor::renderComments()
{
    if (commentViewMode == COMMENTMODE_HIDE_ALL) {
        return;
    }
    TooltipsLayer localTtp;
    localTtp.border = false;
    localTtp.gradientUL = localTtp.gradientUR = 0xA0000000;
    localTtp.gradientLL = localTtp.gradientLR = 0x70000000;

    XY origin = canvas.currentDrawPoint;
    for (CommentData& c : comments) {
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

    int xOffset = undoTimer.started ? ((lastUndoWasRedo ? -1 : 1) * 5 * XM1PW3P1(undoTimer.percentElapsedTime(200))) : 0;
    center.x += xOffset;

    uint8_t lineShade = backgroundColor.r == 0x00 ? 0xff : 0x00;

    for (int x = 0; x < undoStack.size(); x++) {
        SDL_SetRenderDrawColor(g_rd, lineShade, lineShade, lineShade, 0x30);
        SDL_RenderDrawLine(g_rd, center.x - (x + 1) * 10, center.y, center.x - (x + 1) * 10, center.y - 10);
    }
    for (int x = 0; x < redoStack.size(); x++) {
        SDL_SetRenderDrawColor(g_rd, lineShade, lineShade, lineShade, 0x30);
        SDL_RenderDrawLine(g_rd, center.x + (x + 1) * 10, center.y, center.x + (x + 1) * 10, center.y - 10);
    }
    SDL_SetRenderDrawColor(g_rd, lineShade, lineShade, lineShade, 0x50);
    SDL_RenderDrawLine(g_rd, center.x, center.y + 2, center.x, center.y - (15 * XM1PW3P1(undoTimer.started ? undoTimer.percentElapsedTime(200) : 1.0)));
}

void MainEditor::initLayers()
{
    for (Layer*& l : layers) {
        l->commitStateToUndoStack();
    }

    layerPicker->updateLayers();
}

void MainEditor::setUpWidgets()
{
    mainEditorKeyActions = {
        {
            SDL_SCANCODE_F,
            {
                TL("vsp.nav.file"),
                {SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, SDL_SCANCODE_E, SDL_SCANCODE_A, SDL_SCANCODE_R, SDL_SCANCODE_C, SDL_SCANCODE_P, SDL_SCANCODE_X},
                {
                    {SDL_SCANCODE_D, { TL("vsp.maineditor.saveas"),
                            [this]() {
                                this->trySaveAsImage();
                            }
                        }
                    },
                    {SDL_SCANCODE_S, { TL("vsp.nav.save"),
                            [this]() {
                                this->trySaveImage();
                            }
                        }
                    },
                    {SDL_SCANCODE_F, { TL("vsp.maineditor.nav.exportscaled"),
                            [this]() {
                                PopupExportScaled* popup = new PopupExportScaled(this);
                                popup->setCallbackListener(EVENT_MAINEDITOR_EXPORTSCALED, this);
                                g_addPopup(popup);
                            }
                        }
                    },
                    {SDL_SCANCODE_E, { TL("vsp.maineditor.exportpal"),
                            [this]() {
                                this->tryExportPalettizedImage();
                            }
                        }
                    },
                    {SDL_SCANCODE_A, { TL("vsp.maineditor.exportind"),
                            [this]() {
                                this->exportTilesIndividually();
                            }
                        }
                    },
                    {SDL_SCANCODE_R, { TL("vsp.maineditor.paledit"),
                            [this]() {
                                MainEditorPalettized* newEditor = this->toPalettizedSession();
                                if (newEditor != NULL) {
                                    g_addScreen(newEditor);
                                }
                            }
                        }
                    },
                    {SDL_SCANCODE_C, { TL("vsp.maineditor.copyflattoclipboard"),
                            [this]() {
                                this->copyImageToClipboard();
                            }
                        }
                    },
                    {SDL_SCANCODE_X, { TL("vsp.cmn.close"),
                            [this]() {
                                this->requestSafeClose();
                            }
                        }
                    },
                    {SDL_SCANCODE_P, { TL("vsp.maineditor.preference"),
                            [this]() {
                                g_addPopup(new PopupGlobalConfig());
                            }
                        }
                    }
                },
                g_iconNavbarTabFile
            }
        },
        {
            SDL_SCANCODE_E,
            {
                TL("vsp.maineditor.edit"),
                {SDL_SCANCODE_Z, SDL_SCANCODE_R, SDL_SCANCODE_X, SDL_SCANCODE_Y, SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_S, SDL_SCANCODE_C, SDL_SCANCODE_V, SDL_SCANCODE_B, SDL_SCANCODE_N, SDL_SCANCODE_M},
                {
                    {SDL_SCANCODE_Z, { TL("vsp.maineditor.undo"),
                            [this]() {
                                this->undo();
                            }
                        }
                    },
                    {SDL_SCANCODE_R, { TL("vsp.maineditor.redo"),
                            [this]() {
                                this->redo();
                            }
                        }
                    },
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
                    {SDL_SCANCODE_F, { TL("vsp.maineditor.flipallx"),
                            [this]() {
                                this->flipAllLayersOnX();
                            }
                        }
                    },
                    {SDL_SCANCODE_G, { TL("vsp.maineditor.flipally"),
                            [this]() {
                                this->flipAllLayersOnY();
                            }
                        }
                    },
                    {SDL_SCANCODE_C, { TL("vsp.maineditor.rescanv"),
                            [this]() {
                                g_addPopup(new PopupTileGeneric(this, TL("vsp.maineditor.rescanv"), "New canvas size:", this->canvas.dimensions, EVENT_MAINEDITOR_RESIZELAYER));
                            }
                        }
                    },
                    {SDL_SCANCODE_S, { TL("vsp.maineditor.dsel"),
                            [this]() {
                                this->isolateEnabled = false;
                            }
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
                                g_addPopup(new PopupIntegerScale(this, TL("vsp.maineditor.intscale"), "Scale:", XY{ 1,1 }, EVENT_MAINEDITOR_INTEGERSCALE));
                            }
                        }
                    },
                    {SDL_SCANCODE_M, { TL("vsp.maineditor.canvscale"),
                            [this]() {
                                g_addPopup(new PopupTileGeneric(this, TL("vsp.maineditor.canvscale"), "New size:", this->canvas.dimensions, EVENT_MAINEDITOR_RESCALELAYER));
                            }
                        }
                    },
                    {SDL_SCANCODE_P, { TL("vsp.maineditor.nineseg"),
                            [this]() {
                                g_addScreen(new NineSegmentPatternEditorScreen(this));
                            }
                        }
                    },
                },
                g_iconNavbarTabEdit
            }
        },
        {
            SDL_SCANCODE_L,
            {
                TL("vsp.maineditor.layer"),
                {},
                {
                    {SDL_SCANCODE_F, { TL("vsp.maineditor.flipx"),
                            [this]() {
                                this->layer_flipHorizontally();
                            }
                        }
                    },
                    {SDL_SCANCODE_G, { TL("vsp.maineditor.flipy"),
                            [this]() {
                                this->layer_flipVertically();
                            }
                        }
                    },
                    {SDL_SCANCODE_X, { TL("vsp.maineditor.printcol"),
                            [this]() {
                                g_addNotification(Notification("", std::format("{} colors in current layer", this->getCurrentLayer()->numUniqueColors(true))));
                            }
                        }
                    },
                    {SDL_SCANCODE_R, { TL("vsp.maineditor.renlayer"),
                            [this]() {
                                this->layer_promptRename();
                            }
                        }
                    },
                    {SDL_SCANCODE_S, { TL("vsp.maineditor.isolatealpha"),
                            [this]() {
                                this->layer_selectCurrentAlpha();
                            }
                        }
                    },
                    {SDL_SCANCODE_A, { TL("vsp.maineditor.removealpha"),
                            [this]() {
                                this->layer_setAllAlpha255();
                            }
                        }
                    },
                    {SDL_SCANCODE_K, { TL("vsp.maineditor.setckey"),
                            [this]() {
                                PopupPickColor* newPopup = new PopupPickColor(TL("vsp.maineditor.setckey"), TL("vsp.maineditor.setckeydesc"));
                                newPopup->setCallbackListener(EVENT_MAINEDITOR_SETCOLORKEY, this);
                                g_addPopup(newPopup);
                            }
                        }
                    },
                    {SDL_SCANCODE_C, { TL("vsp.maineditor.nav.layer.copylayertoclipboard"),
                            [this]() {
                                this->copyLayerToClipboard(this->getCurrentLayer());
                            }
                        }
                    },
                    {SDL_SCANCODE_E, { TL("vsp.maineditor.nav.layer.clearselection"),
                            [this]() {
                                this->layer_clearSelectedArea();
                            }
                        }
                    },
                    {SDL_SCANCODE_W, { TL("vsp.maineditor.nav.layer.fillselection"),
                            [this]() {
                                this->layer_fillActiveColor();
                            }
                        }
                    },
                },
                g_iconNavbarTabLayer
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
                TL("vsp.maineditor.tab.view"),
                {},
                {
                    {SDL_SCANCODE_R, { "Recenter canvas",
                            [this]() { this->recenterCanvas(); }
                        }
                    },
                    {SDL_SCANCODE_F, { "Add reference...",
                            [this]() {
                                PopupFilePicker::PlatformAnyImageImportDialog(this, TL("vsp.popup.addreference"), EVENT_MAINEDITOR_ADD_REFERENCE);
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
                                g_addNotification(Notification(std::format("{}",
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
                    {SDL_SCANCODE_S, { "Open spritesheet preview...",
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
                    {SDL_SCANCODE_T, { "Open tileset preview...",
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
                    {SDL_SCANCODE_Y, { "Open RPG Maker 2K/2K3 ChipSet preview...",
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
                    {SDL_SCANCODE_N, { "Open cube preview...",
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
                    {SDL_SCANCODE_P, { "Open touch mode panel...",
                            [this]() {
                                if (this->touchModePanel == NULL) {
                                    this->touchModePanel = new EditorTouchToggle(this);
                                    this->touchModePanel->position = { g_windowW - this->touchModePanel->wxWidth - 10, g_windowH - this->touchModePanel->wxHeight - 40 };
                                    this->addWidget(this->touchModePanel);
                                }
                            }
                        }
                    },
#if _DEBUG
                    {SDL_SCANCODE_M, { "Open Minecraft skin preview...",
                            [this]() {
                                if (this->canvas.dimensions.x != this->canvas.dimensions.y && this->canvas.dimensions.x / 2 != this->canvas.dimensions.y) {
                                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid size. Aspect must be 1:1 or 2:1."));
                                    return;
                                }
                                MinecraftSkinPreviewScreen* newScreen = new MinecraftSkinPreviewScreen(this);
                                g_addScreen(newScreen);
                            }
                        }
                    },
#endif
                },
                g_iconNavbarTabView
            }
        }
    };

    SDL_Scancode keyorder[] = { SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R, SDL_SCANCODE_T, SDL_SCANCODE_Y, SDL_SCANCODE_U, SDL_SCANCODE_I, SDL_SCANCODE_O, SDL_SCANCODE_P,
                               SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_H, SDL_SCANCODE_J, SDL_SCANCODE_K, SDL_SCANCODE_L,
                               SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V, SDL_SCANCODE_B, SDL_SCANCODE_N, SDL_SCANCODE_M };

    //load filters
    int i = 0;
    for (auto& filter : g_filters) {
        mainEditorKeyActions[SDL_SCANCODE_Q].actions[keyorder[i++]] = {
            filter->name(), [this, filter]() {
                PopupApplyFilter* newPopup = new PopupApplyFilter(this, this->getCurrentLayer(), filter);
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

    colorPicker = new EditorColorPicker(this);
    CollapsableDraggablePanel* colorPickerPanel = new CollapsableDraggablePanel(TL("vsp.maineditor.panel.colorpicker.title"), colorPicker);
    colorPickerPanel->position.y = 63;
    colorPickerPanel->position.x = 10;
    wxsManager.addDrawable(colorPickerPanel);
    colorPicker->colorUpdated(pickedColor);
    regenerateLastColors();

    brushPicker = new EditorBrushPicker(this);
    CollapsableDraggablePanel* brushPickerPanel = new CollapsableDraggablePanel(TL("vsp.maineditor.panel.brushpicker.title"), brushPicker);
    brushPickerPanel->position.y = 454;
    brushPickerPanel->position.x = 10;
    wxsManager.addDrawable(brushPickerPanel);

    layerPicker = new EditorLayerPicker(this);
    layerPicker->position = XY{ 440, 80 };
    layerPicker->anchor = XY{ 1,0 };
    wxsManager.addDrawable(layerPicker);

    navbar = new ScreenWideNavBar(this, mainEditorKeyActions, { SDL_SCANCODE_F, SDL_SCANCODE_E, SDL_SCANCODE_L, SDL_SCANCODE_Q, SDL_SCANCODE_R, SDL_SCANCODE_V });
    wxsManager.addDrawable(navbar);

    makeActionBar();

    //this must happen after actionbar init
    setActiveBrush(g_brushes[0]);
    currentPattern = g_patterns[0];
}

void MainEditor::makeActionBar()
{
    //action bar
    ScreenWideActionBar* actionbar = new ScreenWideActionBar({});
    actionbar->position = { 0, navbar->wxHeight };

    int nextNavbarX = 5;
    UIButton* undoButton = new UIButton("", TL("vsp.maineditor.undo"));
    undoButton->icon = g_iconActionBarUndo;
    undoButton->onClickCallback = [this](UIButton* btn) { undo(); };
    undoButton->position = { nextNavbarX,0 };
    undoButton->wxWidth = 30;
    undoButton->wxHeight = 30;
    actionbar->addDrawable(undoButton);
    nextNavbarX += 35;

    UIButton* redoButton = new UIButton("", TL("vsp.maineditor.redo"));
    redoButton->icon = g_iconActionBarRedo;
    redoButton->onClickCallback = [this](UIButton* btn) { redo(); };
    redoButton->position = { nextNavbarX,0 };
    redoButton->wxWidth = 30;
    redoButton->wxHeight = 30;
    actionbar->addDrawable(redoButton);
    nextNavbarX += 35;

    UIButton* saveButton = new UIButton("", TL("vsp.nav.save"));
    saveButton->icon = g_iconActionBarSave;
    saveButton->onClickCallback = [this](UIButton* btn) { if (g_shiftModifier) trySaveAsImage(); else trySaveImage(); };
    saveButton->position = { nextNavbarX,0 };
    saveButton->wxWidth = 30;
    saveButton->wxHeight = 30;
    actionbar->addDrawable(saveButton);
    nextNavbarX += 35;

    UIButton* zoomoutButton = new UIButton("", TL("vsp.cmn.zoomout"));
    zoomoutButton->icon = g_iconActionBarZoomOut;
    zoomoutButton->onClickCallback = [this](UIButton* btn) { canvas.zoom(-1, { g_windowW / 2, g_windowH / 2 }); };
    zoomoutButton->position = { nextNavbarX,0 };
    zoomoutButton->wxWidth = 30;
    zoomoutButton->wxHeight = 30;
    actionbar->addDrawable(zoomoutButton);
    nextNavbarX += 35;

    UIButton* zoominButton = new UIButton("", TL("vsp.cmn.zoomin"));
    zoominButton->icon = g_iconActionBarZoomIn;
    zoominButton->onClickCallback = [this](UIButton* btn) { canvas.zoom(1, {g_windowW/2, g_windowH/2}); };
    zoominButton->position = { nextNavbarX,0 };
    zoominButton->wxWidth = 30;
    zoominButton->wxHeight = 30;
    actionbar->addDrawable(zoominButton);
    nextNavbarX += 35;

    toolPropertiesPanel = new Panel();
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
}

void MainEditor::addWidget(Drawable* wx)
{
    wxsManager.addDrawable(wx);
}

void MainEditor::removeWidget(Drawable* wx)
{
    wxsManager.removeDrawable(wx);
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
        closeNextTick = true;
        return true;
    }
    else {
        PopupYesNo* newPopup = new PopupYesNo("Close the project?", "You have unsaved changes! ");
        newPopup->setCallbackListener(EVENT_MAINEDITOR_CONFIRM_CLOSE, this);
        g_addPopup(newPopup);
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
        tryAddReference(convertStringOnWin32(path));
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
                                        currentBrushMouseDowned = false;
                                    }

                                }
                            }
                            mouseHoldPosition = mousePixelTargetPoint;
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
                                    currentBrush->clickDrag(this, mouseHoldPosition, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                                }
                                mouseHoldPosition = mousePixelTargetPoint;
                            }
                            if (currentBrush != NULL) {
                                currentBrush->mouseMotion(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                            }
                        }
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    if (g_ctrlModifier && !g_config.scrollWithTouchpad) {
                        colorPicker->colorUpdatedHSV(colorPicker->currentH, fxmin(fxmax(colorPicker->currentS + 0.1 * evt.wheel.y, 0), 1), colorPicker->currentV);
                    }
                    else if (g_shiftModifier && !g_config.scrollWithTouchpad) {
                        double newH = dxmin(dxmax(colorPicker->currentH + (360.0 / 12) * evt.wheel.y, 0), 359);
                        colorPicker->colorUpdatedHSV(newH, colorPicker->currentS, colorPicker->currentV);
                    }
                    else {
                        if (g_config.scrollWithTouchpad && !g_ctrlModifier) {
                            canvas.panCanvas({
                                (int)(-(g_shiftModifier ? evt.wheel.y : evt.wheel.x) * 20),
                                (int)((g_shiftModifier ? -evt.wheel.x : evt.wheel.y) * 20)
                            });
                        }
                        else {
                            zoom(evt.wheel.y);
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
                    //loginfo(std::format("new pen state: {}", penDown));
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
                    PlatformNativePathString tileName = name + convertStringOnWin32(std::format("_{}_{}{}", x, y, exporter->extension()));
                    if (!exporter->exportsWholeSession()) {
                        exporter->exportData(tileName, clip);
                        delete clip;
                    }
                    else {
                        MainEditor* session = new MainEditor(clip);
                        session->trySaveWithExporter(tileName, exporter);
                        delete session;
                    }
                }
                else {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), std::format("Failed to export tile {}:{}", x,y)));
                }
            }
        }
        delete flatImage;
        g_addNotification(SuccessNotification("Success", "Tiles exported"));
    }
}

void MainEditor::eventPopupClosed(int evt_id, BasePopup* p)
{
    if (evt_id == EVENT_MAINEDITOR_CONFIRM_CLOSE) {
        if (((PopupYesNo*)p)->result) {
            g_closeScreen(this);
        }
    }
    else if (evt_id == EVENT_MAINEDITOR_RESIZELAYER) {
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
}

void MainEditor::eventTextInputConfirm(int evt_id, std::string text)
{
    if (evt_id == EVENT_MAINEDITOR_SET_CURRENT_LAYER_NAME) {
        getCurrentLayer()->name = text;
        layerPicker->updateLayers();
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
            uint32_t targetColor = color;
            if (blendAlphaMode) {
                if (eraserMode) {
                    targetColor = ((0xff - (targetColor >> 24)) << 24) + (targetColor & 0xffffff);
                }
                targetColor = alphaBlend(getCurrentLayer()->getPixelAt(position), targetColor);
            }
            getCurrentLayer()->setPixel(position, targetColor & (eraserMode ? 0xffffff : 0xffffffff));
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
    if (platformPutImageInClipboard(l)) {
        g_addNotification(SuccessNotification(TL("vsp.cmn.copiedtoclipboard"), ""));
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.clipboardcopy")));
    }
}

void MainEditor::trySaveImage()
{
    bool result = false;
    if (splitSessionData.set) {
        if (saveSplitSession(lastConfirmedSavePath, this)) {
            result = true;
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
        changesSinceLastSave = NO_UNSAVED_CHANGES;
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
    ret["CommentData"] = makeCommentDataString();
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
        if (kvmap.contains("CommentData")) { comments = parseCommentDataString(kvmap["CommentData"]); }
        if (kvmap.contains("UsingAltBG")) { setAltBG(kvmap["UsingAltBG"] == "1"); }
    }
    catch (std::exception e) {}
}

std::string MainEditor::makeCommentDataString()
{
    std::string commentsData = "";
    commentsData += std::to_string(comments.size()) + ';';
    for (CommentData& c : comments) {
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
    if (undoStack.size() > 0) {
        UndoStackElement l = undoStack[0];
        switch (l.type) {
            case UNDOSTACK_LAYER_DATA_MODIFIED:
                l.targetlayer->discardLastUndo();
                break;
            case UNDOSTACK_DELETE_LAYER:
                delete l.targetlayer;
                break;
            case UNDOSTACK_RESIZE_LAYER:
                {
                    UndoStackResizeLayerElement* resizeLayerData = (UndoStackResizeLayerElement*)l.extdata4;
                    for (int x = 0; x < layers.size(); x++) {
                        for (auto& variant : resizeLayerData[x].oldLayerData) {
                            tracked_free(variant.pixelData);
                        }
                    }
                    delete resizeLayerData;
                }
                break;
            case UNDOSTACK_ALL_LAYER_DATA_MODIFIED:
                for (Layer* ll : layers) {
                    ll->discardLastUndo();
                }
                break;
        }

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
    l->commitStateToUndoStack();
    addToUndoStack(UndoStackElement{ l, UNDOSTACK_LAYER_DATA_MODIFIED });
}

void MainEditor::commitStateToCurrentLayer()
{
    commitStateToLayer(getCurrentLayer());
}

uint32_t MainEditor::pickColorFromAllLayers(XY pos)
{
    uint32_t c = 0;
    for (int x = layers.size() - 1; x >= 0; x--) {
        if (layers[x]->hidden) {
            continue;
        }
        uint32_t nextC = layers[x]->getPixelAt(pos, false);
        if ((c & 0xff000000) == 0 && (nextC & 0xff000000) == (0xff<<24)) {
            return nextC;
        }
        else {
            c = alphaBlend(nextC, c);
        }
    }
    return c;
}

void MainEditor::addToUndoStack(UndoStackElement undo)
{
    discardRedoStack();
    undoStack.push_back(undo);
    checkAndDiscardEndOfUndoStack();
    changesSinceLastSave = HAS_UNSAVED_CHANGES;
}

void MainEditor::discardUndoStack()
{
    while (!undoStack.empty()) {
        discardEndOfUndoStack();
    }
}

void MainEditor::discardRedoStack()
{
    for (Layer*& x : layers) {
        x->discardRedoStack();
    }

    //clear redo stack
    for (UndoStackElement& l : redoStack) {
        switch (l.type) {
            case UNDOSTACK_CREATE_LAYER:
                delete l.targetlayer;
                break;
            case UNDOSTACK_RESIZE_LAYER:
                UndoStackResizeLayerElement* resizeLayerData = (UndoStackResizeLayerElement*)l.extdata4;
                for (int x = 0; x < layers.size(); x++) {
                    for (auto& variant : resizeLayerData[x].oldLayerData) {
                        tracked_free(variant.pixelData);
                    }
                }
                delete resizeLayerData;
                break;
        }
    }
    redoStack.clear();
}

void MainEditor::undo()
{
    if (!undoStack.empty()) {
        undoTimer.start();
        lastUndoWasRedo = false;
        UndoStackElement l = undoStack[undoStack.size() - 1];
        undoStack.pop_back();
        switch (l.type) {
            case UNDOSTACK_LAYER_DATA_MODIFIED:
                l.targetlayer->undo();
                break;
            case UNDOSTACK_CREATE_LAYER:
            {
                //remove layer from list
                auto pos = std::find(layers.begin(), layers.end(), l.targetlayer);
                if (pos != layers.end()) {
                    layers.erase(pos);
                }
                if (selLayer >= layers.size()) {
                    switchActiveLayer(layers.size() - 1);
                }
                layerPicker->updateLayers();
            }
                break;
            case UNDOSTACK_DELETE_LAYER:
                //add layer to list
                layers.insert(layers.begin() + l.extdata, l.targetlayer);
                layerPicker->updateLayers();
                break;
            case UNDOSTACK_MOVE_LAYER:
            {
                Layer* lr = layers[l.extdata2];
                layers.erase(layers.begin() + l.extdata2);
                layers.insert(layers.begin() + l.extdata, lr);
                layerPicker->updateLayers();
            }
                break;
            case UNDOSTACK_ADD_COMMENT:
                _removeCommentAt({ l.extdata, l.extdata2 });
                break;
            case UNDOSTACK_REMOVE_COMMENT:
                comments.push_back(CommentData{ {l.extdata, l.extdata2}, l.extdata3 });
                break;
            case UNDOSTACK_SET_OPACITY:
                l.targetlayer->layerAlpha = (uint8_t)l.extdata;
                l.targetlayer->lastConfirmedlayerAlpha = l.targetlayer->layerAlpha;
                layerPicker->updateLayers();
                break;
            case UNDOSTACK_RESIZE_LAYER:
            {
                UndoStackResizeLayerElement* resizeLayerData = (UndoStackResizeLayerElement*)l.extdata4;
                for (int x = 0; x < layers.size(); x++) {
                    //memcpy(layers[x]->pixelData, resizeLayerData[x].oldData, resizeLayerData[x].oldW * resizeLayerData[x].oldH * 4);
                    std::vector<LayerVariant> oldData = layers[x]->layerData;
                    XY oldDimensions = XY{ layers[x]->w, layers[x]->h };
                    layers[x]->layerData = resizeLayerData[x].oldLayerData;
                    layers[x]->w = resizeLayerData[x].oldDimensions.x;
                    layers[x]->h = resizeLayerData[x].oldDimensions.y;
                    layers[x]->markLayerDirty();
                    resizeLayerData[x].oldLayerData = oldData;
                    resizeLayerData[x].oldDimensions = oldDimensions;
                }
                canvas.dimensions = { layers[0]->w, layers[0]->h };
                XY td = XY{ l.extdata, l.extdata2 };
                l.extdata = tileDimensions.x;
                l.extdata2 = tileDimensions.y;
                tileDimensions = td;
            }
                break;
            case UNDOSTACK_ALL_LAYER_DATA_MODIFIED:
                for (Layer* ll : layers) {
                    ll->undo();
                }
                break;
        }
        changesSinceLastSave = HAS_UNSAVED_CHANGES;
        redoStack.push_back(l);
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
        UndoStackElement l = redoStack[redoStack.size() - 1];
        redoStack.pop_back();
        switch (l.type) {
        case UNDOSTACK_LAYER_DATA_MODIFIED:
            l.targetlayer->redo();
            break;
        case UNDOSTACK_CREATE_LAYER:
            //add layer back to list
            layers.insert(layers.begin() + l.extdata, l.targetlayer);
            layerPicker->updateLayers();
            break;
        case UNDOSTACK_DELETE_LAYER:
            //add layer to list
            for (int x = 0; x < layers.size(); x++) {
                if (layers[x] == l.targetlayer) {
                    layers.erase(layers.begin() + x);
                    break;
                }
            }
            if (selLayer >= layers.size()) {
                switchActiveLayer(layers.size() - 1);
            }
            layerPicker->updateLayers();
            break;
        case UNDOSTACK_MOVE_LAYER:
        {
            Layer* lr = layers[l.extdata];
            layers.erase(layers.begin() + l.extdata);
            layers.insert(layers.begin() + l.extdata2, lr);
            layerPicker->updateLayers();
        }
            break;
        case UNDOSTACK_ADD_COMMENT:
            comments.push_back(CommentData{ {l.extdata, l.extdata2}, l.extdata3 });
            break;
        case UNDOSTACK_REMOVE_COMMENT:
            _removeCommentAt({ l.extdata, l.extdata2 });
            break;
        case UNDOSTACK_SET_OPACITY:
            l.targetlayer->layerAlpha = (uint8_t)l.extdata2;
            l.targetlayer->lastConfirmedlayerAlpha = l.targetlayer->layerAlpha;
            layerPicker->updateLayers();
            break;
        case UNDOSTACK_RESIZE_LAYER:
        {
            UndoStackResizeLayerElement* resizeLayerData = (UndoStackResizeLayerElement*)l.extdata4;
            for (int x = 0; x < layers.size(); x++) {
                //memcpy(layers[x]->pixelData, resizeLayerData[x].oldData, resizeLayerData[x].oldW * resizeLayerData[x].oldH * 4);
                std::vector<LayerVariant> oldData = layers[x]->layerData;
                XY oldDimensions = XY{ layers[x]->w, layers[x]->h };
                layers[x]->layerData = resizeLayerData[x].oldLayerData;
                layers[x]->w = resizeLayerData[x].oldDimensions.x;
                layers[x]->h = resizeLayerData[x].oldDimensions.y;
                layers[x]->markLayerDirty();
                resizeLayerData[x].oldLayerData = oldData;
                resizeLayerData[x].oldDimensions = oldDimensions;
            }
            canvas.dimensions = { layers[0]->w, layers[0]->h };
            XY td = XY{ l.extdata, l.extdata2 };
            l.extdata = tileDimensions.x;
            l.extdata2 = tileDimensions.y;
            tileDimensions = td;
        }
            break;
        case UNDOSTACK_ALL_LAYER_DATA_MODIFIED:
            for (Layer* ll : layers) {
                ll->redo();
            }
            break;
        }
        changesSinceLastSave = HAS_UNSAVED_CHANGES;
        undoStack.push_back(l);
    }
    else {
        g_addNotification(ErrorNotification("Nothing to redo", ""));
    }
}

Layer* MainEditor::newLayer()
{
    Layer* nl = Layer::tryAllocLayer(canvas.dimensions.x, canvas.dimensions.y);
    if (nl != NULL) {
        nl->name = std::format("New Layer {}", layers.size() + 1);
        int insertAtIdx = std::find(layers.begin(), layers.end(), getCurrentLayer()) - layers.begin() + 1;
        logprintf("adding new layer at %i\n", insertAtIdx);
        layers.insert(layers.begin() + insertAtIdx, nl);
        switchActiveLayer(insertAtIdx);

        addToUndoStack(UndoStackElement{ nl, UNDOSTACK_CREATE_LAYER, insertAtIdx });
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.mallocfail")));
    }
    return nl;
}

void MainEditor::deleteLayer(int index) {
    if (layers.size() <= 1) {
        g_addNotification(ErrorNotification("Can't delete the last layer", ""));
        return;
    }

    Layer* layerAtPos = layers[index];
    layers.erase(layers.begin() + index);
    if (selLayer >= layers.size()) {
        switchActiveLayer(layers.size() - 1);
    }

    addToUndoStack(UndoStackElement{ layerAtPos, UNDOSTACK_DELETE_LAYER, index });
}

void MainEditor::regenerateLastColors()
{
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
    colorPicker->colorUpdated(col);
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
            time_t now = time(NULL);
            tm ltm;
            // todo: make a platform-specific localtime function
            #if defined(__unix__) || defined(__APPLE__)
                localtime_r(&now, &ltm);
            #elif defined(_WIN32)
                localtime_s(&ltm, &now);
            #else
                ltm = *std::localtime(&now);
            #endif
            std::string autosaveName = "autosave_" + std::format("{}-{}-{}--{}-{}-{}", ltm.tm_year + 1900, ltm.tm_mon+1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec) + ".voidsn";
            try {
                if (voidsnExporter->exportData(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/autosaves/" + autosaveName), this)) {
                    g_addNotification(SuccessNotification("Recovery autosave", "Autosave successful"));
                    changesSinceLastSave = CHANGES_RECOVERY_AUTOSAVED;
                }
                else {
                    g_addNotification(ErrorNotification("Recovery autosave", "Autosave failed"));
                }
            }
            catch (std::exception e) {
                g_addNotification(ErrorNotification("Recovery autosave", "Exception during autosave"));
            }
        }
    }
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

void MainEditor::tryAddReference(PlatformNativePathString path)
{
    MainEditor* ssn = loadAnyIntoSession(convertStringToUTF8OnWin32(path));
    if (ssn != NULL) {
        Layer* flat = ssn->flattenImage();
        Panel* referencePanel = new PanelReference(flat);
        CollapsableDraggablePanel* refWPanel = new CollapsableDraggablePanel("REFERENCE", referencePanel);
        addWidget(refWPanel);
        refWPanel->playPanelOpenVFX();
        delete ssn;
    }
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

void MainEditor::moveLayerUp(int index) {
    if (index >= layers.size()-1) {
        return;
    }

    Layer* clayer = layers[index];
    layers.erase(layers.begin() + index);
    layers.insert(layers.begin() + index + 1, clayer);

    if (index == selLayer) {
        switchActiveLayer(selLayer + 1);
    }

    addToUndoStack(UndoStackElement{ clayer, UNDOSTACK_MOVE_LAYER, index, index + 1 });
}

void MainEditor::moveLayerDown(int index) {
    if (index  <= 0) {
        return;
    }

    Layer* clayer = layers[index];
    layers.erase(layers.begin() + index);
    layers.insert(layers.begin() + index - 1, clayer);

    if (index == selLayer) {
        switchActiveLayer(selLayer-1);
    }

    addToUndoStack(UndoStackElement{ clayer, UNDOSTACK_MOVE_LAYER, index, index - 1 });
}

void MainEditor::mergeLayerDown(int index)
{
    if (index == 0) {
        return;
    }
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

void MainEditor::layer_setOpacity(uint8_t opacity) {
    Layer* clayer = getCurrentLayer();
    addToUndoStack(UndoStackElement{ clayer, UNDOSTACK_SET_OPACITY, clayer->lastConfirmedlayerAlpha, opacity });
    //logprintf("added to undo stack: %i, %i\n", clayer->lastConfirmedlayerAlpha, opacity);
    clayer->layerAlpha = opacity;
    clayer->lastConfirmedlayerAlpha = clayer->layerAlpha;
}

void MainEditor::layer_promptRename()
{
    PopupTextBox* ninput = new PopupTextBox("Rename layer", "Enter the new layer name:", this->getCurrentLayer()->name);
    ninput->setCallbackListener(EVENT_MAINEDITOR_SET_CURRENT_LAYER_NAME, this);
    g_addPopup(ninput);
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
    Layer* ret = Layer::tryAllocLayer(canvas.dimensions.x, canvas.dimensions.y);
    if (ret != NULL) {
        int x = 0;
        uint32_t* retppx = ret->pixels32();
        for (Layer*& l : layers) {
            if (l->hidden) {
                continue;
            }
            uint32_t* ppx = l->pixels32();
            if (x++ == 0) {
                if (l->layerAlpha == 255) {
                    memcpy(ret->pixels32(), l->pixels32(), l->w * l->h * 4);
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
    Layer* ret = new Layer(bottom->w, bottom->h);

    memcpy(ret->pixels32(), bottom->pixels32(), bottom->w * bottom->h * 4);

    uint32_t* ppx = top->pixels32();
    uint32_t* retppx = ret->pixels32();
    for (uint64_t p = 0; p < ret->w * ret->h; p++) {
        uint32_t pixel = ppx[p];
        uint32_t srcPixel = retppx[p];
        pixel = modAlpha(pixel, (uint8_t)(((pixel>>24)/255.0f) * (top->layerAlpha / 255.0f) * 255));
        retppx[p] = alphaBlend(srcPixel, pixel);
    }

    return ret;
}

void MainEditor::flipAllLayersOnX()
{
    addToUndoStack(UndoStackElement{ NULL, UNDOSTACK_ALL_LAYER_DATA_MODIFIED });
    for (Layer* l : layers) {
        l->commitStateToUndoStack();
        l->flipHorizontally();
    }
}

void MainEditor::flipAllLayersOnY()
{
    addToUndoStack(UndoStackElement{ NULL, UNDOSTACK_ALL_LAYER_DATA_MODIFIED });
    for (Layer* l : layers) {
        l->commitStateToUndoStack();
        l->flipVertically();
    }
}

void MainEditor::rescaleAllLayersFromCommand(XY size) {
    if (xyEqual(canvas.dimensions, size)) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Size must be different to rescale."));
        return;
    }

    //todo: detect if copyscaled or malloc fails
    UndoStackResizeLayerElement* layerResizeData = new UndoStackResizeLayerElement[layers.size()];
    for (int x = 0; x < layers.size(); x++) {
        layerResizeData[x].oldDimensions = XY{layers[x]->w, layers[x]->h};
        layerResizeData[x].oldLayerData = layers[x]->layerData;
        Layer* sc = layers[x]->copyScaled(size);
        layers[x]->pixelData = (u8*)tracked_malloc(size.x * size.y * 4);
        memcpy(layers[x]->pixelData, sc->pixelData, size.x * size.y * 4);
        delete sc;
        layers[x]->w = size.x;
        layers[x]->h = size.y;
        layers[x]->markLayerDirty();
    }
    canvas.dimensions = {layers[0]->w, layers[0]->h};

    UndoStackElement undoData{};
    undoData.type = UNDOSTACK_RESIZE_LAYER;
    undoData.extdata = tileDimensions.x;
    undoData.extdata2 = tileDimensions.y;
    undoData.extdata4 = layerResizeData;
    addToUndoStack(undoData);
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

    UndoStackResizeLayerElement* layerResizeData = new UndoStackResizeLayerElement[layers.size()];
    for (int x = 0; x < layers.size(); x++) {
        layerResizeData[x].oldDimensions = XY{ layers[x]->w, layers[x]->h };
        if (byTile) {
            layerResizeData[x].oldLayerData = layers[x]->resizeByTileSizes(tileDimensions, size);
        }
        else {
            layerResizeData[x].oldLayerData = layers[x]->resize(size);
        }
        layers[x]->markLayerDirty();
    }
    canvas.dimensions = { layers[0]->w, layers[0]->h };
    
    UndoStackElement undoData{};
    undoData.type = UNDOSTACK_RESIZE_LAYER;
    undoData.extdata = tileDimensions.x;
    undoData.extdata2 = tileDimensions.y;
    undoData.extdata4 = layerResizeData;
    addToUndoStack(undoData);

    if (byTile) {
        tileDimensions = size;
    }
}

void MainEditor::resizzeAllLayersByTilecountFromCommand(XY size)
{
    UndoStackResizeLayerElement* layerResizeData = new UndoStackResizeLayerElement[layers.size()];
    for (int x = 0; x < layers.size(); x++) {
        layerResizeData[x].oldDimensions = XY{ layers[x]->w, layers[x]->h };
        layerResizeData[x].oldLayerData = layers[x]->resizeByTileCount(tileDimensions, size);
        layers[x]->markLayerDirty();
    }
    canvas.dimensions = { layers[0]->w, layers[0]->h };

    UndoStackElement undoData{};
    undoData.type = UNDOSTACK_RESIZE_LAYER;
    undoData.extdata = tileDimensions.x;
    undoData.extdata2 = tileDimensions.y;
    undoData.extdata4 = layerResizeData;
    addToUndoStack(undoData);
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

    UndoStackResizeLayerElement* layerResizeData = new UndoStackResizeLayerElement[layers.size()];
    for (int x = 0; x < layers.size(); x++) {
        layerResizeData[x].oldDimensions = XY{ layers[x]->w, layers[x]->h };
        layerResizeData[x].oldLayerData = downscale ? layers[x]->integerDownscale(scale) : layers[x]->integerScale(scale);
        layers[x]->markLayerDirty();
    }
    canvas.dimensions = { layers[0]->w, layers[0]->h };

    UndoStackElement undoData{};
    undoData.type = UNDOSTACK_RESIZE_LAYER;
    undoData.extdata = tileDimensions.x;
    undoData.extdata2 = tileDimensions.y;
    undoData.extdata4 = layerResizeData;
    addToUndoStack(undoData);
    tileDimensions = downscale ? XY{tileDimensions.x / scale.x, tileDimensions.y / scale.y} : XY{tileDimensions.x * scale.x, tileDimensions.y * scale.y};
}

MainEditorPalettized* MainEditor::toPalettizedSession()
{
    if (isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "?????"));
        return NULL;
    }
    else {
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

bool MainEditor::canAddCommentAt(XY a)
{
    for (CommentData& c : comments) {
        if (xyEqual(c.position, a)) {
            return false;
        }
    }
    return true;
}

void MainEditor::addComment(CommentData c)
{
    if (canAddCommentAt(c.position)) {
        addToUndoStack(UndoStackElement{ NULL, UNDOSTACK_ADD_COMMENT, c.position.x, c.position.y, c.data });
        comments.push_back(c);
    }
}

void MainEditor::addCommentAt(XY a, std::string c)
{
    if (canAddCommentAt(a)) {
        addToUndoStack(UndoStackElement{ NULL, UNDOSTACK_ADD_COMMENT, a.x, a.y, c });

        CommentData newComment = { a, c };
        comments.push_back(newComment);
    }
}

void MainEditor::removeCommentAt(XY a)
{
    CommentData c = _removeCommentAt(a);
    if (c.data[0] != '\1') {

        addToUndoStack(UndoStackElement{ NULL, UNDOSTACK_REMOVE_COMMENT, a.x, a.y, c.data });
    }
}

CommentData MainEditor::_removeCommentAt(XY a)
{
    for (int x = 0; x < comments.size(); x++) {
        if (xyEqual(comments[x].position, a)) {
            CommentData c = comments[x];
            comments.erase(comments.begin() + x);
            return c;
        }
    }
    logprintf("_removeComment NOT FOUND\n");
    //shitass workaround tell noone thanks
    //@hirano185 hey girlie check this out!
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
            if (neighborCount > 0 && neighborCount != 4) {
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
            if (l->isPalettized) {
                if (px != -1) {
                    isolatedFragment.addPoint({ x,y });
                    p++;
                }
            }
            else {
                if ((px & 0xFF000000) != 0) {
                    isolatedFragment.addPoint({ x,y });
                    p++;
                }
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
