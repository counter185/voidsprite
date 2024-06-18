#include "MinecraftSkinPreviewScreen.h"
#include "maineditor.h"

int scaled(int texPosition, int refW, int texW) {
	return ((float)texPosition / refW) * texW;
}

void MinecraftSkinPreviewScreen::renderBox(XY positionOnScreen, XY wholeSkinDimensions, SDL_Rect textureRegion, SDL_Rect textureSpaceRegion, XY targetRegion, int texW, int texH, bool horizontalFlip) {
	SDL_Vertex headBox[4];
	headBox[0].position = { positionOnScreen.x + (float)scaled(textureSpaceRegion.x, wholeSkinDimensions.x, targetRegion.x), positionOnScreen.y + (float)scaled(textureSpaceRegion.y, wholeSkinDimensions.y, targetRegion.y) };
	headBox[0].color = { 255, 255, 255, 255 };
	headBox[0].tex_coord = { (float)scaled(textureRegion.x, 64, texW) / texW, (float)scaled(textureRegion.y, 64 / (texW/texH), texH) / texH};

	headBox[1].position = { positionOnScreen.x + (float)scaled(textureSpaceRegion.x+textureSpaceRegion.w, wholeSkinDimensions.x, targetRegion.x), positionOnScreen.y + (float)scaled(textureSpaceRegion.y, wholeSkinDimensions.y, targetRegion.y) };
	headBox[1].color = { 255, 255, 255, 255 };
	headBox[1].tex_coord = { (float)scaled(textureRegion.x + textureRegion.w, 64, texW) / texW, (float)scaled(textureRegion.y, 64 / (texW / texH), texH) / texH };

	headBox[2].position = { positionOnScreen.x + (float)scaled(textureSpaceRegion.x + textureSpaceRegion.w, wholeSkinDimensions.x, targetRegion.x), positionOnScreen.y + (float)scaled(textureSpaceRegion.y+textureSpaceRegion.h, wholeSkinDimensions.y, targetRegion.y) };
	headBox[2].color = { 255, 255, 255, 255 };
	headBox[2].tex_coord = { (float)scaled(textureRegion.x + textureRegion.w, 64, texW) / texW, (float)scaled(textureRegion.y + textureRegion.h, 64 / (texW / texH), texH) / texH };

	headBox[3].position = { positionOnScreen.x + (float)scaled(textureSpaceRegion.x, wholeSkinDimensions.x, targetRegion.x), positionOnScreen.y + (float)scaled(textureSpaceRegion.y + textureSpaceRegion.h, wholeSkinDimensions.y, targetRegion.y) };
	headBox[3].color = { 255, 255, 255, 255 };
	headBox[3].tex_coord = { (float)scaled(textureRegion.x, 64, texW) / texW, (float)scaled(textureRegion.y + textureRegion.h, 64 / (texW / texH), texH) / texH };

	int indices[] = { 0,1,2,0,2,3 };

	SDL_RenderGeometry(g_rd, caller->layers[0]->tex, headBox, 4, indices, 6);
}

void MinecraftSkinPreviewScreen::render()
{
	int texW = caller->texW;
	int texH = caller->texH;

	XY orthoFrontPosition = { g_windowW / 12, g_windowH / 8 };
	int previewHeight = g_windowH / 3;    //32 texture pixels height

	SDL_Rect headRegion = { 8, 8, 8, 8 };
	SDL_Rect headTxRegion = { 4, 0, 8, 8 };
	renderBox(orthoFrontPosition, XY{ 16, 32 }, headRegion, headTxRegion, XY{previewHeight/2, previewHeight}, texW, texH);

	SDL_Rect hatFrontRegion = { 40, 8, 8, 8 };
	SDL_Rect hatFrontTxRegion = { 4, 0, 8, 8 };
	renderBox(orthoFrontPosition, XY{ 16, 32 }, hatFrontRegion, hatFrontTxRegion, XY{previewHeight/2, previewHeight}, texW, texH);

	SDL_Rect bodyRegion = { 20, 20, 8, 12 };
	SDL_Rect bodyTxRegion = { 4, 8, 8, 12 };
	renderBox(orthoFrontPosition, XY{ 16, 32 }, bodyRegion, bodyTxRegion, XY{ previewHeight / 2, previewHeight }, texW, texH);

	SDL_Rect handRRegion = { 48, 20, 4, 12 };
	SDL_Rect handRTxRegion = { 12, 8, 4, 12 };
	renderBox(orthoFrontPosition, XY{ 16, 32 }, handRRegion, handRTxRegion, XY{ previewHeight / 2, previewHeight }, texW, texH);

	SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
	SDL_RenderDrawLine(g_rd, orthoFrontPosition.x, orthoFrontPosition.y, orthoFrontPosition.x, orthoFrontPosition.y + previewHeight);
}

void MinecraftSkinPreviewScreen::tick()
{
}

void MinecraftSkinPreviewScreen::takeInput(SDL_Event evt)
{
}

BaseScreen* MinecraftSkinPreviewScreen::isSubscreenOf()
{
	return caller;
}
