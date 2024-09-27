#pragma once
#include "globals.h"
#include "DrawableManager.h"

#define LALT_TO_SUMMON_NAVBAR if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_LALT) { wxsManager.forceFocusOn(navbar); return; }

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

	virtual std::string getName() { return "Base screen"; }

	void setCallbackListener(int evt_id, EventCallbackListener* callback) {
		this->callback = callback;
		this->callback_id = evt_id;
	}
};

