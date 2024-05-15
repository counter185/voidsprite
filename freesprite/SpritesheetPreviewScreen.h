#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "EventCallbackListener.h"
#include "DrawableManager.h"
#include "UILabel.h"
#include "UITextField.h"
#include "EditorSpritesheetPreview.h"

class SpritesheetPreviewScreen : public BaseScreen, public EventCallbackListener
{
public:
	MainEditor* caller;
	EditorSpritesheetPreview* previewWx;

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

		previewWx = new EditorSpritesheetPreview(this);

		msPerSpriteLabel = new UILabel();
		msPerSpriteLabel->text = "MS per sprite";
		wxsManager.addDrawable(msPerSpriteLabel);

		textfieldMSPerSprite = new UITextField();
		textfieldMSPerSprite->text = std::to_string(msPerSprite);
		textfieldMSPerSprite->numeric = true;
		textfieldMSPerSprite->setCallbackListener(EVENT_SPRITEPREVIEW_SET_SPRITE_TICK, this);
		wxsManager.addDrawable(textfieldMSPerSprite);
	}
	~SpritesheetPreviewScreen();

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;
	BaseScreen* isSubscreenOf() override;

	void eventTextInput(int evt_id, std::string data) override;

	std::string getName() override { return "Preview sprites"; }

	void drawPreview(XY at);
};

