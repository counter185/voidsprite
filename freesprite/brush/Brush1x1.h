#pragma once
#include "BaseBrush.h"

class Brush1x1 : public BaseBrush
{
    std::string getName() override { return TL("vsp.brush.squarepixel"); }
    std::string getIconPath() override { return "brush_px1x1.png"; }
    std::map<std::string, BrushProperty> getProperties() override 
    { 
        return {
            {"brush.squarepixel.round", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.round"),0)},
            {"brush.squarepixel.pressuresens", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.pressuresize"),0)},
            {"brush.squarepixel.size", BRUSH_INT_PROPERTY(TL("vsp.brush.param.size"),1,16,1)}
        }; 
    }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override {}
    void renderOnCanvas(MainEditor* editor, int scale) override;
};

class Brush1x1PixelPerfect : public BaseBrush
{
    XY lastPoint = { 0,0 };
    XY lastTrailPoint = { 0,0 };
    bool hasTrailPoint = false;
    bool dragging = false;

    std::string getName() override { return TL("vsp.brush.squarepixelpxperfect"); }
    std::string getIconPath() override { return "brush_px1x1pxpf.png"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) override {
        drawSelectedPoint(editor, lastMouseMotionPos);
    }
};


//bug promoted to feature
class Brush1x1Burst : public BaseBrush
{
    XY lastPoint = { 0,0 };
    XY lastTrailPoint = { 0,0 };
    bool hasTrailPoint = false;
    bool dragging = false;
    bool rightHeld = false;

    std::string getName() override { return TL("vsp.brush.squarepixelburst"); }
    std::string getTooltip() override { return TL("vsp.brush.squarepixelburst.desc"); }
    std::string getIconPath() override { return "brush_px1x1burst.png"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    bool overrideRightClick() override { return true; }
    void rightClickPress(MainEditor* editor, XY pos) override;
    void rightClickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) {
        drawSelectedPoint(editor, lastMouseMotionPos);
    }
};

