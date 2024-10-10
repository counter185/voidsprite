#include "maineditor.h"
#include "MainEditorPalettized.h"
#include "FontRenderer.h"
#include "EditorBrushPicker.h"
#include "EditorLayerPicker.h"
#include "ScreenWideNavBar.h"
#include "Notification.h"
#include "SpritesheetPreviewScreen.h"
#include "EditorSpritesheetPreview.h"
#include "RPG2KTilemapPreviewScreen.h"
#include "FileIO.h"
#include "TilemapPreviewScreen.h"
#include "MinecraftSkinPreviewScreen.h"
#include "Gamepad.h"
#include "LayerPalettized.h"
#include "UICheckbox.h"
#include "TooltipsLayer.h"

#include "PopupIntegerScale.h"
#include "PopupTextBox.h"
#include "PopupSetEditorPixelGrid.h"
#include "PopupTileGeneric.h"
#include "PopupYesNo.h"
#include "PopupGlobalConfig.h"
#include "PopupPickColor.h"

MainEditor::MainEditor(XY dimensions) {

    texW = dimensions.x;
    texH = dimensions.y;
    //canvasCenterPoint = XY{ texW / 2, texH / 2 };
    layers.push_back(new Layer(texW, texH));
    FillTexture();

    setUpWidgets();
    recenterCanvas();
    initLayers();
}
MainEditor::MainEditor(SDL_Surface* srf) {

    //todo i mean just use MainEditor(Layer*) here
    texW = srf->w;
    texH = srf->h;

    Layer* nlayer = new Layer(texW, texH);
    layers.push_back(nlayer);
    SDL_ConvertPixels(srf->w, srf->h, srf->format->format, srf->pixels, srf->pitch, SDL_PIXELFORMAT_ARGB8888, nlayer->pixelData, texW*4);

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditor::MainEditor(Layer* layer)
{

    texW = layer->w;
    texH = layer->h;

    layers.push_back(layer);

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditor::MainEditor(std::vector<Layer*> layers)
{
    texW = layers[0]->w;
    texH = layers[0]->h;
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
}


void MainEditor::render() {
    SDL_SetRenderDrawColor(g_rd, backgroundColor.r/6*5, backgroundColor.g/6*5, backgroundColor.b/6*5, 255);
    SDL_RenderClear(g_rd);
    DrawBackground();

    SDL_Rect canvasRenderRect;
    canvasRenderRect.w = texW * scale;
    canvasRenderRect.h = texH * scale;
    canvasRenderRect.x = canvasCenterPoint.x;
    canvasRenderRect.y = canvasCenterPoint.y;
    int orient = 0;
    for (int x = 0; x < layers.size(); x++) {
        Layer* imgLayer = layers[x];
        if (!imgLayer->hidden) {
            uint8_t alpha = imgLayer->layerAlpha;
            imgLayer->render(canvasRenderRect, (layerSwitchTimer.started && x == selLayer) ? (uint8_t)(alpha * XM1PW3P1(layerSwitchTimer.percentElapsedTime(1300))) : alpha);
        }
    }

    //draw a separate 1x1 grid if the scale is >= 1600%
    if (scale >= 10) {

        uint8_t tileGridAlpha = scale < 16 ? 0x10 * ((scale - 9) / 7.0) : 0x10;

        int dx = canvasRenderRect.x;
        while (dx < g_windowW && dx < canvasRenderRect.x + canvasRenderRect.w) {
            dx += scale;
            if (dx >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, tileGridAlpha);
                SDL_RenderDrawLine(g_rd, dx, canvasRenderRect.y, dx, canvasRenderRect.y + canvasRenderRect.h);
            }
        }
        int dy = canvasRenderRect.y;
        while (dy < g_windowH && dy < canvasRenderRect.y + canvasRenderRect.h) {
            dy += scale;
            if (dy >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, tileGridAlpha);
                SDL_RenderDrawLine(g_rd, canvasRenderRect.x, dy, canvasRenderRect.x + canvasRenderRect.w, dy);
            }
        }
    }

    //draw tile lines
    if (tileDimensions.x != 0) {
        int dx = canvasRenderRect.x;
        while (dx < g_windowW && dx < canvasRenderRect.x + canvasRenderRect.w) {
            dx += tileDimensions.x * scale;
            if (dx >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, tileGridAlpha);
                SDL_RenderDrawLine(g_rd, dx, canvasRenderRect.y, dx, canvasRenderRect.y + canvasRenderRect.h);
            }
        }
    }
    if (tileDimensions.y != 0) {
        int dy = canvasRenderRect.y;
        while (dy < g_windowH && dy < canvasRenderRect.y + canvasRenderRect.h) {
            dy += tileDimensions.y * scale;
            if (dy >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff-backgroundColor.r, 0xff-backgroundColor.g, 0xff-backgroundColor.b, tileGridAlpha);
                SDL_RenderDrawLine(g_rd, canvasRenderRect.x, dy, canvasRenderRect.x + canvasRenderRect.w, dy);
            }
        }
    }
    drawIsolatedRect();
    drawSymmetryLines();
    renderGuidelines(orient);

    //draw tile repeat preview
    if (qModifier || (lockedTilePreview.x >= 0 && lockedTilePreview.y >= 0)) {
        XY tileDim = tileDimensions.x != 0 && tileDimensions.y != 0 ? tileDimensions : XY{texW, texH};
        XY mouseInCanvasPoint = XY{
            (canvasCenterPoint.x - g_mouseX) / -scale,
            (canvasCenterPoint.y - g_mouseY) / -scale
        };
        if ((qModifier && mouseInCanvasPoint.x >= 0 && mouseInCanvasPoint.y >= 0
            && mouseInCanvasPoint.x < texW && mouseInCanvasPoint.y < texH) || !qModifier) {
            XY tilePosition = !qModifier ? lockedTilePreview :
                XY{
                    mouseInCanvasPoint.x / tileDim.x,
                    mouseInCanvasPoint.y / tileDim.y
                };
            SDL_Rect tileRect = {
                canvasCenterPoint.x + tilePosition.x * tileDim.x * scale,
                canvasCenterPoint.y + tilePosition.y * tileDim.y * scale,
                tileDim.x * scale,
                tileDim.y * scale
            };
            SDL_Rect canvasClipRect = {
                tilePosition.x * tileDim.x,
                tilePosition.y * tileDim.y,
                tileDim.x,
                tileDim.y
            };

            for (int yy = -1; yy <= 1; yy++) {
                for (int xx = -1; xx <= 1; xx++) {
                    if (yy == 0 && xx == 0) {
                        continue;
                    }

                    for (int x = 0; x < layers.size(); x++) {
                        Layer* imgLayer = layers[x];
                        if (!imgLayer->hidden) {
                            uint8_t alpha = imgLayer->layerAlpha;
                            XY position = { tileRect.x + (xx * scale * tileDim.x),
                                tileRect.y + (yy * scale * tileDim.y)};
                            SDL_Rect finalTileRect = { position.x, position.y, tileRect.w, tileRect.h };
                            imgLayer->render(finalTileRect, canvasClipRect, alpha);
                        }
                    }
                }
            }

            //lock animation
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
            double anim = 1.0 - XM1PW3P1(tileLockTimer.percentElapsedTime(300));
            drawLine(XY{ tileRect.x, tileRect.y }, XY{ tileRect.x + tileRect.w, tileRect.y }, anim);
            drawLine(XY{ tileRect.x, tileRect.y }, XY{ tileRect.x, tileRect.y + tileRect.h }, anim);
            drawLine(XY{ tileRect.x + tileRect.w, tileRect.y + tileRect.h }, XY{ tileRect.x, tileRect.y + tileRect.h }, anim);
            drawLine(XY{ tileRect.x + tileRect.w, tileRect.y + tileRect.h }, XY{ tileRect.x + tileRect.w, tileRect.y }, anim);
        }
    }

    renderComments();

    renderUndoStack();

    if (currentBrush != NULL) {
        currentBrush->renderOnCanvas(this, scale);
    }

    /*if (spritesheetPreview != NULL) {
        spritesheetPreview->previewWx->render(XY{ 0,0 });
    }*/

    //g_fnt->RenderString(std::string("Scale: ") + std::to_string(scale), 0, 20);
    //g_fnt->RenderString(std::string("MousePixelPoint: ") + std::to_string(mousePixelTargetPoint.x) + std::string(":") + std::to_string(mousePixelTargetPoint.y), 0, 50);

    

    if (wxsManager.anyFocused() && navbar->focused) {
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        SDL_RenderFillRect(g_rd, NULL);
    }

    DrawForeground();

    wxsManager.renderAll();

    if (eraserMode) {
        SDL_Rect eraserRect = { g_mouseX + 6, g_mouseY - 30, 28, 28 };
        SDL_SetTextureAlphaMod(g_iconEraser, 0xa0);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x60);
        SDL_RenderFillRect(g_rd, &eraserRect);
        SDL_RenderCopy(g_rd, g_iconEraser, NULL, &eraserRect);
    }
    renderColorPickerAnim();
}

