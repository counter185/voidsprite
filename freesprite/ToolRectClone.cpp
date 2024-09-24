#include "ToolRectClone.h"
#include "mathops.h"
#include "maineditor.h"
#include "Notification.h"

void ToolRectClone::clickPress(MainEditor* editor, XY pos)
{
	mouseDown = true;
	mouseDownPoint = pos;
}

void ToolRectClone::clickRelease(MainEditor* editor, XY pos)
{
	mouseDown = false;
	int xmin = ixmin(pos.x, mouseDownPoint.x);
	int xmax = ixmax(pos.x, mouseDownPoint.x)+1;
	int ymin = ixmin(pos.y, mouseDownPoint.y);
	int ymax = ixmax(pos.y, mouseDownPoint.y)+1;
	clonedAreaPointAndDimensions = SDL_Rect{ xmin, ymin, xmax - xmin, ymax - ymin };
	if (clonedArea != NULL) {
		free(clonedArea);
		SDL_DestroyTexture(cacheClonePreview);
	}
	clonedArea = (uint32_t*)malloc(clonedAreaPointAndDimensions.w * clonedAreaPointAndDimensions.h * 4);
	if (clonedArea == NULL) {
		g_addNotification(ErrorNotification("Error", "malloc failed"));
		return;
	}
	uint64_t copyIndex = 0;
	for (int y = ymin; y < ymax; y++) {
		for (int x = xmin; x < xmax; x++) {
			clonedArea[copyIndex++] = editor->layer_getPixelAt(XY{ x,y });
		}
	}
	cacheClonePreview = SDL_CreateTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, clonedAreaPointAndDimensions.w, clonedAreaPointAndDimensions.h);
	uint32_t* pixels;
	int pitch;
	SDL_LockTexture(cacheClonePreview, NULL, (void**)&pixels, &pitch);
	memcpy(pixels, clonedArea, copyIndex*4);
	SDL_UnlockTexture(cacheClonePreview);
	SDL_SetTextureBlendMode(cacheClonePreview, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(cacheClonePreview, 0x60);
}

void ToolRectClone::rightClickPress(MainEditor* editor, XY pos)
{
	if (clonedArea != NULL) {
		editor->commitStateToCurrentLayer();
		uint64_t dataPointer = 0;
		for (int y = 0; y < clonedAreaPointAndDimensions.h; y++) {
			for (int x = 0; x < clonedAreaPointAndDimensions.w; x++) {
				editor->SetPixel(xyAdd(pos, XY{ x,y }), clonedArea[dataPointer++]);
			}
		}
	}
}
