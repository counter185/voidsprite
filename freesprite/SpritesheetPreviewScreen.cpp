#include "SpritesheetPreviewScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "UILabel.h"
#include "UITextField.h"
#include "EditorSpritesheetPreview.h"
#include "ScrollingView.h"
#include "PanelSpritesheetPreview.h"

#define N_BUTTONS_ADDED_TO_TIMELINE 3
#define MIN_DISTANCE_BETWEEN_TIMELINE_SPRITES 120

SpritesheetPreviewScreen::SpritesheetPreviewScreen(MainEditor* parent) {
    caller = parent;
    canvasZoom = parent->scale;

    canvasDrawOrigin = { 60, 60 };

    previewWx = new EditorSpritesheetPreview(this);

    panel = new PanelSpritesheetPreview(this);
    panel->position = { 5, 40 };
    wxsManager.addDrawable(panel);

    navbar = new ScreenWideNavBar<SpritesheetPreviewScreen*>(this,
        {
            {
                SDLK_f,
                {
                    "File",
                    {},
                    {
                        {SDLK_c, { "Close",
                                [](SpritesheetPreviewScreen* screen) {
                                    screen->closeNextTick = true;
                                }
                            }
                        },
                    },
                    g_iconNavbarTabFile
                }
            },
        }, { SDLK_f });
    wxsManager.addDrawable(navbar);


    spriteView = new ScrollingView();
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
    delete previewWx;
    caller->spritesheetPreview = NULL;
}

void SpritesheetPreviewScreen::render()
{
    drawBackground();

    SDL_Rect canvasRenderRect = { canvasDrawOrigin.x, canvasDrawOrigin.y, caller->texW * canvasZoom, caller->texH * canvasZoom };
    for (Layer*& l : caller->layers) {
        SDL_RenderCopy(g_rd, l->tex, NULL, &canvasRenderRect);
    }

    if (caller->tileDimensions.x != 0 && caller->tileDimensions.y != 0) {

        int dx = canvasRenderRect.x;
        while (dx < g_windowW && dx < canvasRenderRect.x + canvasRenderRect.w) {
            dx += caller->tileDimensions.x * canvasZoom;
            if (dx >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
                SDL_RenderDrawLine(g_rd, dx, canvasRenderRect.y, dx, canvasRenderRect.y + canvasRenderRect.h);
            }
        }

        int dy = canvasRenderRect.y;
        while (dy < g_windowH && dy < canvasRenderRect.y + canvasRenderRect.h) {
            dy += caller->tileDimensions.y * canvasZoom;
            if (dy >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
                SDL_RenderDrawLine(g_rd, canvasRenderRect.x, dy, canvasRenderRect.x + canvasRenderRect.w, dy);
            }
        }
    }
    
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
    SDL_RenderDrawRect(g_rd, &canvasRenderRect);

    XY tileSize = caller->tileDimensions;

    int i = 0;
    for (XY& sprite : sprites) {
        SDL_Rect spriteArea = {
            canvasDrawOrigin.x + (sprite.x * tileSize.x * canvasZoom),
            canvasDrawOrigin.y + (sprite.y * tileSize.y * canvasZoom),
            tileSize.x * canvasZoom,
            tileSize.y * canvasZoom
        };
        if (spritesProgress == i) {
            SDL_SetRenderDrawColor(g_rd, 0, 255, 0, 0xa0);
        }
        else {
            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
        }
        SDL_RenderDrawRect(g_rd, &spriteArea);
        g_fnt->RenderString(std::to_string(i++), spriteArea.x, spriteArea.y, SDL_Color{255,255,255,0xa0});
    }

    int rightPanelWidth = ixmax(300, canvasZoom * tileSize.x);

    /*SDL_Rect rightPanelRect = {g_windowW - rightPanelWidth, 0, rightPanelWidth, g_windowH};
    SDL_SetRenderDrawColor(g_rd, 0x00, 0x00, 0x00, 0xe0);
    SDL_RenderFillRect(g_rd, &rightPanelRect);*/

    //drawPreview(XY{ rightPanelRect.x + 10, 100 });

    wxsManager.renderAll();

    XY spriteListOrigin = xyAdd(spriteView->position, spriteView->scrollOffset);

    if (sprites.size() > 0) {
        for (int x = 0; x < sprites.size(); x++) {
            int elementWidth = ixmax(MIN_DISTANCE_BETWEEN_TIMELINE_SPRITES, caller->tileDimensions.x * canvasZoom);
            XY spritePos = xyAdd(spriteListOrigin, XY{ x * elementWidth + x * 5 , 50 });
            drawPreview(spritePos, x);
            SDL_Rect spriteArea = {
                spritePos.x,
                spritePos.y,
                caller->tileDimensions.x * canvasZoom,
                caller->tileDimensions.y * canvasZoom
            };
            SDL_SetRenderDrawColor(g_rd, spritesProgress == x ? 0 : 255, 255, spritesProgress == x ? 0 : 255, 0x80);
            SDL_RenderDrawRect(g_rd, &spriteArea);
            g_fnt->RenderString(std::to_string(x), spritePos.x, spritePos.y - 24);
        }
    }
    else {
        g_fnt->RenderString("Click on sprites to add them to the timeline...", 20, spriteView->position.y + 60);
    }

    g_fnt->RenderString("Timeline", 2, spriteView->position.y + 2);

}

