#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "EventCallbackListener.h"
#include "DrawableManager.h"

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

	SpritesheetPreviewScreen(MainEditor* parent);
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

