#pragma once
#include "BaseBrush.h"

class Brush3pxCircle : public BaseBrush
{
	std::string getName() override { return TL("vsp.brush.roundpixel"); }
	std::string getIconPath() override { return "brush_3pxcircle.png"; }
	std::map<std::string, BrushProperty> getProperties() override
	{
		return {
			{"brush.circlepixel.pressuresens", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.pressuresize"),1)},
			{"brush.circlepixel.size", BRUSH_INT_PROPERTY(TL("vsp.brush.param.size"),3,16,3)}
		};
	}
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override;
	void clickRelease(MainEditor* editor, XY pos) override {}
	void renderOnCanvas(MainEditor* editor, int scale) override;

	static void rasterizeCirclePoint(XY point, int r, std::function<void(XY)> forEachPoint);
};

