#include "Brush1pxLinePathfind.h"
#include "Notification.h"
#include "globals.h"
#include "maineditor.h"
#include "UtilPathfind.h"
#include "background_operation.h"
#include "mathops.h"
#include "mathops.tcc"
#include <algorithm>
#include <cassert>
#include <cstring>

class BigBitField {
private:
    char* bits;
    usize w, h;

public:
    BigBitField(usize w, usize h) : bits((char*) malloc(w * h)), w(w), h(h) {
        if (bits != NULL) memset(bits, 0, div_ceil<usize>(w * h, 8));
    }

    ~BigBitField() {
        if (bits != NULL) free(bits);
    }

    inline bool bad() const {
        return bits == NULL;
    }

    inline void set(isize x, isize y) {
        if (x < 0 || y < 0 || (usize) x > w || (usize) y > h) return;

        usize p = (usize) y * h + (usize) x;
        usize s = p % 8;
        p /= 8;

        bits[p] |= 1 << s;
    }

    inline bool is(isize x, isize y) {
        if (x < 0 || y < 0 || (usize) x > w || (usize) y > h) return false;

        usize p = (usize) y * h + (usize) x;
        usize s = p % 8;
        p /= 8;

        return bits[p] & (1 << s);
    }
};

void Brush1pxLinePathfind::clickPress(MainEditor* editor, XY pos)
{
    (void) editor;
    
    startPos = pos;
    dragging = true;
}

bool CanWalkOnMapPoint(Layer* map, int x, int y, u32 col1, u32 col2);

void Brush1pxLinePathfind::clickRelease(MainEditor* editor, XY pos)
{
    auto layer = editor->getCurrentLayer();
    auto known = BigBitField(layer->w, layer->h);

    if (known.bad()) {
        g_addNotification(ErrorNotification("Allocation failed", "Failed to allocate space for check buffer."));
    } else {
        known.set((isize) startPos.x, (isize) startPos.y);
        bool changed = true;
        isize maxDist = 0;

        u32 sCol = layer->getPixelAt(startPos);
        u32 eCol = layer->getPixelAt(pos);

        while (changed) {
            maxDist++;
            changed = false;

            for (isize i = std::max<isize>(0, (isize) startPos.x - maxDist), ib = std::min((isize) layer->w, (isize) startPos.x + maxDist); i < ib; i++)
                for (isize o = std::max<isize>(0, (isize) startPos.y - maxDist), ob = std::min((isize) layer->h, (isize) startPos.y + maxDist); o < ob; o++)
                    if (!known.is(i, o) && CanWalkOnMapPoint(layer, (int) i, (int) o, sCol, eCol) && (known.is(i - 1, o) || known.is(i + 1, o) || known.is(i, o - 1) || known.is(i, o + 1))) {
                        changed = true;
                        assert(CanWalkOnMapPoint(layer, (int) i, (int) o, sCol, eCol));
                        known.set(i, o);
                        if (i == pos.x && o == pos.y) goto found;
                    }
        }

        found:

        if (!known.is(pos.x, pos.y)) {
            g_addNotification(ErrorNotification("No path found", "Selected point is unreachable from specified starting location."));
            dragging = false;
            return;
        }
    }

    g_startNewOperation([this, editor, pos]() {
        std::vector<Node> pathfindResult = genAStar(editor->getCurrentLayer(), startPos, pos);
        for (Node& n : pathfindResult) {
            editor->SetPixel(XY{ n.x, n.y }, editor->getActiveColor());
        }
        //editor->DrawLine(startPos, pos, 0xFF000000 | editor->pickedColor);
        dragging = false;
    });
}

void Brush1pxLinePathfind::renderOnCanvas(XY canvasDrawPoint, int scale)
{
    if (dragging) {
        rasterizeLine(startPos, lastMouseMotionPos, [&](XY a) {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
            drawLocalPoint(canvasDrawPoint, a, scale);
            SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
            drawPointOutline(canvasDrawPoint, a, scale);
        });
    }
    else {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
        drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
    }
}
