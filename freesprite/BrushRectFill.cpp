#include "BrushRectFill.h"
#include "mathops.h"
#include "maineditor.h"

void BrushRectFill::clickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
}

void BrushRectFill::clickRelease(MainEditor* editor, XY pos)
{
	int minx = ixmin(pos.x, startPos.x);
	int maxx = ixmax(pos.x, startPos.x);
	int miny = ixmin(pos.y, startPos.y);
	int maxy = ixmax(pos.y, startPos.y);

	for (int x = minx; x <= maxx; x++) {
		for (int y = miny; y <= maxy; y++) {
			editor->SetPixel(XY{ x,y }, 0xFF000000 | editor->pickedColor);
		}
	}
}