void SpritesheetPreviewScreen::tick()
{
    if (closeNextTick || caller->tileDimensions.x == 0 || caller->tileDimensions.y == 0) {
        g_closeScreen(this);
        return;
    }

    panel->position.x = g_windowW - 10 - panel->wxWidth;

    int rightPanelWidth = ixmax(300, canvasZoom * caller->tileDimensions.x);
    XY origin = { g_windowW - rightPanelWidth, 20 };
    //msPerSpriteLabel->position = { origin.x + 5, origin.y + 20 };
    //textfieldMSPerSprite->position = { origin.x + 130, origin.y + 20 };

    spriteView->wxWidth = g_windowW;
    spriteView->wxHeight = 30 + caller->tileDimensions.y * canvasZoom + 60;
    spriteView->position = { 0, g_windowH - spriteView->wxHeight };

    canvasDrawOrigin = XY{
        iclamp(-caller->texW * canvasZoom + 4, canvasDrawOrigin.x, g_windowW - rightPanelWidth - 4),
        iclamp(-caller->texH * canvasZoom + 4, canvasDrawOrigin.y, g_windowH - spriteView->wxHeight - 4)
    };

    for (int x = 0; x < spriteView->subWidgets.drawablesList.size(); x++) {
        int timelineIndex = x / N_BUTTONS_ADDED_TO_TIMELINE;
        int timelineAction = x % N_BUTTONS_ADDED_TO_TIMELINE;

        int elementWidth = ixmax(MIN_DISTANCE_BETWEEN_TIMELINE_SPRITES, caller->tileDimensions.x * canvasZoom);
        spriteView->subWidgets.drawablesList[x]->position = XY{ timelineIndex * elementWidth + timelineIndex * 5 + 30 + (24 * timelineAction), 24 };
    }
}

void SpritesheetPreviewScreen::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }

    LALT_TO_SUMMON_NAVBAR;

    if (!DrawableManager::processInputEventInMultiple({wxsManager}, evt)) {
        switch (evt.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (evt.button.button == SDL_BUTTON_MIDDLE) {
                    scrollingCanvas = true;
                }
                else if (evt.button.button == SDL_BUTTON_LEFT) {
                    if (caller->tileDimensions.x != 0 && caller->tileDimensions.y != 0) {
                        XY pos = xySubtract(XY{ evt.button.x, evt.button.y }, canvasDrawOrigin);
                        if (pos.x >= 0 && pos.y >= 0 && pos.x < (caller->texW * canvasZoom) && pos.y < (caller->texH * canvasZoom)) {
                            pos.x /= caller->tileDimensions.x * canvasZoom;
                            pos.y /= caller->tileDimensions.y * canvasZoom;
                            sprites.push_back(pos);
                            addTimelineButton();
                        }
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (evt.button.button == SDL_BUTTON_MIDDLE) {
                    scrollingCanvas = false;
                }
                break;
            case SDL_MOUSEMOTION:
                if (scrollingCanvas) {
                    canvasDrawOrigin = xyAdd(canvasDrawOrigin, XY{ evt.motion.xrel, evt.motion.yrel });
                }
                break;
            case SDL_MOUSEWHEEL:
                canvasZoom += evt.wheel.y;
                canvasZoom = ixmax(1, canvasZoom);
                break;
        }
    }
}

BaseScreen* SpritesheetPreviewScreen::isSubscreenOf() { 
    return caller; 
}

void SpritesheetPreviewScreen::eventTextInput(int evt_id, std::string data)
{
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

void SpritesheetPreviewScreen::drawPreview(XY at, int which)
{
    if (!sprites.empty() && msPerSprite > 0) {
        spritesProgress = (SDL_GetTicks64() / msPerSprite) % sprites.size();
    }

    if (!sprites.empty()) {
        SDL_Rect spriteDrawArea = { at.x, at.y,
            caller->tileDimensions.x * canvasZoom,
            caller->tileDimensions.y * canvasZoom
        };
        if (spritesProgress >= sprites.size()) {
            spritesProgress = 0;
        }
        XY currentSprite = sprites[which == -1 ? spritesProgress : which];

        SDL_Rect layersClipArea = {
            currentSprite.x * caller->tileDimensions.x,
            currentSprite.y * caller->tileDimensions.y,
            caller->tileDimensions.x,
            caller->tileDimensions.y
        };

        for (Layer*& l : caller->layers) {
            SDL_RenderCopy(g_rd, l->tex, &layersClipArea, &spriteDrawArea);
        }
    }
}

void SpritesheetPreviewScreen::drawBackground()
{
    if (g_config.animatedBackground) {
        uint64_t now = SDL_GetTicks64();
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
