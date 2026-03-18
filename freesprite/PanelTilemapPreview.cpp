#include "PanelTilemapPreview.h"
#include "TilemapPreviewScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "UILabel.h"
#include "UIButton.h"

PanelTilemapPreview::PanelTilemapPreview(TilemapPreviewScreen* parent) {
    caller = parent;

    wxWidth = 400;
    wxHeight = 200;
    position = { 20,50 };

    setupDraggable();
    setupCollapsible();
    addTitleText(TL("vsp.tilemappreview.title"));
    
    UIButton* b = new UIButton(TL("vsp.tilemappreview.selecttile"));
    b->wxWidth = 180;
    b->position = { wxWidth - b->wxWidth - 10, 5 };
    b->onClickCallback = ([this](UIButton*) { caller->toggleTileSelect(); caller->forceUnfocusAll(); });
    wxsTarget().addDrawable(b);

    wxsTarget().addDrawable(new UILabel(TL("vsp.tilemappreview.selectedtile"), {15, 60}));
}

void PanelTilemapPreview::renderAfterBG(XY position)
{
    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };

    //current tile panel
    //SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xa0);
    SDL_Rect panelRect = { 0,40, ixmax(400, 30 + caller->caller->ssne.tileDimensions.x * caller->canvas.scale * 2), ixmax(200, 110 + caller->caller->ssne.tileDimensions.y * caller->canvas.scale * 2) };
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

    SDL_Rect tileClip = caller->caller->getPaddedTilePosAndDimensions(caller->pickedTile); 

    for (Layer* l : caller->caller->getLayerStack()) {
        l->prerender();
        SDL_RenderCopy(g_rd, l->renderData[g_rd].tex, &tileClip, &tileDraw);
    }
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x90);
    SDL_RenderDrawRect(g_rd, &tileDraw);
}