void MainEditor::tick() {

    if (abs(g_gamepad->gamepadLSX) > 0.05f || abs(g_gamepad->gamepadLSY) > 0.05f) {
        canvasCenterPoint.x += g_gamepad->gamepadLSX * 10;
        canvasCenterPoint.y += g_gamepad->gamepadLSY * 10;
        RecalcMousePixelTargetPoint(g_windowW / 2, g_windowH / 2);
        currentBrush->mouseMotion(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
    }

    canvasCenterPoint = XY{
        iclamp(-texW * scale + 4, canvasCenterPoint.x, g_windowW - 4),
        iclamp(-texH * scale + 4, canvasCenterPoint.y, g_windowH - 4)
    };

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

    if (closeNextTick) {
        g_closeScreen(this);
    }
}

void MainEditor::DrawBackground()
{
    uint32_t colorBG1 = 0xFF000000 | (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x000000 : 0xDFDFDF);
    uint32_t colorBG2 = 0xFF000000 | (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x202020 : 0x808080);
    renderGradient({ 0,0, g_windowW, g_windowH }, colorBG1, colorBG1, colorBG1, colorBG2);

    uint64_t bgtimer = g_config.animatedBackground >= 3 ? 0 : SDL_GetTicks64();
    if (g_config.animatedBackground == 1 || g_config.animatedBackground == 3) {
        int lineX = 400;
        for (int x = 40 + (bgtimer % 5000 / 5000.0 * 60); x < g_windowW + lineX; x += 60) {
            SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x40);
            SDL_RenderDrawLine(g_rd, x, 0, x - lineX, g_windowH);
            SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x0d);
            SDL_RenderDrawLine(g_rd, g_windowW - x, 0, g_windowW - x + lineX / 4 * 6, g_windowH);
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
    int lw = texW * scale + 2;
    int lh = texH * scale + 2;
    SDL_Rect r = { canvasCenterPoint.x - 1, canvasCenterPoint.y - 1, lw, lh };
    uint8_t a = 0xff;
    SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, a);
    SDL_RenderDrawRect(g_rd, &r);
    for (int x = 0; x < 6; x++) {
        r.w += 2;
        r.h += 2;
        r.x -= 1;
        r.y -= 1;
        a /= 2;
        SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, a);
        SDL_RenderDrawRect(g_rd, &r);
    }
}

void MainEditor::drawSymmetryLines() {
    if (symmetryEnabled[0]) {
        int symXPos = symmetryPositions.x / 2;
        bool symXMiddle = symmetryPositions.x % 2;
        int lineDrawXPoint = canvasCenterPoint.x + symXPos * scale + (symXMiddle ? scale/2 : 0);

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
        SDL_RenderDrawLine(g_rd, lineDrawXPoint, 0, lineDrawXPoint, g_windowH);
    }
    if (symmetryEnabled[1]) {
        int symYPos = symmetryPositions.y / 2;
        bool symYMiddle = symmetryPositions.y % 2;
        int lineDrawYPoint = canvasCenterPoint.y + symYPos * scale + (symYMiddle ? scale/2 : 0);

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
        SDL_RenderDrawLine(g_rd,0, lineDrawYPoint, g_windowW, lineDrawYPoint);
    }
}

