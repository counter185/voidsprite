#pragma once
#include "BaseScreen.h"
#include "DrawableManager.h"

class TilemapPreviewScreen : public BaseScreen
{
public:
	MainEditor* caller;

	XY tilemapDrawPoint = XY{ 0,0 };

	XY** tilemap = NULL;
	XY tilemapDimensions = XY{-1,-1};
	int tilemapScale = 1;
	bool scrollingTilemap = false;
	bool mouseLeftingTilemap = false;

	XY pickedTile = XY{ 0,0 };
	XY hoveredTilePosition = XY{ 0,0 };

	DrawableManager wxsManager;

	bool tileSelectOpen = false;
	XY tileSelectOffset = XY{ 0,0 };
	XY tileSelectHoveredTile = XY{ 0,0 };
	int tileSelectScale = 1;
	Timer64 tileSelectTimer;

	TilemapPreviewScreen(MainEditor* parent);

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;
	BaseScreen* isSubscreenOf() override;

	std::string getName() override { return "Preview tiles"; }

	void resizeTilemap(int w, int h);
	void drawBackground();
	void recenterTilemap();
	void recenterTilePicker();
};

