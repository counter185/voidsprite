#include "SpritesheetPreviewScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "UILabel.h"
#include "UITextField.h"
#include "EditorSpritesheetPreview.h"
#include "ScrollingPanel.h"
#include "PanelSpritesheetPreview.h"
#include "UIButton.h"
#include "TilemapPreviewScreen.h"
#include "io/io_avif.h"
#include "Notification.h"
#include "PopupSaveAnimation.h"

#define N_BUTTONS_ADDED_TO_TIMELINE 3
#define MIN_DISTANCE_BETWEEN_TIMELINE_SPRITES 120

SpritesheetPreviewScreen::SpritesheetPreviewScreen(MainEditor* parent) {
    caller = parent;
    canvas.scale = parent->canvas.scale;
    canvas.dimensions = parent->canvas.dimensions;
    canvas.recenter();

    previewWx = new EditorSpritesheetPreview(this);
    caller->addWidget(previewWx);

    panel = new PanelSpritesheetPreview(this);
    panel->position = { 5, 40 };
    panel->position.x = g_windowW - 10 - panel->wxWidth;
    panel->anchor = { 1,0 };
    wxsManager.addDrawable(panel);

    navbar = new ScreenWideNavBar(this,
        {
            {
                SDL_SCANCODE_F,
                makeNavbarSection(
                    TL("vsp.nav.file"), g_iconNavbarTabFile,
                    {
                        {SDL_SCANCODE_E, { "Export animation...", [this]() { promptSaveAnimation(); }}},
                        {SDL_SCANCODE_C, { "Close", [this]() { this->closeNextTick = true; }}},
                    }
                )
            },
            {
                SDL_SCANCODE_E,
                makeNavbarSection(
                    TL("vsp.nav.edit"), g_iconNavbarTabEdit,
                    {
                        {SDL_SCANCODE_C, { "Clear timeline", [this]() { sprites.clear(); genTimelineButtons(); }}},
                    }
                )
            },
        }, { SDL_SCANCODE_F, SDL_SCANCODE_E });
    wxsManager.addDrawable(navbar);


    spriteView = new ScrollingPanel();
    spriteView->scrollHorizontally = true;
    spriteView->scrollVertically = false;
    spriteView->wxWidth = 200;
    spriteView->wxHeight = 200;
    spriteView->position = XY{ 0, 300 };
    wxsManager.addDrawable(spriteView);

    /*UILabel* spriteScrollerLabel = new UILabel();
    spriteScrollerLabel->text = "Timeline";
    spriteScrollerLabel->position = XY{ 2, 2 };
    spriteView->tabButtons.addDrawable(spriteScrollerLabel);*/

}

SpritesheetPreviewScreen::~SpritesheetPreviewScreen() {
    wxsManager.freeAllDrawables();
    caller->removeWidget(previewWx);
    //delete previewWx;
}

void SpritesheetPreviewScreen::render()
{
    renderWithBlurPanelsIfEnabled([this]() { RenderCanvas(); });

    //int rightPanelWidth = ixmax(300, canvas.scale * tileSize.x);
    //drawPreview(XY{ rightPanelRect.x + 10, 100 });

    BaseScreen::render();

    //sprite timeline
    XY spriteListOrigin = xyAdd(spriteView->position, spriteView->scrollOffset);

    if (sprites.size() > 0) {
        for (int x = 0; x < sprites.size(); x++) {
            int elementWidth = ixmax(MIN_DISTANCE_BETWEEN_TIMELINE_SPRITES, caller->ssne.tileDimensions.x * timelineSpriteScale);
            XY spritePos = xyAdd(spriteListOrigin, XY{ x * elementWidth + x * 5 , 50 });
            drawPreview(spritePos, timelineSpriteScale, x);
            SDL_Rect spriteArea = {
                spritePos.x,
                spritePos.y,
                caller->ssne.tileDimensions.x * timelineSpriteScale,
                caller->ssne.tileDimensions.y * timelineSpriteScale
            };
            SDL_SetRenderDrawColor(g_rd, spritesProgress == x ? 0 : 255, 255, spritesProgress == x ? 0 : 255, 0x80);
            SDL_RenderDrawRect(g_rd, &spriteArea);
            g_fnt->RenderString(std::to_string(x), spritePos.x, spritePos.y - 24);
        }
    }
    else {
        g_fnt->RenderString("Click on sprites to add them to the timeline...", 20, spriteView->position.y + 60);
    }

    static std::string tlTimeline = TL("vsp.spritesheetpreview.timeline");
    g_fnt->RenderString(tlTimeline, 2, spriteView->position.y + 2);

}

