#include "ToolRectClone.h"
#include "../Notification.h"
#include "../background_operation.h"

void ToolRectClone::clickPress(MainEditor* editor, XY pos)
{
	mouseDown = true;
	mouseDownPoint = pos;
}

void ToolRectClone::clickRelease(MainEditor* editor, XY pos)
{
	if (mouseDown) {
		mouseDown = false;

		if (xyEqual(prevReleasePoint, pos) && pointInBox(pos, {0,0,editor->canvas.dimensions.x, editor->canvas.dimensions.y})
			&& lastClickTimer.started && lastClickTimer.elapsedTime() < 1000) {
			XY tileDim = editor->tileDimensions;
			tileDim.x = tileDim.x == 0 ? editor->canvas.dimensions.x : tileDim.x;
			tileDim.y = tileDim.y == 0 ? editor->canvas.dimensions.y : tileDim.y;
        	XY tileOrigin = {pos.x / tileDim.x * tileDim.x, pos.y / tileDim.y * tileDim.y};
			mouseDownPoint = tileOrigin;
			pos = xyAdd(tileOrigin, xySubtract(tileDim, {1,1}));
		}
    	prevReleasePoint = pos;
		lastClickTimer.start();

		int xmin = ixmin(pos.x, mouseDownPoint.x);
		int xmax = ixmax(pos.x, mouseDownPoint.x)+1;
		int ymin = ixmin(pos.y, mouseDownPoint.y);
		int ymax = ixmax(pos.y, mouseDownPoint.y)+1;
		clonedAreaPointAndDimensions = SDL_Rect{ xmin, ymin, xmax - xmin, ymax - ymin };
		if (clonedArea != NULL) {
			tracked_free(clonedArea);
			tracked_destroyTexture(cacheClonePreview);
		}
		clonedArea = (uint32_t*)tracked_malloc(clonedAreaPointAndDimensions.w * clonedAreaPointAndDimensions.h * 4, "Temp. mem.");
		if (clonedArea == NULL) {
			g_addNotification(NOTIF_MALLOC_FAIL);
			return;
		}
		uint64_t copyIndex = 0;
		for (int y = ymin; y < ymax; y++) {
			for (int x = xmin; x < xmax; x++) {
				clonedArea[copyIndex++] = editor->layer_getPixelAt(XY{ x,y });
			}
		}
		cacheClonePreview = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, clonedAreaPointAndDimensions.w, clonedAreaPointAndDimensions.h);
		uint32_t* pixels;
		int pitch;
		SDL_LockTexture(cacheClonePreview, NULL, (void**)&pixels, &pitch);
		memcpy(pixels, clonedArea, copyIndex*4);
		SDL_UnlockTexture(cacheClonePreview);
		SDL_SetTextureBlendMode(cacheClonePreview, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(cacheClonePreview, 0x60);
	}
}

void ToolRectClone::rightClickPress(MainEditor* editor, XY pos)
{
	if (clonedArea != NULL) {

		editor->commitStateToCurrentLayer();
		uint64_t dataPointer = 0;
		for (int y = 0; y < clonedAreaPointAndDimensions.h; y++) {
			for (int x = 0; x < clonedAreaPointAndDimensions.w; x++) {
				editor->SetPixel(xyAdd(pos, XY{ x,y }), clonedArea[dataPointer++], false);
			}
		}
	}
}
