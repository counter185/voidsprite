#include "Brush3pxCircle.h"
#include "maineditor.h"

void Brush3pxCircle::clickPress(MainEditor* editor, XY pos)
{
	int size = editor->toolProperties["brush.circlepixel.size"];

	rasterizeCirclePoint(pos, size, [editor](XY p) {
		editor->SetPixel(p, editor->getActiveColor());
	});
}

void Brush3pxCircle::clickDrag(MainEditor* editor, XY from, XY to)
{
	int size = editor->toolProperties["brush.circlepixel.size"];
	rasterizeLine(from, to, [editor, size](XY pos) {
		rasterizeCirclePoint(pos, size, [editor](XY p) {
			editor->SetPixel(p, editor->getActiveColor());
		});
	});
	//editor->DrawLine(from, to, 0xFF000000 | editor->pickedColor);
}

void Brush3pxCircle::renderOnCanvas(MainEditor* editor, int scale) {
	int size = editor->toolProperties["brush.circlepixel.size"];
	rasterizeCirclePoint(lastMouseMotionPos, size, [this, editor, scale](XY p) {
		XY canvasDrawPoint = editor->canvas.currentDrawPoint;
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
		drawLocalPoint(canvasDrawPoint, p, scale);
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
		drawPointOutline(canvasDrawPoint, p, scale);
	});
}

void Brush3pxCircle::rasterizeCirclePoint(XY point, int r, std::function<void(XY)> forEachPoint)
{
	int halfSize = r / 2;
	//bool sizeOdd = r % 2;
	XY origin = {point.x - halfSize, point.y - halfSize};
	for (int x = 0; x < r; x++) {
		for (int y = 0; y < r; y++) {
			int dx = x - halfSize;
			int dy = y - halfSize;
			if (dx * dx + dy * dy <= halfSize * halfSize) {
				forEachPoint(xyAdd(origin, {x,y}));
			}
		}
	}
}
