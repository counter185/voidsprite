#pragma once
#include "BaseScreen.h"
#include "Canvas.h"

class ScreenNonogramPlayer : public BaseScreen
{
private:
	u8* puzzleData;
	u8* puzzlePlayerInputData;
	XY mousePoint = {0,0};

	bool mmiddleDown = false;
	bool mleftDown = false;
	bool mrightDown = false;
protected:
	int toolActive = 0;	//1: fill, 2: x
	std::vector<std::vector<int>> rowHints;
	std::vector<std::vector<int>> colHints;

public:
	Canvas c;

	static void StartDebugGame();

	ScreenNonogramPlayer(u8* puzzleData, int w, int h);
	~ScreenNonogramPlayer();

	void render() override;
	void takeInput(SDL_Event evt) override;
	std::string getName() override { return "Nonogram Player"; }

	void GenHints();
	void RenderBackground();
	void UpdateMousePoint(XY mousePoint) { this->mousePoint = mousePoint; }
	void PlaceFillAt(XY pos);
	void PlaceXAt(XY pos);
};