void MainEditor::drawIsolatedRect()
{
    if (isolateEnabled) {
        SDL_Rect r = { 
            canvasCenterPoint.x + isolateRect.x * scale, 
            canvasCenterPoint.y + isolateRect.y * scale,
            isolateRect.w * scale,
            isolateRect.h * scale
        };

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
        SDL_RenderDrawRect(g_rd, &r);

        r.x += 1;
        r.y += 1;
        r.w -= 2;
        r.h -= 2;

        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        SDL_RenderDrawRect(g_rd, &r);
    }
}

void MainEditor::renderGuidelines(int o) {

        int gYPos = guidelinePosition.y / 2;
        bool gYMiddle = guidelinePosition.y % 2;
        int lineDrawYPoint = canvasCenterPoint.y + gYPos * scale + (gYMiddle ? scale/2 : 0);

        int gXPos = guidelinePosition.x / 2;
        bool gXMiddle = guidelinePosition.x % 2;
        int lineDrawXPoint = canvasCenterPoint.x + gXPos * scale + (gXMiddle ? scale/2 : 0);

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
        switch (o) {
        case 0:
            //horizontal
            SDL_RenderDrawLine(g_rd, 0, lineDrawYPoint, g_windowW, lineDrawYPoint);
            break;
        case 1:
            //vertical
            SDL_RenderDrawLine(g_rd, lineDrawXPoint, 0, lineDrawXPoint, g_windowH);
            break;
        case 2:
            //todo let me do the math (DIAGONAL TL-BR)
            break;
        case 3:
            //todo let me do the math (DIAGONAL TR-BL)
            break;
        }
        
    
}

