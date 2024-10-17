#include "PanelTilemapPreview.h"
#include "TilemapPreviewScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "UILabel.h"

PanelTilemapPreview::PanelTilemapPreview(TilemapPreviewScreen* parent) {
	caller = parent;

    wxWidth = 400;
    wxHeight = 200;
    position = { 20,50 };

    UILabel* ll = new UILabel();
    ll->text = "Tileset preview";
    ll->position = { 5,5 };
    subWidgets.addDrawable(ll);
    
    ll = new UILabel();
    ll->text = "Selected tile: [TAB] to switch";
    ll->position = { 15,60 };
    subWidgets.addDrawable(ll);
}

void PanelTilemapPreview::render(XY position)
{
    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };

    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, focused ? 0xa0 : 0x90 };
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, focused ? 0xa0 : 0x90 };
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    //current tile panel
    //SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xa0);
    SDL_Rect panelRect = { 0,40, ixmax(400, 30 + caller->caller->tileDimensions.x * caller->canvas.scale * 2), ixmax(200, 110 + caller->caller->tileDimensions.y * caller->canvas.scale * 2) };
    wxHeight = panelRect.h;
    wxWidth = panelRect.w;
    //SDL_RenderFillRect(g_rd, &panelRect);

    XY callerTileDims = caller->caller->getPaddedTileDimensions();

    SDL_Rect tileDraw = {
        position.x + 15,
        position.y + 90,
        callerTileDims.x * caller->canvas.scale * 2,
        callerTileDims.y * caller->canvas.scale * 2
    };

    SDL_Rect tileClip = caller->caller->getPaddedTilePosAndDimensions(caller->pickedTile); /*{
        caller->pickedTile.x * caller->caller->tileDimensions.x,
        caller->pickedTile.y * caller->caller->tileDimensions.y,
        caller->caller->tileDimensions.x,
        caller->caller->tileDimensions.y
    };*/

    for (Layer* l : caller->caller->layers) {
        SDL_RenderCopy(g_rd, l->tex, &tileClip, &tileDraw);
    }
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x90);
    SDL_RenderDrawRect(g_rd, &tileDraw);

    DraggablePanel::render(position);
}