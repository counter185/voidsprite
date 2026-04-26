#include "ToolMeasure.h"
#include "../FontRenderer.h"
#include "../TooltipsLayer.h"
#include "../PopupContextMenu.h"
#include "../Notification.h"

void ToolMeasure::clickPress(MainEditor* editor, XY pos)
{
    heldDown = true;
    lastMousePos = pos;
    startPos = lastOrigin = pos;
    lastEnd = startPos;
    clickTimer.start();
}

void ToolMeasure::clickDrag(MainEditor* editor, XY from, XY to) {
    lastEnd = g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos;
    lastMousePos = to;
}

void ToolMeasure::clickRelease(MainEditor* editor, XY pos)
{
    lastEnd = g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos;
    heldDown = false;
}

void ToolMeasure::rightClickPress(MainEditor* editor, XY pos)
{
    if (!xyEqual(lastOrigin, lastEnd)) {
        PopupContextMenu* popup = new PopupContextMenu({
            {editor->eraserMode ? "Remove guidelines around area" : "Place guidelines around area", [this, editor]() { editorPlaceGuidelinesAroundSelRegion(editor); }},
            {"Crop to this area", [this, editor]() { editorCropToSelRegion(editor); }}
            });
        g_addPopup(popup);
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "No selected area"));
    }
}

void ToolMeasure::renderOnCanvas(MainEditor* editor, int scale)
{
    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    if (heldDown) {
        XY measureEndPoint = g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos;
        drawPixelRect(startPos, measureEndPoint, canvasDrawPoint, scale);
        XY pointFrom = XY{ ixmin(startPos.x, measureEndPoint.x), ixmin(startPos.y, measureEndPoint.y) };
        XY pointTo = XY{ ixmax(startPos.x, measureEndPoint.x), ixmax(startPos.y, measureEndPoint.y) };

        g_ttp->addTooltip(Tooltip{
            canvasDrawPoint.x + lastMousePos.x * scale + 25, canvasDrawPoint.y + lastMousePos.y * scale,
            frmt("{}px x {}px", pointTo.x - pointFrom.x + 1, pointTo.y - pointFrom.y + 1),
            {0xff,0xff,0xff,0xff},
            clickTimer.percentElapsedTime(200)
            });
    }
    else if (!xyEqual(lastOrigin, lastEnd)) {
        XY pointFrom = XY{ ixmin(lastOrigin.x, lastEnd.x), ixmin(lastOrigin.y, lastEnd.y) };
        XY pointTo = XY{ ixmax(lastOrigin.x, lastEnd.x), ixmax(lastOrigin.y, lastEnd.y) };
        int rectW = pointTo.x - pointFrom.x + 1;
        int rectH = pointTo.y - pointFrom.y + 1;

        auto accentColor = editor->getAccentColor();

        SDL_Rect rect = calcPixelRect(startPos, lastEnd, canvasDrawPoint, scale);
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
        SDL_RenderDrawRect(g_rd, &rect);
        rect = offsetRect(rect, 1);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xa0);
        SDL_RenderDrawRect(g_rd, &rect);
        g_fnt->RenderString(frmt("{}x{}", rectW, rectH), rect.x + 2, rect.y, { accentColor.r, accentColor.g, accentColor.b,0xa0 });
    }

    drawWholeSelectedPoint(canvasDrawPoint, lastMouseMotionPos, scale);

}

SDL_Rect ToolMeasure::getSelectedRegion()
{
    XY pointFrom = XY{ ixmin(lastOrigin.x, lastEnd.x), ixmin(lastOrigin.y, lastEnd.y) };
    XY pointTo = XY{ ixmax(lastOrigin.x, lastEnd.x), ixmax(lastOrigin.y, lastEnd.y) };
    int rectW = pointTo.x - pointFrom.x + 1;
    int rectH = pointTo.y - pointFrom.y + 1;
    return { pointFrom.x, pointFrom.y, rectW, rectH };
}

void ToolMeasure::editorPlaceGuidelinesAroundSelRegion(MainEditor* editor)
{
    XY pointFrom = XY{ ixmin(lastOrigin.x, lastEnd.x), ixmin(lastOrigin.y, lastEnd.y) };
    XY pointTo = XY{ ixmax(lastOrigin.x, lastEnd.x), ixmax(lastOrigin.y, lastEnd.y) };

    if (editor->eraserMode) {
        editor->removeGuideline(pointFrom.y * 2, false);
        editor->removeGuideline((pointTo.y + 1) * 2, false);
        editor->removeGuideline(pointFrom.x * 2, true);
        editor->removeGuideline((pointTo.x + 1) * 2, true);
    }
    else {
        editor->addGuideline(pointFrom.y * 2, false);
        editor->addGuideline((pointTo.y + 1) * 2, false);
        editor->addGuideline(pointFrom.x * 2, true);
        editor->addGuideline((pointTo.x + 1) * 2, true);
    }
}

void ToolMeasure::editorCropToSelRegion(MainEditor* editor)
{
    SDL_Rect region = getSelectedRegion();
    editor->cropAllLayersFromCommand(region);
    lastOrigin.x -= region.x;
    lastOrigin.y -= region.y;
    lastEnd.x -= region.x;
    lastEnd.y -= region.y;
}