void MainEditor::DrawForeground()
{
    SDL_Rect r = { 0, g_windowH - 30, g_windowW, 30 };
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xb0);
    SDL_RenderFillRect(g_rd, &r);

    g_fnt->RenderString(std::format("{}x{} ({}%)", texW, texH, scale * 100), 2, g_windowH - 28, SDL_Color{255,255,255,0xa0});

    XY endpoint = g_fnt->RenderString(std::format("{}:{}", mousePixelTargetPoint.x, mousePixelTargetPoint.y), 200, g_windowH - 28, SDL_Color{255,255,255,0xd0});
    if (tileDimensions.x != 0 && tileDimensions.y != 0) {
        std::string s = std::format("(t{}:{})", (int)floor(mousePixelTargetPoint.x / (float)tileDimensions.x), (int)floor(mousePixelTargetPoint.y / (float)tileDimensions.y));
        endpoint = g_fnt->RenderString(s, endpoint.x + 5, endpoint.y, SDL_Color{ 255,255,255,0x90 });
    }

    if (currentBrush != NULL) {
        g_fnt->RenderString(std::format("{} {}", currentBrush->getName(), eraserMode ? "(Erase)" : ""), ixmax(endpoint.x + 10, 370), g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
    }

    if (currentPattern != NULL) {
        g_fnt->RenderString(std::format("{}", currentPattern->getName()), 620, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
    }
}

void MainEditor::renderComments()
{
    if (commentViewMode == COMMENTMODE_HIDE_ALL) {
        return;
    }
    TooltipsLayer localTtp;
    localTtp.border = false;
    localTtp.gradientUL = localTtp.gradientUR = 0x80000000;
    localTtp.gradientLL = localTtp.gradientLR = 0x40000000;

    XY origin = canvasCenterPoint;
    for (CommentData& c : comments) {
        XY onScreenPosition = xyAdd(origin, { c.position.x * scale, c.position.y * scale });
        SDL_Rect iconRect = { onScreenPosition.x, onScreenPosition.y, 16, 16 };
        SDL_SetTextureAlphaMod(g_iconComment, 0x80);
        SDL_RenderCopy(g_rd, g_iconComment, NULL, &iconRect);
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

void MainEditor::renderColorPickerAnim()
{
    if (colorPickTimer.started && colorPickTimer.percentElapsedTime(500) <= 1.0f) {
        float progress = colorPickTimer.percentElapsedTime(500);
        int pxDistance = 40 * (lastColorPickWasFromWholeImage ? XM1PW3P1(progress) : (1.0 - XM1PW3P1(progress)));

        SDL_Rect colRect2 = { g_mouseX, g_mouseY, 1,1 };
        int pxDistance2 = pxDistance + 1;
        colRect2.x -= pxDistance2;
        colRect2.w = pxDistance2 * 2;
        colRect2.y -= pxDistance2;
        colRect2.h = pxDistance2 * 2;

        //SDL_SetRenderDrawColor(g_rd, 255,255,255, (uint8_t)(127 * XM1PW3P1(1.0f - progress)));
        //SDL_RenderDrawRect(g_rd, &colRect2);

        for (int x = 3; x >= 1; x--) {
            SDL_Rect colRect = { g_mouseX, g_mouseY, 1,1 };
            int nowPxDistance = pxDistance / x;
            colRect.x -= nowPxDistance;
            colRect.w = nowPxDistance * 2;
            colRect.y -= nowPxDistance;
            colRect.h = nowPxDistance * 2;
            hsv thsv = rgb2hsv(rgb{ ((pickedColor >> 16) & 0xff) / 255.0f, ((pickedColor >> 8) & 0xff) / 255.0f, (pickedColor & 0xff) / 255.0f });
            thsv.s /= 3;
            thsv.v += 0.4;
            thsv.v = dxmin(1.0, thsv.v);
            rgb trgb = hsv2rgb(thsv);
            SDL_Color trgbColor = SDL_Color{ (uint8_t)(trgb.r * 255.0), (uint8_t)(trgb.g * 255.0), (uint8_t)(trgb.b * 255.0), 255 };
            SDL_SetRenderDrawColor(g_rd, trgbColor.r, trgbColor.g, trgbColor.b, (uint8_t)(255 * XM1PW3P1(1.0f - progress)));
            SDL_RenderDrawRect(g_rd, &colRect);
        }
    }
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
            SDLK_f,
            {
                "File",
                {SDLK_s, SDLK_d, SDLK_e, SDLK_a, SDLK_r, SDLK_p, SDLK_c},
                {
                    {SDLK_d, { "Save as",
                            [](MainEditor* editor) {
                                editor->trySaveAsImage();
                            }
                        }
                    },
                    {SDLK_s, { "Save",
                            [](MainEditor* editor) {
                                editor->trySaveImage();
                            }
                        }
                    },
                    {SDLK_e, { "Export as palettized",
                            [](MainEditor* editor) {
                                editor->tryExportPalettizedImage();
                            }
                        }
                    },
                    {SDLK_a, { "Export tiles individually",
                            [](MainEditor* editor) {
                                editor->exportTilesIndividually();
                            }
                        }
                    },
                    {SDLK_r, { "Open in palettized editor",
                            [](MainEditor* editor) {
                                MainEditorPalettized* newEditor = editor->toPalettizedSession();
                                if (newEditor != NULL) {
									g_addScreen(newEditor);
								}
                            }
                        }
                    },
                    {SDLK_c, { "Close",
                            [](MainEditor* editor) {
                                editor->requestSafeClose();
                            }
                        }
                    },
                    {SDLK_p, { "Preferences",
                            [](MainEditor* screen) {
                                g_addPopup(new PopupGlobalConfig());
                            }
                        }
                    }
                },
                g_iconNavbarTabFile
            }
        },
        {
            SDLK_e,
            {
                "Edit",
                {SDLK_z, SDLK_r, SDLK_x, SDLK_y, SDLK_s, SDLK_c, SDLK_v, SDLK_b, SDLK_n},
                {
                    {SDLK_z, { "Undo",
                            [](MainEditor* editor) {
                                editor->undo();
                            }
                        }
                    },
                    {SDLK_r, { "Redo",
                            [](MainEditor* editor) {
                                editor->redo();
                            }
                        }
                    },
                    {SDLK_x, { "Toggle symmetry: X",
                            [](MainEditor* editor) {
                                editor->symmetryEnabled[0] = !editor->symmetryEnabled[0];
                            }
                        }
                    },
                    {SDLK_y, { "Toggle symmetry: Y",
                            [](MainEditor* editor) {
                                editor->symmetryEnabled[1] = !editor->symmetryEnabled[1];
                            }
                        }
                    },
                    {SDLK_c, { "Resize canvas",
                            [](MainEditor* editor) {
                                g_addPopup(new PopupTileGeneric(editor, "Resize canvas", "New canvas size:", XY{editor->texW, editor->texH}, EVENT_MAINEDITOR_RESIZELAYER));
                            }
                        }
                    },
                    {SDLK_s, { "Deselect",
                            [](MainEditor* editor) {
                                editor->isolateEnabled = false;
                            }
                        }
                    },
                    {SDLK_v, { "Resize canvas (per tile)",
                            [](MainEditor* editor) {
                                if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
                                }
                                else {
                                    g_addPopup(new PopupTileGeneric(editor, "Resize canvas by tile size", "New tile size:", XY{ editor->tileDimensions.x, editor->tileDimensions.y }, EVENT_MAINEDITOR_RESIZELAYER_BY_TILE));
                                }
                            }
                        }
                    },
                    {SDLK_b, { "Resize canvas (per n.tiles)",
                            [](MainEditor* editor) {
                                if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
                                }
                                else {
                                    g_addPopup(new PopupTileGeneric(editor, "Resize canvas by tile count", "New tile count:", XY{ (int)ceil(editor->texW / (float)editor->tileDimensions.x), (int)ceil(editor->texH / (float)editor->tileDimensions.y) }, EVENT_MAINEDITOR_RESIZELAYER_BY_TILECOUNT));
                                }
                            }
                        }
                    },
                    {SDLK_n, { "Integer scale canvas",
                            [](MainEditor* editor) {
                                g_addPopup(new PopupIntegerScale(editor, "Integer scale canvas", "Scale:", XY{ 1,1 }, EVENT_MAINEDITOR_INTEGERSCALE));
                            }
                        }
                    },
                },
                g_iconNavbarTabEdit
            }
        },
        {
            SDLK_l,
            {
                "Layer",
                {},
                {
                    {SDLK_f, { "Flip current layer: X axis",
                            [](MainEditor* editor) {
                                editor->layer_flipHorizontally();
                            }
                        }
                    },
                    {SDLK_g, { "Flip current layer: Y axis",
                            [](MainEditor* editor) {
                                editor->layer_flipVertically();
                            }
                        }
                    },
                    {SDLK_b, { "Swap channels RGB->BGR",
                            [](MainEditor* editor) {
                                editor->layer_swapLayerRGBtoBGR();
                            }
                        }
                    },
                    {SDLK_x, { "Print number of colors",
                            [](MainEditor* editor) {
                                g_addNotification(Notification("", std::format("{} colors in current layer", editor->getCurrentLayer()->numUniqueColors(true))));
                            }
                        }
                    },
                    {SDLK_r, { "Rename current layer",
                            [](MainEditor* editor) {
                                editor->layer_promptRename();
                            }
                        }
                    },
                    {SDLK_a, { "Remove alpha channel",
                            [](MainEditor* editor) {
                                editor->layer_setAllAlpha255();
                            }
                        }
                    },
                    {SDLK_k, { "Set color key",
                            [](MainEditor* editor) {
                                PopupPickColor* newPopup = new PopupPickColor("Set color key", "Pick a color to set as the color key:");
                                newPopup->setCallbackListener(EVENT_MAINEDITOR_SETCOLORKEY, editor);
                                g_addPopup(newPopup);
                            }
                        }
                    },
                },
                g_iconNavbarTabLayer
            }
        },
        {
            SDLK_v,
            {
                "View",
                {},
                {
                    {SDLK_r, { "Recenter canvas",
                            [](MainEditor* editor) {
                                editor->recenterCanvas();
                            }
                        }
                    },
                    {SDLK_b, { "Toggle background color",
                            [](MainEditor* editor) {
                                editor->backgroundColor.r = ~editor->backgroundColor.r;
                                editor->backgroundColor.g = ~editor->backgroundColor.g;
                                editor->backgroundColor.b = ~editor->backgroundColor.b;
                            }
                        }
                    },
                    {SDLK_c, { "Toggle comments",
                            [](MainEditor* editor) {
                                (*(int*)&editor->commentViewMode)++;
                                (*(int*)&editor->commentViewMode) %= 3;
                                g_addNotification(Notification(std::format("{}",
                                    editor->commentViewMode == COMMENTMODE_HIDE_ALL ? "All comments hidden" :
                                    editor->commentViewMode == COMMENTMODE_SHOW_HOVERED ? "Comments shown on hover" :
                                    "All comments shown"), "", 1500
                                ));
                            }
                        }
                    },
                    {SDLK_g, { "Set pixel grid...",
                            [](MainEditor* editor) {
                                g_addPopup(new PopupSetEditorPixelGrid(editor, "Set pixel grid", "Enter grid size <w>x<h>:"));
                            }
                        }
                    },
                    {SDLK_s, { "Open spritesheet preview...",
                            [](MainEditor* editor) {
                                //if (editor->spritesheetPreview == NULL) {
                                    if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                        g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
                                        return;
                                    }
                                    SpritesheetPreviewScreen* newScreen = new SpritesheetPreviewScreen(editor);
                                    g_addScreen(newScreen);
                                    //editor->spritesheetPreview = newScreen;
                                //}
                                //else {
                                //    g_addNotification(ErrorNotification("Error", "Spritesheet preview is already open."));
                                //}
                            }
                        }
                    },
                    {SDLK_t, { "Open tileset preview...",
                            [](MainEditor* editor) {
                                if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
                                    return;
                                }
                                TilemapPreviewScreen* newScreen = new TilemapPreviewScreen(editor);
                                g_addScreen(newScreen);
                                //editor->spritesheetPreview = newScreen;
                            }
                        }
                    },
                    {SDLK_y, { "Open RPG Maker 2K/2K3 ChipSet preview...",
                            [](MainEditor* editor) {
                                if (editor->texW != 480 || editor->texH != 256) {
                                    g_addNotification(ErrorNotification("Error", "Dimensions must be 480x256"));
                                    return;
                                }
                                RPG2KTilemapPreviewScreen* newScreen = new RPG2KTilemapPreviewScreen(editor);
                                g_addScreen(newScreen);
                                //editor->spritesheetPreview = newScreen;
                            }
                        }
                    },
    #if _DEBUG
                    {SDLK_m, { "Open Minecraft skin preview...",
                            [](MainEditor* editor) {
                                if (editor->texW != editor->texH && editor->texW / 2 != editor->texH) {
                                    g_addNotification(ErrorNotification("Error", "Invalid size. Aspect must be 1:1 or 2:1."));
                                    return;
                                }
                                MinecraftSkinPreviewScreen* newScreen = new MinecraftSkinPreviewScreen(editor);
                                g_addScreen(newScreen);
                                //editor->spritesheetPreview = newScreen;
                            }
                        }
                    },
    #endif
                },
                g_iconNavbarTabView
            }
        }
    };

    currentBrush = g_brushes[0];
    currentPattern = g_patterns[0];

    colorPicker = new EditorColorPicker(this);
    colorPicker->position.y = 80;
    colorPicker->position.x = 10;
    wxsManager.addDrawable(colorPicker);
    colorPicker->setMainEditorColorRGB(pickedColor);
    regenerateLastColors();

    brushPicker = new EditorBrushPicker(this);
    brushPicker->position.y = 480;
    brushPicker->position.x = 10;
    wxsManager.addDrawable(brushPicker);

    layerPicker = new EditorLayerPicker(this);
    layerPicker->position = XY{ 440, 80 };
    layerPicker->anchor = XY{ 1,0 };
    wxsManager.addDrawable(layerPicker);

    navbar = new ScreenWideNavBar<MainEditor*>(this, mainEditorKeyActions, { SDLK_f, SDLK_e, SDLK_l, SDLK_v });
    wxsManager.addDrawable(navbar);
}

