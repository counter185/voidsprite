#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "EventCallbackListener.h"
#include "maineditor.h"
#include "DrawableManager.h"
#include "UILabel.h"

class SpritesheetPreviewScreen : public BaseScreen, public EventCallbackListener
{
private:
	MainEditor* caller;
public:
	DrawableManager wxsManager;
	std::vector<XY> sprites;
	int spritesProgress = 0;
	int msPerSprite = 128;


	XY canvasDrawOrigin = {0,0};
	int canvasZoom = 1;
	bool scrollingCanvas = false;

	UILabel* msPerSpriteLabel;
	UITextField* textfieldMSPerSprite;

	SpritesheetPreviewScreen(MainEditor* parent) {
		caller = parent;
		msPerSpriteLabel = new UILabel();
		msPerSpriteLabel->text = "MS per sprite";
		wxsManager.addDrawable(msPerSpriteLabel);

		textfieldMSPerSprite = new UITextField();
		textfieldMSPerSprite->text = std::to_string(msPerSprite);
		textfieldMSPerSprite->numeric = true;
		textfieldMSPerSprite->setCallbackListener(EVENT_SPRITEPREVIEW_SET_SPRITE_TICK, this);
		wxsManager.addDrawable(textfieldMSPerSprite);
	}
	~SpritesheetPreviewScreen() {
		wxsManager.freeAllDrawables();
	}

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;
	BaseScreen* isSubscreenOf() override { return caller; }

	void eventTextInput(int evt_id, std::string data) override;

	std::string getName() override { return "Preview sprites"; }
};

