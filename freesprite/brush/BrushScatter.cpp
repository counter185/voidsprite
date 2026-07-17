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
    bool move1px = editor->toolProperties["brush.scatter.move1px"] == 1;

    Layer* l = editor->getCurrentLayer();

    if (!move1px) {
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
    else {
        std::map<u64, int> validPositions;
        std::vector<std::pair<XY, u32>> posColors;

        rasterizePoint(pos, size, [&](XY p) {
            posColors.push_back({ p, l->getPixelAt(p) });
            validPositions[encodeXY(p)] = posColors.size()-1;
        }, round);

        for (auto& [enc, idx] : validPositions) {
            auto& posColorRef = posColors[idx];
            XY p = posColorRef.first;
            XY nextPos = xyAdd(p, XY{ randomInt(-1, 2), randomInt(-1, 2) });
            if (!xyEqual(nextPos, p) && validPositions.contains(encodeXY(nextPos))) {
                auto& other = posColors[validPositions[encodeXY(nextPos)]];
                posColorRef.first = other.first;
                other.first = p;
            }
        }

        for (auto& [pos, color] : posColors) {
            editor->SetPixel(pos, color, false);
        }

    }
}
