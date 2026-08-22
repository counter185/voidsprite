#include "ToolRectSwap.h"
#include "../Notification.h"

void ToolRectSwap::clickPress(MainEditor* editor, XY pos)
{
	mouseDown = true;
	mouseDownPoint = pos;
}

void ToolRectSwap::clickRelease(MainEditor* editor, XY pos)
{
	mouseDown = false;
	int xmin = ixmin(pos.x, mouseDownPoint.x);
	int xmax = ixmax(pos.x, mouseDownPoint.x) + 1;
	int ymin = ixmin(pos.y, mouseDownPoint.y);
	int ymax = ixmax(pos.y, mouseDownPoint.y) + 1;
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
	copyPixelsToTexture(clonedArea, clonedAreaPointAndDimensions.w, clonedAreaPointAndDimensions.h, (u8*)pixels, pitch);
	SDL_UnlockTexture(cacheClonePreview);
	SDL_SetTextureBlendMode(cacheClonePreview, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(cacheClonePreview, 0x60);
}

void ToolRectSwap::rightClickPress(MainEditor* editor, XY pos)
{
	if (clonedArea != NULL) {
		editor->commitStateToCurrentLayer();
		uint64_t dataPointer = 0;
		for (int y = 0; y < clonedAreaPointAndDimensions.h; y++) {
			for (int x = 0; x < clonedAreaPointAndDimensions.w; x++) {
				XY targetPos = xyAdd(pos, XY{ x,y });
				editor->SetPixel(xyAdd({ clonedAreaPointAndDimensions.x, clonedAreaPointAndDimensions.y }, { x,y }), editor->layer_getPixelAt(targetPos), false);
				editor->SetPixel(targetPos, clonedArea[dataPointer++], false);
			}
		}
	}
	else {
		g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "No area selected."));
	}
}

void ToolRectSwap::renderOnCanvas(MainEditor* editor, int scale) {
	XY canvasDrawPoint = editor->canvas.currentDrawPoint;
	if (mouseDown) {
		drawPixelRect(mouseDownPoint, lastMouseMotionPos, canvasDrawPoint, scale);
	}
	else if (clonedArea != NULL) {
		SDL_Color accent = editor->getAccentColor();
		SDL_SetRenderDrawColor(g_rd, accent.r, accent.g, accent.b, 0x40);
		SDL_Rect cAreaRect = SDL_Rect{
			canvasDrawPoint.x + clonedAreaPointAndDimensions.x * scale,
			canvasDrawPoint.y + clonedAreaPointAndDimensions.y * scale,
			clonedAreaPointAndDimensions.w * scale,
			clonedAreaPointAndDimensions.h * scale
		};
		SDL_RenderDrawRect(g_rd, &cAreaRect);

		SDL_Rect previewRect = SDL_Rect{
			canvasDrawPoint.x + lastMouseMotionPos.x * scale,
			canvasDrawPoint.y + lastMouseMotionPos.y * scale,
			clonedAreaPointAndDimensions.w * scale,
			clonedAreaPointAndDimensions.h * scale
		};
		SDL_RenderCopy(g_rd, cacheClonePreview, NULL, &previewRect);
		SDL_RenderDrawRect(g_rd, &previewRect);

		SDL_SetRenderDrawColor(g_rd, accent.r, accent.g, accent.b, 0x20);
		SDL_RenderDrawLine(g_rd, 0, cAreaRect.y, g_windowW, cAreaRect.y);
		SDL_RenderDrawLine(g_rd, 0, cAreaRect.y + cAreaRect.h, g_windowW, cAreaRect.y + cAreaRect.h);
		SDL_RenderDrawLine(g_rd, cAreaRect.x, 0, cAreaRect.x, g_windowH);
		SDL_RenderDrawLine(g_rd, cAreaRect.x + cAreaRect.w, 0, cAreaRect.x + cAreaRect.w, g_windowH);
	}

	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
}