void MainEditor::addWidget(Drawable* wx)
{
    wxsManager.addDrawable(wx);
}

void MainEditor::removeWidget(Drawable* wx)
{
    wxsManager.removeDrawable(wx);
}

void MainEditor::RecalcMousePixelTargetPoint(int x, int y) {
    mousePixelTargetPoint =
        XY{
            (canvasCenterPoint.x - x) / -scale,
            (canvasCenterPoint.y - y) / -scale
        };
    mousePixelTargetPoint2xP =
        XY{
            (int)((canvasCenterPoint.x - x) / (float)(-scale) / 0.5f),
            (int)((canvasCenterPoint.y - y) / (float)(-scale) / 0.5f)
    };
}

bool MainEditor::requestSafeClose() {
    if (!changesSinceLastSave) {
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
    XY screenCenterPoint = XY{
            (canvasCenterPoint.x - g_windowW/2) / -scale,
            (canvasCenterPoint.y - g_windowH/2) / -scale
    };
    scale += how_much;
    scale = scale < 1 ? 1 : scale;
    XY onscreenPointNow = XY{
        canvasCenterPoint.x + screenCenterPoint.x * scale,
        canvasCenterPoint.y + screenCenterPoint.y * scale
    };
    XY pointDiff = xySubtract(XY{ g_windowW / 2, g_windowH / 2 }, onscreenPointNow);
    canvasCenterPoint = xyAdd(canvasCenterPoint, pointDiff);
}

bool MainEditor::isInBounds(XY pos)
{
    return
        pointInBox(pos, { 0,0,texW,texH })
        && (!isolateEnabled || (isolateEnabled && pointInBox(pos, isolateRect)));
}

void MainEditor::takeInput(SDL_Event evt) {

    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        if (requestSafeClose()) {
            return;
        }
    }

    LALT_TO_SUMMON_NAVBAR;

    if ((evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) && evt.key.keysym.sym == SDLK_q) {
        qModifier = evt.key.state;
    }

    if (!DrawableManager::processInputEventInMultiple({wxsManager}, evt)) {
        switch (evt.type) {
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (evt.button.button == 1) {
                    RecalcMousePixelTargetPoint(evt.button.x, evt.button.y);
                    if (currentBrush != NULL) {
                        if (evt.button.state) {
                            if (!currentBrush->isReadOnly()) {
                                commitStateToCurrentLayer();
                            }
                            currentBrush->clickPress(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                        }
                        else {
                            currentBrush->clickRelease(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                        }
                    }
                    mouseHoldPosition = mousePixelTargetPoint;
                    leftMouseHold = evt.button.state;
                }
                else if (evt.button.button == 2) {
                    middleMouseHold = evt.button.state;
                }
                else if (evt.button.button == 3) {
                    RecalcMousePixelTargetPoint(evt.button.x, evt.button.y);
                    if (currentBrush != NULL && currentBrush->overrideRightClick()) {
                        if (evt.button.state) {
                            currentBrush->rightClickPress(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                        }
                        else {
                            currentBrush->rightClickRelease(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
                        }
                    }
                    else {
                        if (evt.button.state) {
                            lastColorPickWasFromWholeImage = !g_ctrlModifier;
                            setActiveColor(g_ctrlModifier ? getCurrentLayer()->getPixelAt(mousePixelTargetPoint) : pickColorFromAllLayers(mousePixelTargetPoint));
                        }
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                RecalcMousePixelTargetPoint(evt.motion.x, evt.motion.y);
                if (middleMouseHold) {
                    canvasCenterPoint.x += evt.motion.xrel * (g_shiftModifier ? 2 : 1);
                    canvasCenterPoint.y += evt.motion.yrel * (g_shiftModifier ? 2 : 1);
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
                break;
            case SDL_MOUSEWHEEL:
                if (g_ctrlModifier && !g_config.scrollWithTouchpad) {
                    colorPicker->setMainEditorColorHSV(colorPicker->currentH, fxmin(fxmax(colorPicker->currentS + 0.1 * evt.wheel.y, 0), 1), colorPicker->currentV);
                }
                else if (g_shiftModifier && !g_config.scrollWithTouchpad) {
                    double newH = dxmin(dxmax(colorPicker->currentH + (360.0 / 12) * evt.wheel.y, 0), 359);
                    colorPicker->setMainEditorColorHSV(newH, colorPicker->currentS, colorPicker->currentV);
                }
                else {
                    if (g_config.scrollWithTouchpad && !g_ctrlModifier) {
                        canvasCenterPoint.x -= (g_shiftModifier ? evt.wheel.y : evt.wheel.x) * 20;
                        canvasCenterPoint.y += (g_shiftModifier ? evt.wheel.x : evt.wheel.y) * 20;
                    }
                    else {
                        zoom(evt.wheel.y);
                    }
                }
                break;
            case SDL_KEYDOWN:
                switch (evt.key.keysym.sym) {
                    case SDLK_e:
                        colorPicker->eraserButton->click();
                        //colorPicker->toggleEraser();
                        break;
                    case SDLK_RCTRL:
                        middleMouseHold = !middleMouseHold;
                        break;
                    case SDLK_z:
                        if (g_ctrlModifier) {
                            if (g_shiftModifier) {
                                redo();
                            }
                            else {
                                undo();
                            }
                        }
                        break;
                    case SDLK_y:
                        if (g_ctrlModifier) {
                            redo();
                        }
                        break;
                    case SDLK_s:
                        if (g_ctrlModifier) {
                            if (g_shiftModifier) {
                                trySaveAsImage();
                            }
                            else {
                                trySaveImage();
                            }
                        }
                        break;
                    case SDLK_q:
                        if (g_ctrlModifier) {
                            if (lockedTilePreview.x != -1 && lockedTilePreview.y != -1) {
                                lockedTilePreview = { -1,-1 };
                            }
                            else {
                                
                                if (tileDimensions.x != 0 && tileDimensions.y != 0) {
                                    XY mouseInCanvasPoint = XY{
                                        (canvasCenterPoint.x - g_mouseX) / -scale,
                                        (canvasCenterPoint.y - g_mouseY) / -scale
                                    };
                                    XY tileToLock = XY{
                                        mouseInCanvasPoint.x / tileDimensions.x,
                                        mouseInCanvasPoint.y / tileDimensions.y
                                    };
                                    if (g_config.isolateRectOnLockTile) {
                                        isolateEnabled = true;
                                        isolateRect = { tileToLock.x * tileDimensions.x, tileToLock.y * tileDimensions.y, tileDimensions.x, tileDimensions.y };
                                    }
                                    if (tileToLock.x >= 0 && tileToLock.y >= 0) {
                                        lockedTilePreview = tileToLock;
                                        tileLockTimer.start();
                                    }
                                    else {
                                        g_addNotification(ErrorNotification("Error", "Tile position out of bounds"));
                                    }
                                }
                                else {
                                    lockedTilePreview = { 0,0 };
                                    tileLockTimer.start();
                                }
                            }
                        }
                        break;
                    case SDLK_F2:
                        layer_promptRename();
                        break;
                }
                break;
        }
    }
}

void MainEditor::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterID)
{
    if (evt_id == EVENT_MAINEDITOR_SAVEFILE) {
        exporterID--;
        printf("eventFileSaved: got file name %ls\n", name.c_str());

        bool result = false;

        FileExporter* exporter = NULL;
        if (exporterID < g_fileExporters.size()) {
            exporter = g_fileExporters[exporterID];

            result = trySaveWithExporter(name, exporter);
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
                    g_addNotification(ErrorNotification("Error", "Failed to exported file."));
                }
            }
            else {
                g_addNotification(ErrorNotification("Error", "Failed to exported file."));
            }
        }
        else {
            g_addNotification(ErrorNotification("Error", "Invalid exporter"));
        }
    }
    else if (evt_id == EVENT_MAINEDITOR_EXPORTTILES) {
        exporterID--;

        FileExporter* exporter = g_fileExporters[exporterID];
        XY tileCounts = { texW / tileDimensions.x, texH / tileDimensions.y };
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
                    g_addNotification(ErrorNotification("Error", std::format("Failed to export tile {}:{}", x,y)));
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
        l->layerDirty = true;
    }
}

void MainEditor::FillTexture() {
    int* pixels = (int*)getCurrentLayer()->pixelData;
    //int pitch;
    //SDL_LockTexture(mainTexture, NULL, (void**)&pixels, &pitch);
    for (int x = 0; x < texW; x++) {
        for (int y = 0; y < texH; y++) {
            pixels[x + (y * texW)] = 0x00000000;
        }
    }
    //SDL_UnlockTexture(mainTexture);
}

void MainEditor::SetPixel(XY position, uint32_t color, uint8_t symmetry) {
    if (currentPattern->canDrawAt(position) && (!replaceAlphaMode || (replaceAlphaMode && ((layer_getPixelAt(position) & 0xFF000000) != 0)))) {
        if (!isolateEnabled || (isolateEnabled && pointInBox(position, isolateRect))) {
            uint32_t targetColor = color;
            if (blendAlphaMode) {
                if (eraserMode) {
                    targetColor = ((0xff - (targetColor >> 24)) << 24) + (targetColor & 0xffffff);
                }
                targetColor = alphaBlend(getCurrentLayer()->getPixelAt(position), targetColor);
            }
            getCurrentLayer()->setPixel(position, targetColor & (eraserMode ? 0xffffff : 0xffffffff));
            colorPicker->pushLastColor(color);
        }
    }
    if (symmetryEnabled[0] && !(symmetry & 0b10)) {
        int symmetryXPoint = symmetryPositions.x / 2;
        bool symXPointIsCentered = symmetryPositions.x % 2;
        int symmetryFlippedX = symmetryXPoint + (symmetryXPoint - position.x) - (symXPointIsCentered ? 0 : 1);
        SetPixel(XY{symmetryFlippedX, position.y}, color, symmetry | 0b10);
    }
    if (symmetryEnabled[1] && !(symmetry & 0b1)) {
        int symmetryYPoint = symmetryPositions.y / 2;
        bool symYPointIsCentered = symmetryPositions.y % 2;
        int symmetryFlippedY = symmetryYPoint + (symmetryYPoint - position.y) - (symYPointIsCentered ? 0 : 1);
        SetPixel(XY{position.x, symmetryFlippedY}, color, symmetry | 0b1);
    }
}

void MainEditor::DrawLine(XY from, XY to, uint32_t color) {
    rasterizeLine(from, to, [&](XY a)->void {
        SetPixel(a, color);
        });
}

void MainEditor::trySaveImage()
{
    lastWasSaveAs = false;
    if (!lastConfirmedSave) {
        trySaveAsImage();
    }
    else {
        trySaveWithExporter(lastConfirmedSavePath, lastConfirmedExporter);
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
        result = exporter->exportData(name, flat);
        delete flat;
    }

    if (result) {
        lastConfirmedSave = true;
        lastConfirmedSavePath = name;
        lastConfirmedExporter = exporter;
        changesSinceLastSave = false;
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
    lastWasSaveAs = true;
    std::vector<std::pair<std::string, std::string>> formats;
    for (auto f : g_fileExporters) {
        formats.push_back({ f->extension(), f->name()});
    }
    platformTrySaveOtherFile(this, formats, "save image", EVENT_MAINEDITOR_SAVEFILE);
}

void MainEditor::recenterCanvas()
{
    canvasCenterPoint = XY{
        (g_windowW / 2) - (texW*scale)/2,
        (g_windowH / 2) - (texH*scale)/2
    };
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
                UndoStackResizeLayerElement* resizeLayerData = (UndoStackResizeLayerElement*)l.extdata4;
                for (int x = 0; x < layers.size(); x++) {
                    free(resizeLayerData[x].oldData);
                }
                delete resizeLayerData;
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
    changesSinceLastSave = true;
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
                    free(resizeLayerData[x].oldData);
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
                //remove layer from list
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
                UndoStackResizeLayerElement* resizeLayerData = (UndoStackResizeLayerElement*)l.extdata4;
                for (int x = 0; x < layers.size(); x++) {
                    //memcpy(layers[x]->pixelData, resizeLayerData[x].oldData, resizeLayerData[x].oldW * resizeLayerData[x].oldH * 4);
                    uint8_t* oldData = layers[x]->pixelData;
                    XY oldDimensions = XY{ layers[x]->w, layers[x]->h };
                    layers[x]->pixelData = resizeLayerData[x].oldData;
                    layers[x]->w = resizeLayerData[x].oldDimensions.x;
                    layers[x]->h = resizeLayerData[x].oldDimensions.y;
                    layers[x]->layerDirty = true;
                    resizeLayerData[x].oldData = oldData;
                    resizeLayerData[x].oldDimensions = oldDimensions;
                }
                texW = layers[0]->w;
                texH = layers[0]->h;
                XY td = XY{ l.extdata, l.extdata2 };
                l.extdata = tileDimensions.x;
                l.extdata2 = tileDimensions.y;
                tileDimensions = td;
                break;
        }
        redoStack.push_back(l);
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
            layers.push_back(l.targetlayer);
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
            UndoStackResizeLayerElement* resizeLayerData = (UndoStackResizeLayerElement*)l.extdata4;
            for (int x = 0; x < layers.size(); x++) {
                //memcpy(layers[x]->pixelData, resizeLayerData[x].oldData, resizeLayerData[x].oldW * resizeLayerData[x].oldH * 4);
                uint8_t* oldData = layers[x]->pixelData;
                XY oldDimensions = XY{ layers[x]->w, layers[x]->h };
                layers[x]->pixelData = resizeLayerData[x].oldData;
                layers[x]->w = resizeLayerData[x].oldDimensions.x;
                layers[x]->h = resizeLayerData[x].oldDimensions.y;
                layers[x]->layerDirty = true;
                resizeLayerData[x].oldData = oldData;
                resizeLayerData[x].oldDimensions = oldDimensions;
            }
            texW = layers[0]->w;
            texH = layers[0]->h;
            XY td = XY{ l.extdata, l.extdata2 };
            l.extdata = tileDimensions.x;
            l.extdata2 = tileDimensions.y;
            tileDimensions = td;
            break;
        }
        undoStack.push_back(l);
    }
}

Layer* MainEditor::newLayer()
{
    Layer* nl = new Layer(texW, texH);
    nl->name = std::format("New Layer {}", layers.size()+1);
    layers.push_back(nl);
    switchActiveLayer(layers.size() - 1);

    addToUndoStack(UndoStackElement{nl, UNDOSTACK_CREATE_LAYER});
    return nl;
}

void MainEditor::deleteLayer(int index) {
    if (layers.size() <= 1) {
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

void MainEditor::setActiveColor(uint32_t col, bool animate)
{
    colorPicker->setMainEditorColorRGB(col);
    if (animate) {
        colorPickTimer.start();
    }
}

uint32_t MainEditor::getActiveColor()
{
    return pickedColor;
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
    memcpy(bottomLayer->pixelData, merged->pixelData, bottomLayer->w * bottomLayer->h * 4);
    bottomLayer->layerDirty = true;
    delete merged;
    
}

void MainEditor::duplicateLayer(int index)
{
    Layer* currentLayer = layers[index];
    Layer* newL = newLayer();
    memcpy(newL->pixelData, currentLayer->pixelData, texW * texH * 4);
    newL->name = "Copy:" + currentLayer->name;
    newL->layerDirty = true;
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

void MainEditor::layer_swapLayerRGBtoBGR()
{
    commitStateToCurrentLayer();
    Layer* clayer = getCurrentLayer();
    uint8_t* convData = (uint8_t*)malloc(clayer->w * clayer->h * 4);
    SDL_ConvertPixels(clayer->w, clayer->h, SDL_PIXELFORMAT_ARGB8888, clayer->pixelData, clayer->w * 4, SDL_PIXELFORMAT_ABGR8888, convData, clayer->w * 4);
    free(clayer->pixelData);
    clayer->pixelData = convData;
    clayer->layerDirty = true;
}

void MainEditor::layer_setOpacity(uint8_t opacity) {
    Layer* clayer = getCurrentLayer();
    addToUndoStack(UndoStackElement{ clayer, UNDOSTACK_SET_OPACITY, clayer->lastConfirmedlayerAlpha, opacity });
    //printf("added to undo stack: %i, %i\n", clayer->lastConfirmedlayerAlpha, opacity);
    clayer->layerAlpha = opacity;
    clayer->lastConfirmedlayerAlpha = clayer->layerAlpha;
}

void MainEditor::layer_promptRename()
{
    PopupTextBox* ninput = new PopupTextBox("Rename layer", "Enter the new layer name:");
    ninput->setCallbackListener(EVENT_MAINEDITOR_SET_CURRENT_LAYER_NAME, this);
    ninput->tbox->text = this->getCurrentLayer()->name;
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
    Layer* ret = new Layer(texW, texH);
    int x = 0;
    uint32_t* retppx = (uint32_t*)ret->pixelData;
    for (Layer*& l : layers) {
        if (l->hidden) {
            continue;
        }
        uint32_t* ppx = (uint32_t*)l->pixelData;
        if (x++ == 0) {
            if (l->layerAlpha == 255) {
                memcpy(ret->pixelData, l->pixelData, l->w * l->h * 4);
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
    return ret;
}

Layer* MainEditor::mergeLayers(Layer* bottom, Layer* top)
{
    Layer* ret = new Layer(bottom->w, bottom->h);

    memcpy(ret->pixelData, bottom->pixelData, bottom->w * bottom->h * 4);

    uint32_t* ppx = (uint32_t*)top->pixelData;
    uint32_t* retppx = (uint32_t*)ret->pixelData;
    for (uint64_t p = 0; p < ret->w * ret->h; p++) {
        uint32_t pixel = ppx[p];
        uint32_t srcPixel = retppx[p];
        pixel = modAlpha(pixel, (uint8_t)(((pixel>>24)/255.0f) * (top->layerAlpha / 255.0f) * 255));
        retppx[p] = alphaBlend(srcPixel, pixel);
    }

    return ret;
}

void MainEditor::resizeAllLayersFromCommand(XY size, bool byTile)
{
    if (byTile) {
        if (tileDimensions.x == size.x && tileDimensions.y == size.y) {
            g_addNotification(ErrorNotification("Error", "Tile size must be different to resize."));
            return;
        }
    }
    else {
        if (texW == size.x && texH == size.y) {
            g_addNotification(ErrorNotification("Error", "Size must be different to resize."));
            return;
        }
    }

    UndoStackResizeLayerElement* layerResizeData = new UndoStackResizeLayerElement[layers.size()];
    for (int x = 0; x < layers.size(); x++) {
        layerResizeData[x].oldDimensions = XY{ layers[x]->w, layers[x]->h };
        if (byTile) {
            layerResizeData[x].oldData = layers[x]->resizeByTileSizes(tileDimensions, size);
        }
        else {
            layerResizeData[x].oldData = layers[x]->resize(size);
        }
        layers[x]->layerDirty = true;
    }
    texW = layers[0]->w;
    texH = layers[0]->h;
    
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
        layerResizeData[x].oldData = layers[x]->resizeByTileCount(tileDimensions, size);
        layers[x]->layerDirty = true;
    }
    texW = layers[0]->w;
    texH = layers[0]->h;

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
        g_addNotification(ErrorNotification("Error", "Invalid scale."));
        return;
    }
    else if (downscale && (texW % scale.x != 0 || texH % scale.y != 0)) {
        g_addNotification(ErrorNotification("Error", "Dimensions not divisible."));
        return;
    }

    UndoStackResizeLayerElement* layerResizeData = new UndoStackResizeLayerElement[layers.size()];
    for (int x = 0; x < layers.size(); x++) {
        layerResizeData[x].oldDimensions = XY{ layers[x]->w, layers[x]->h };
        layerResizeData[x].oldData = downscale ? layers[x]->integerDownscale(scale) : layers[x]->integerScale(scale);
        layers[x]->layerDirty = true;
    }
    texW = layers[0]->w;
    texH = layers[0]->h;

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
        g_addNotification(ErrorNotification("Error", "?????"));
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

                uint32_t* ppx = (uint32_t*)l->pixelData;
                uint32_t* outpx = (uint32_t*)newLayer->pixelData;
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
            g_addNotification(ErrorNotification("Error", "Too many colors in current session"));
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
        g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
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
    printf("_removeComment NOT FOUND\n");
    //shitass workaround tell noone thanks
    //@hirano185 hey girlie check this out!
    return { {0,0}, "\1" };
}

void MainEditor::layer_replaceColor(uint32_t from, uint32_t to)
{
    //commitStateToCurrentLayer();
    getCurrentLayer()->replaceColor(from, to, isolateEnabled ? isolateRect : SDL_Rect{-1,-1,-1,-1});
}
