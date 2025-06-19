#include "Brush9SegmentRect.h"
#include "../Notification.h"
#include "../PopupSet9SPattern.h"

void Brush9SegmentRect::clickPress(MainEditor* editor, XY pos)
{
    heldDown = true;
    lastMousePos = pos;
    startPos = pos;
}

void Brush9SegmentRect::clickRelease(MainEditor* editor, XY pos)
{
    if (!heldDown) {
        return;
    }
    heldDown = false;
    pos = g_shiftModifier ? getSnappedPoint(startPos, pos) : pos;
    int minx = ixmin(pos.x, startPos.x);
    int maxx = ixmax(pos.x, startPos.x);
    int miny = ixmin(pos.y, startPos.y);
    int maxy = ixmax(pos.y, startPos.y);
    maxx++;
    maxy++;

    if (editor->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Tool not supported in palettized editor."));
        return;
    }
    if (pickedPattern == NULL) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "No pattern selected."));
        return;
    }

    XY wh = XY{ maxx - minx, maxy - miny };
    for (int x = 0; x < wh.x; x++) {
        for (int y = 0; y < wh.y; y++) {
            XY fragment;
            int segXLineInRect1 = ixmin(wh.x / 2, pickedPattern->point1.x);
            int segXLineInRect2 = ixmax(wh.x / 2, wh.x - pickedPattern->point2.x) - 1;
            int segYLineInRect1 = ixmin(wh.y / 2, pickedPattern->point1.y);
            int segYLineInRect2 = ixmax(wh.y / 2, wh.y - pickedPattern->point2.y) - 1;
            fragment.y = y < segYLineInRect1 ? 0
                : y >= segYLineInRect2 ? 2
                : 1;
            fragment.x = x < segXLineInRect1 ? 0
                : x >= segXLineInRect2 ? 2
                : 1;
            XY inPatternPos = {
                fragment.x == 0 ? x
                : fragment.x == 2 ? pickedPattern->dimensions.x - (wh.x - x)
                : pickedPattern->point1.x + (x - pickedPattern->point1.x) % (pickedPattern->dimensions.x - pickedPattern->point2.x - pickedPattern->point1.x - 1),
                fragment.y == 0 ? y
                : fragment.y == 2 ? pickedPattern->dimensions.y - (wh.y - y)
                : pickedPattern->point1.y + (y - pickedPattern->point1.y) % (pickedPattern->dimensions.y - pickedPattern->point2.y - pickedPattern->point1.y - 1)
            };
            editor->SetPixel(XY{ minx + x, miny + y }, pickedPattern->pixelData[inPatternPos.y * pickedPattern->dimensions.x + inPatternPos.x]);
            /*editor->SetPixel(XY{minx + x, miny + y},
                0xFF000000 + ((0x7F * fragment.x) << 16) + (0x7F * fragment.y)
                );*/
        }
    }
}

void Brush9SegmentRect::rightClickPress(MainEditor* editor, XY pos)
{
    PopupSet9SPattern* p = new PopupSet9SPattern();
    p->setCallbackListener(EVENT_9SPPICKER_POPUP , this);
    g_addPopup(p);
    /*if (pickedPattern == NULL) {
        pickedPattern = g_9spatterns[0];
    }
    else {
        int f = std::find(g_9spatterns.begin(), g_9spatterns.end(), pickedPattern) - g_9spatterns.begin();
        f++;
        f %= g_9spatterns.size();
        pickedPattern = g_9spatterns[f];
    }*/
}

void Brush9SegmentRect::renderOnCanvas(XY canvasDrawPoint, int scale)
{
    if (heldDown) {
        drawPixelRect(startPos, g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos, canvasDrawPoint, scale);
    }

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
    drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}

void Brush9SegmentRect::eventGeneric(int evt_id, int data1, int data2)
{
    if (evt_id == EVENT_9SPPICKER_POPUP) {
        if (data1 >= 0 && data1 < g_9spatterns.size()) {
            pickedPattern = g_9spatterns[data1];
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"),"Invalid pattern"));
        }
    }
}
