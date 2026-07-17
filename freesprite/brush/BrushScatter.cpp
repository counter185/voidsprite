#include "BrushScatter.h"

void BrushScatter::clickPress(MainEditor* editor, XY pos)
{
	bool pressureSize = editor->toolProperties["brush.scatter.pressuresens"] == 1;
    int size = (int)(round(editor->toolProperties["brush.scatter.size"]
        * (pressureSize ? editor->penPressure : 1.0)));
    bool round = editor->toolProperties["brush.scatter.round"] == 1;

    doScatter(editor, pos, size, round);
}

void BrushScatter::clickDrag(MainEditor* editor, XY from, XY to)
{
    rasterizeLine(from, to, [&](XY a) { clickPress(editor, a); });
}

void BrushScatter::renderOnCanvas(MainEditor * editor, int scale)
{
    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    bool pressureSize = editor->toolProperties["brush.scatter.pressuresens"] == 1;
    int size = (int)(round(editor->toolProperties["brush.scatter.size"]
        * (pressureSize ? editor->penPressure : 1.0)));
    bool round = editor->toolProperties["brush.scatter.round"] == 1;

    rasterizePoint(lastMouseMotionPos, size, [&](XY p) {
        drawSelectedPoint(editor, p);
    }, round);
}

void BrushScatter::doScatter(MainEditor* editor, XY pos, int size, bool round)
{
    Layer* l = editor->getCurrentLayer();
    std::vector<std::pair<XY, u32>> posColors;
    rasterizePoint(pos, size, [&](XY p) {
        XY ogP = p;
        if (!posColors.empty()) {
            auto& randomIndex = posColors[randomInt(0, posColors.size())];
            XY pp = randomIndex.first;
            randomIndex.first = p;
            p = pp;
            
        }
        posColors.push_back({ p, l->getPixelAt(ogP) });
    }, round);

    for (auto& [pos, color] : posColors) {
        editor->SetPixel(pos, color, false);
    }
}
