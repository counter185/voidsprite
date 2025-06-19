#include "Brush3pxCircle.h"

void Brush3pxCircle::clickPress(MainEditor* editor, XY pos)
{
    int size = (int)(editor->toolProperties["brush.circlepixel.size"] 
        * (editor->toolProperties["brush.circlepixel.pressuresens"] == 1 ? editor->penPressure : 1.0));
    rasterizeCirclePoint(pos, size, [editor](XY p) {
        editor->SetPixel(p, editor->getActiveColor());
    });
}

void Brush3pxCircle::clickDrag(MainEditor* editor, XY from, XY to)
{
    int size = (int)(editor->toolProperties["brush.circlepixel.size"] 
        * (editor->toolProperties["brush.circlepixel.pressuresens"] == 1 ? editor->penPressure : 1.0));
    rasterizeLine(from, to, [editor, size](XY pos) {
        rasterizeCirclePoint(pos, size, [editor](XY p) {
            editor->SetPixel(p, editor->getActiveColor());
        });
    });
    //editor->DrawLine(from, to, 0xFF000000 | editor->pickedColor);
}

void Brush3pxCircle::renderOnCanvas(MainEditor* editor, int scale) {
    bool pressureSensitivity = editor->toolProperties["brush.circlepixel.pressuresens"] == 1;
    int size = editor->toolProperties["brush.circlepixel.size"];
    if (pressureSensitivity && editor->leftMouseHold) {
        size = (int)(size * editor->penPressure);
    }
    rasterizeCirclePoint(lastMouseMotionPos, size, [this, editor, scale](XY p) {
        drawSelectedPoint(editor, p);
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
