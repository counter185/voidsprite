#pragma once
#include "globals.h"
#include "DrawableManager.h"

#define LALT_TO_SUMMON_NAVBAR if (evt.type == SDL_KEYDOWN && evt.key.scancode == SDL_SCANCODE_LALT) { if (!navbar->focused) wxsManager.forceFocusOn(navbar); else wxsManager.forceUnfocus(); return; }

class BaseScreen
{
protected:
    int callback_id = -1;
    EventCallbackListener* callback = NULL;
    DrawableManager wxsManager;
public:
    virtual ~BaseScreen() = default;

    virtual void render() {
        wxsManager.renderAll();
    }
    virtual void takeInput(SDL_Event evt) 
    {
        if (evt.type == SDL_QUIT) {
            g_closeScreen(this);
            return;
        }
        DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);
        DrawableManager::processInputEventInMultiple({ wxsManager }, evt);
    }
    virtual void tick() {}
    virtual BaseScreen* isSubscreenOf() { return NULL; }
    virtual bool takesTouchEvents() { return false; }
    virtual void onReturnToScreen() {}

    virtual std::string getName() { return "Base screen"; }
    virtual std::string getRPCString() { return getName(); }

    void drawBottomBar() {
        SDL_Rect r = { 0, g_windowH - 30, g_windowW, 30 };
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xb0);
        SDL_RenderFillRect(g_rd, &r);
    }

    void setCallbackListener(int evt_id, EventCallbackListener* callback) {
        this->callback = callback;
        this->callback_id = evt_id;
    }
};

