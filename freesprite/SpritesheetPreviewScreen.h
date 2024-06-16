#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "EventCallbackListener.h"
#include "DrawableManager.h"
#include "UILabel.h"
#include "UITextField.h"
#include "EditorSpritesheetPreview.h"
#include "ScrollingView.h"

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
	ScrollingView* spriteView;

	SpritesheetPreviewScreen(MainEditor* parent) {
		caller = parent;

		previewWx = new EditorSpritesheetPreview(this);

		msPerSpriteLabel = new UILabel();
		msPerSpriteLabel->text = "MS per sprite";
		wxsManager.addDrawable(msPerSpriteLabel);

		textfieldMSPerSprite = new UITextField();
		textfieldMSPerSprite->text = std::to_string(msPerSprite);
		textfieldMSPerSprite->numeric = true;
		textfieldMSPerSprite->wxWidth = 150;
		textfieldMSPerSprite->setCallbackListener(EVENT_SPRITEPREVIEW_SET_SPRITE_TICK, this);
		wxsManager.addDrawable(textfieldMSPerSprite);

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
	~SpritesheetPreviewScreen();

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;
	BaseScreen* isSubscreenOf() override;

	void eventTextInput(int evt_id, std::string data) override;

	std::string getName() override { return "Preview sprites"; }

	void drawPreview(XY at, int which = -1);
	void drawBackground();
};

