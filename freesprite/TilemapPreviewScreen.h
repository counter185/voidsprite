#pragma once
#include "BaseScreen.h"
class TilemapPreviewScreen : public BaseScreen
{
public:
	MainEditor* caller;

	TilemapPreviewScreen(MainEditor* parent) {
		caller = parent;
	}

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;
	BaseScreen* isSubscreenOf() override;

	std::string getName() override { return "Preview tiles"; }
};

