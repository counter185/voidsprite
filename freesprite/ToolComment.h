#pragma once
#include "BaseBrush.h"
#include "EventCallbackListener.h"
class ToolComment :
    public BaseBrush, public EventCallbackListener
{
public:
    MainEditor* clickEditor = NULL;
    XY clickPos = {0,0};

    virtual std::string getIconPath() { return VOIDSPRITE_ASSETS_PATH "assets/tool_comment.png"; }
    bool isReadOnly() override { return true; }
    std::string getName() { return "Add comment"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void eventTextInputConfirm(int evt_id, std::string data) override;
    //void clickDrag(MainEditor* editor, XY from, XY to) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
        drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
    }
};