void SpritesheetPreviewScreen::RenderCanvas()
{
    drawBackground();

    canvas.dimensions = caller->canvas.dimensions;
    SDL_Rect canvasRenderRect = canvas.getCanvasOnScreenRect();
    for (Layer*& l : caller->getLayerStack()) {
        if (!l->hidden) {
            l->render(canvasRenderRect, l->layerAlpha);
        }
    }

    // lines between tiles
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    canvas.drawTileGrid(caller->ssne.tileDimensions);

    //canvas border
    canvas.drawCanvasOutline(5, SDL_Color{ 255,255,255,0x80 });

    XY tileSize = caller->ssne.tileDimensions;

    //sprite rects
    std::map<u64, int> drawnRects;
    int i = 0;
    for (XY& sprite : sprites) {
        SDL_Rect spriteArea = canvas.getTileScreenRectAt(sprite, tileSize);

        if (spritesProgress == i) {
            SDL_SetRenderDrawColor(g_rd, 0, 255, 0, 0xa0);
        }
        else {
            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
        }
        SDL_RenderDrawRect(g_rd, &spriteArea);
        u64 key = encodeXY(sprite);
        g_fnt->RenderString(std::to_string(i++), spriteArea.x, spriteArea.y + (25 * drawnRects[key]++), SDL_Color{ 255,255,255,0xa0 });
    }
}

void SpritesheetPreviewScreen::tick()
{
    if (closeNextTick || caller->ssne.tileDimensions.x == 0 || caller->ssne.tileDimensions.y == 0) {
        g_closeScreen(this);
        return;
    }

    int rightPanelWidth = ixmax(300, canvas.scale * caller->ssne.tileDimensions.x);
    XY origin = { g_windowW - rightPanelWidth, 20 };

    timelineSpriteScale = calcMaxTimelineScale();

    spriteView->wxWidth = g_windowW;
    spriteView->wxHeight = 30 + caller->ssne.tileDimensions.y * timelineSpriteScale + 60;
    spriteView->position = { 0, g_windowH - spriteView->wxHeight };

    canvas.lockToScreenBounds(0, 0, spriteView->wxHeight, rightPanelWidth);

    for (int x = 0; x < spriteView->subWidgets.drawablesList.size(); x++) {
        int timelineIndex = x / N_BUTTONS_ADDED_TO_TIMELINE;
        int timelineAction = x % N_BUTTONS_ADDED_TO_TIMELINE;

        int elementWidth = ixmax(MIN_DISTANCE_BETWEEN_TIMELINE_SPRITES, caller->ssne.tileDimensions.x * timelineSpriteScale);
        spriteView->subWidgets.drawablesList[x]->position = XY{ timelineIndex * elementWidth + timelineIndex * 5 + 30 + (24 * timelineAction), 24 };
    }
}

