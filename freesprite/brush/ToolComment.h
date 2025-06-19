#pragma once
#include "BaseBrush.h"
#include "../EventCallbackListener.h"

class ToolComment :
    public BaseBrush, public EventCallbackListener
{
public:
    MainEditor* clickEditor = NULL;
    XY clickPos = {0,0};

    std::string getIconPath() override { return "tool_comment.png"; }
    bool isReadOnly() override { return true; }
    std::string getName() override { return TL("vsp.brush.comment"); }
    std::string getTooltip() override { return TL("vsp.brush.comment.desc"); }
    XY getSection() override { return XY{ 1,1 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void eventTextInputConfirm(int evt_id, std::string data) override;
    //void clickDrag(MainEditor* editor, XY from, XY to) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
        drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
    }
};

