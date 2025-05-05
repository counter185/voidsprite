#pragma once
#include "globals.h"
#include "BaseBrush.h"
#include "EventCallbackListener.h"

class Brush9SegmentRect :
    public BaseBrush, public EventCallbackListener
{
    XY startPos = XY{ 0,0 };
    bool heldDown = false;
    XY lastMousePos = XY{ 0,0 };

    NineSegmentPattern* pickedPattern = NULL;

    void resetState() override {
        startPos = XY{ 0,0 };
    }
    std::string getName() override { return "9-segment Rectangle [WIP]"; };
    std::string getTooltip() override { return "Mouse Right to select a 9-segment pattern.\nSelect a rectangle with Mouse Left to place it."; }
    std::string getIconPath() override { return "brush_9srect.png"; }
    XY getSection() override { return XY{ 0,2 }; }

    bool overrideRightClick() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override { lastMousePos = to; }
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;

    void eventGeneric(int evt_id, int data1, int data2) override;
};

