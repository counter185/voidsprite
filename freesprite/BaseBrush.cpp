#include "BaseBrush.h"
#include "maineditor.h"

void BaseBrush::renderOnCanvas(MainEditor* editor, int scale)
{
	renderOnCanvas(editor->canvas.currentDrawPoint, scale);
}

void BaseBrush::drawPixelRect(XY from, XY to, XY canvasDrawPoint, int scale)
{
	XY pointFrom = XY{ ixmin(from.x, to.x), ixmin(from.y, to.y) };
	XY pointTo = XY{ ixmax(from.x, to.x), ixmax(from.y, to.y) };
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x20);
	SDL_Rect r = { canvasDrawPoint.x + (pointFrom.x * scale), canvasDrawPoint.y + (pointFrom.y * scale), ((pointTo.x - pointFrom.x + 1) * scale), ((pointTo.y - pointFrom.y + 1) * scale) };
	SDL_RenderFillRect(g_rd, &r);
	SDL_SetRenderDrawColor(g_rd, 0x00, 0x00, 0x00, 0x80);
	SDL_Rect r2 = r;
	r2.x++;
	r2.y++;
	r2.w -= 2;
	r2.h -= 2;
	SDL_RenderDrawRect(g_rd, &r2);
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
	SDL_RenderDrawRect(g_rd, &r);

	SDL_SetRenderDrawColor(g_rd, 0x00, 0x00, 0x00, 0x80);
	SDL_RenderDrawLine(g_rd, r.x, r.y + 1, r.x + r.w - 1, r.y + r.h);
	//SDL_RenderDrawLine(g_rd, r.x + 1, r.y, r.x + r.w, r.y + r.h - 1);
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
	SDL_RenderDrawLine(g_rd, r.x, r.y, r.x + r.w, r.y + r.h);
}
