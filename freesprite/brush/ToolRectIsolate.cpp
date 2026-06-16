#include "ToolRectIsolate.h"
#include "../background_operation.h"

void ToolRectIsolate::clickPress(MainEditor* editor, XY pos)
{
    heldDown = true;
    pos = clampPointInsideCanvasIfParam(editor, pos);
    startPos = pos;
    lastMousePos = pos;
    clickTimer.start();
}

void ToolRectIsolate::clickRelease(MainEditor* editor, XY pos) { 
    pos = clampPointInsideCanvasIfParam(editor, pos);
    if (heldDown) {
        heldDown = false;
        //double click to wand select
        if (xyEqual(startPos, lastMousePos) && clickTimer.started && clickTimer.elapsedTime() < BRUSH_DOUBLE_CLICK_TIME) {
            if (!g_ctrlModifier && !editor->eraserMode) {
                editor->isolatedFragment.clear();
            }
            bool selectAllOfColor = g_shiftModifier;

            Layer* targetLayer = editor->getCurrentLayer();
            g_startNewOperation([targetLayer, editor, pos, selectAllOfColor]() {
                ScanlineMap selectionMap = 
                    selectAllOfColor ? targetLayer->selectAllOfColor(targetLayer->getPixelAt(pos))
                    : targetLayer->wandSelectAt(pos);
                g_startNewMainThreadOperation([editor, selectionMap]() {
                    ScanlineMap old = editor->isolatedFragment;
                    if (editor->eraserMode) {
                        editor->isolatedFragment.removeOtherMap(selectionMap);
                    }
                    else {
                        editor->isolatedFragment.addOtherMap(selectionMap);
                    }
                    editor->commitIsolatedFragmentState(old);
                    editor->isolateEnabled = !editor->isolatedFragment.empty();
                    editor->shouldUpdateRenderedIsolatedFragmentPoints = true;
                });
            });
        } else {
            XY p1 = startPos;
            XY p2 = pos;
            XY minpoint = XY{ixmin(p1.x, p2.x), ixmin(p1.y, p2.y)};
            XY maxpoint = XY{ixmax(p1.x, p2.x), ixmax(p1.y, p2.y)};
            ScanlineMap old = editor->isolatedFragment;
            if (!g_ctrlModifier && !editor->eraserMode) {
                editor->isolatedFragment.clear();
            }
            if (editor->eraserMode) {
                editor->isolatedFragment.removeRect(
                    { minpoint.x, minpoint.y, maxpoint.x - minpoint.x + 1, maxpoint.y - minpoint.y + 1 });
            }
            else {
                editor->isolatedFragment.addRect(
                    { minpoint.x, minpoint.y, maxpoint.x - minpoint.x + 1, maxpoint.y - minpoint.y + 1 });
            }
            editor->commitIsolatedFragmentState(old);
            editor->isolateEnabled = !editor->isolatedFragment.empty();
            editor->shouldUpdateRenderedIsolatedFragmentPoints = true;
        }
        clickTimer.start();
    }
}

void ToolRectIsolate::renderOnCanvas(MainEditor* editor, int scale)
{
    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    if (heldDown) {
        XY pointFrom = XY{ ixmin(startPos.x, lastMousePos.x), ixmin(startPos.y, lastMousePos.y) };
        XY pointTo = XY{ ixmax(startPos.x, lastMousePos.x), ixmax(startPos.y, lastMousePos.y) };
        pointFrom = clampPointInsideCanvasIfParam(editor, pointFrom);
        pointTo = clampPointInsideCanvasIfParam(editor, pointTo);
        drawPixelRect(pointFrom, pointTo, canvasDrawPoint, scale);

        g_ttp->addTooltip(Tooltip{
            canvasDrawPoint.x + lastMousePos.x * scale + 25, canvasDrawPoint.y + lastMousePos.y * scale,
            frmt("{}px x {}px", pointTo.x - pointFrom.x + 1, pointTo.y - pointFrom.y + 1),
            {0xff,0xff,0xff,0xff},
            clickTimer.percentElapsedTime(200)
        });
    }
}

void ToolRectIsolate::rightClickPress(MainEditor* editor, XY pos)
{
    editor->deselectAndCommitToUndoStack();
}

XY ToolRectIsolate::clampPointInsideCanvasIfParam(MainEditor* editor, XY point)
{
    return editor->toolProperties["brush.isolate.locktobounds"] == 1 ?
        XY{
            ixmin(ixmax(point.x, 0), editor->canvas.dimensions.x - 1),
            ixmin(ixmax(point.y, 0), editor->canvas.dimensions.y - 1)
    }
    : point;
}