void SpritesheetPreviewScreen::defaultInputAction(SDL_Event evt)
{
    if (!canvas.takeInput(evt)) {
        switch (evt.type) {
            case SDL_FINGERDOWN:
                lastTapPosition = { (int)(g_windowW * evt.tfinger.x), (int)(g_windowH * evt.tfinger.y) };
                break;
            case SDL_FINGERUP:
            {
                XY touchPos = { g_windowW * evt.tfinger.x, g_windowH * evt.tfinger.y };
                if (xyDistance(touchPos, lastTapPosition) <= 10 && lastTapTimer.started && lastTapTimer.elapsedTime() < 1000) {
                    selectTileAt(touchPos);
                }
                lastTapPosition = touchPos;
                lastTapTimer.start();
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                if (evt.button.button == SDL_BUTTON_LEFT) {
                    selectTileAt({(int)evt.button.x, (int)evt.button.y});
                }
                break;
        }
    }
}

BaseScreen* SpritesheetPreviewScreen::isSubscreenOf() { 
    return caller; 
}

void SpritesheetPreviewScreen::eventButtonPressed(int evt_id)
{
    int timelineIndex = evt_id / N_BUTTONS_ADDED_TO_TIMELINE;
    int timelineAction = evt_id % N_BUTTONS_ADDED_TO_TIMELINE;
    if (timelineIndex < sprites.size()) {
        if (timelineAction == 0) {
            sprites.erase(sprites.begin() + timelineIndex);
            popTimelineButton();
        }
        else if (timelineAction == 1) {
            //move sprite at index timelineIndex one place to the left
            if (timelineIndex > 0) {
                XY temp = sprites[timelineIndex];
                sprites[timelineIndex] = sprites[timelineIndex - 1];
                sprites[timelineIndex - 1] = temp;
            }
        }
        else if (timelineAction == 2) {
            //move sprite at index timelineIndex one place to the right
            if (timelineIndex < sprites.size() - 1) {
                XY temp = sprites[timelineIndex];
                sprites[timelineIndex] = sprites[timelineIndex + 1];
                sprites[timelineIndex + 1] = temp;
            }
        }
        
    }
}

void SpritesheetPreviewScreen::drawPreview(XY at, int scale, int which)
{
    if (!sprites.empty() && msPerSprite > 0) {
        spritesProgress = (SDL_GetTicks64() / msPerSprite) % sprites.size();
    }

    if (!sprites.empty()) {
        XY callerPaddedTileSize = caller->getPaddedTileDimensions();
        SDL_Rect spriteDrawArea = { at.x, at.y,
            callerPaddedTileSize.x * scale,
            callerPaddedTileSize.y * scale
        };
        if (spritesProgress >= sprites.size()) {
            spritesProgress = 0;
        }
        XY currentSprite = sprites[which == -1 ? spritesProgress : which];

        SDL_Rect layersClipArea = caller->getPaddedTilePosAndDimensions(currentSprite);

        for (Layer*& l : caller->getLayerStack()) {
            if (!l->hidden) {
                l->render(spriteDrawArea, layersClipArea, l->layerAlpha);
                //SDL_RenderCopy(g_rd, l->renderData[g_rd].tex, &layersClipArea, &spriteDrawArea);
            }
        }
    }
}

void SpritesheetPreviewScreen::drawBackground()
{
    TilemapPreviewScreen::drawBackground();
    if (g_config.animatedBackground) {
        uint64_t now = g_config.animatedBackground >= 3 ? 0 : SDL_GetTicks64();
        uint64_t progress = now % 120000;
        for (int y = -(1.0 - progress / 120000.0) * g_windowH; y < g_windowH; y += 50) {
            if (y >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x22);
                SDL_RenderDrawLine(g_rd, 0, y, g_windowW, y);
            }
        }

        for (int x = -(1.0 - (now % 100000) / 100000.0) * g_windowW; x < g_windowW; x += 30) {
            if (x >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x19);
                SDL_RenderDrawLine(g_rd, x, 0, x, g_windowH);
            }
        }
    }
}

void SpritesheetPreviewScreen::genTimelineButtons()
{
    spriteView->subWidgets.freeAllDrawables();
    for (int x = 0; x < sprites.size(); x++) {
        addTimelineButton();
    }
}

