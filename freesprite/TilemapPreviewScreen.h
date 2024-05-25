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

	TilemapPreviewScreen(MainEditor* parent) {
		caller = parent;
		resizeTilemap(32, 32);
	}

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;
	BaseScreen* isSubscreenOf() override;

	std::string getName() override { return "Preview tiles"; }

	void resizeTilemap(int w, int h);
};