void SpritesheetPreviewScreen::addTimelineButton()
{
    int newIndex = spriteView->subWidgets.drawablesList.size() / N_BUTTONS_ADDED_TO_TIMELINE;
    UIButton* deleteButton = new UIButton();
    deleteButton->text = "-";
    deleteButton->wxWidth = deleteButton->wxHeight = 24;
    deleteButton->setCallbackListener(newIndex * N_BUTTONS_ADDED_TO_TIMELINE, this);
    spriteView->subWidgets.addDrawable(deleteButton);

    UIButton* moveLeftButton = new UIButton();
    moveLeftButton->text = "<";
    moveLeftButton->wxWidth = moveLeftButton->wxHeight = 24;
    moveLeftButton->setCallbackListener(newIndex * N_BUTTONS_ADDED_TO_TIMELINE + 1, this);
    spriteView->subWidgets.addDrawable(moveLeftButton);

    UIButton* moveRightButton = new UIButton();
    moveRightButton->text = ">";
    moveRightButton->wxWidth = moveRightButton->wxHeight = 24;
    moveRightButton->setCallbackListener(newIndex * N_BUTTONS_ADDED_TO_TIMELINE + 2, this);
    spriteView->subWidgets.addDrawable(moveRightButton);
}

void SpritesheetPreviewScreen::popTimelineButton()
{
    for (int x = 0; x < N_BUTTONS_ADDED_TO_TIMELINE; x++) {
        Drawable* d = spriteView->subWidgets.drawablesList.back();
        spriteView->subWidgets.removeDrawable(d);
    }
}

int SpritesheetPreviewScreen::calcMaxTimelineScale() {
    XY tileSize = caller->ssne.tileDimensions;
    int scale = canvas.scale;
    int maxH = g_windowH/5;
    while (tileSize.y * scale > maxH && scale > 1) {
        scale--;
    }
    return scale;
}

void SpritesheetPreviewScreen::selectTileAt(XY pos)
{
    if (caller->ssne.tileDimensions.x != 0 && caller->ssne.tileDimensions.y != 0
        && canvas.pointInCanvasBounds(canvas.screenPointToCanvasPoint(pos)))
    {
        XY tile = canvas.getTilePosAt(pos, caller->ssne.tileDimensions);
        sprites.push_back(tile);
        addTimelineButton();
    }
}

void SpritesheetPreviewScreen::promptSaveAnimation()
{
    PopupSaveAnimation* popup = new PopupSaveAnimation("Export animation", "Export the current spritesheet preview as an animation?");
    popup->msPerFrame = msPerSprite;
    popup->quality = 100;
    popup->onConfirmCallback = [this](PopupSaveAnimation* p, PlatformNativePathString path) {
        auto rec = p->recorder;
        auto msPerFrame = p->msPerFrame;
        auto quality = p->quality;
        auto scale = p->scale;
        auto repeat = p->repeatTimes;
        g_startNewOperation([this, path, rec, msPerFrame, quality, scale, repeat]() {
            VideoEncoder* enc = rec.createFn();
            enc->startRecording(path, msPerFrame, quality);
            saveCurrentAnimation(enc, scale, repeat);
            enc->stopRecording();
            delete enc;
        });
    };
    g_addPopup(popup);
}

void SpritesheetPreviewScreen::saveCurrentAnimation(VideoEncoder* encoder, int scale, int repeat)
{
    Layer* flat = caller->flattenImage();
    if (flat != NULL) {
        for (int i = 0; i < repeat; i++) {
            for (auto& frame : sprites) {
                SDL_Rect sourceRect = caller->getPaddedTilePosAndDimensions(frame);
                Layer* t = flat->trim(sourceRect);
                if (scale > 1) {
                    Layer* tt = t->copyAllVariantsScaled({ t->w * scale, t->h * scale });
                    delete t;
                    t = tt;
                }
                encoder->submitFrame(t);
                delete t;
            }
        }
        delete flat;
        g_addNotificationFromThread(Notification("Animation saved", frmt("Saved {} frames", sprites.size() * repeat)));
    }
    else {
        g_addNotification(NOTIF_MALLOC_FAIL);
    }
}
