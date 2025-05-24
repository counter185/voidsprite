#pragma once
#include "globals.h"

#define BRUSH_INT_PROPERTY(name,min,max,defaultvalue) BrushProperty{name,1,min,max,defaultvalue}
#define BRUSH_DOUBLE_PROPERTY(name,min,max,defaultvalue) BrushProperty{name,2,min,max,defaultvalue}
#define BRUSH_BOOL_PROPERTY(name,defaultvalue) BrushProperty{name,3,0,1,defaultvalue}

struct BrushProperty {
	std::string name;
	int type;
	double min;
	double max;
	double defaultValue;
};

class BaseBrush
{
public:
	SDL_Texture* cachedIcon = NULL;
	XY lastMouseMotionPos = XY{ 0,0 };

	virtual void resetState() {}
	virtual bool isReadOnly() { return false; }
	virtual bool wantDoublePosPrecision() { return false; }
	virtual bool overrideRightClick() { return false; }
	virtual std::string getIconPath() { return "brush_default.png"; }
	virtual std::string getName() { return TL("vsp.brush.undefined"); }
	virtual std::string getTooltip() { return ""; }
	virtual XY getSection() { return XY{ 0,0 }; }
	virtual std::map<std::string, BrushProperty> getProperties() { return {}; }

	virtual void mouseMotion(MainEditor* editor, XY pos) {
		lastMouseMotionPos = pos;
	}
	virtual void clickPress(MainEditor* editor, XY pos) {}
	virtual void clickDrag(MainEditor* editor, XY from, XY to) {}
	virtual void clickRelease(MainEditor* editor, XY pos) {}
	virtual void rightClickPress(MainEditor* editor, XY pos) {}
	virtual void rightClickRelease(MainEditor* editor, XY pos) {}
	virtual void renderOnCanvas(XY canvasDrawPoint, int scale) {}
	// Get canvasDrawPoint with editor->canvas.currentDrawPoint
	virtual void renderOnCanvas(MainEditor* editor, int scale);

	void drawLocalPoint(XY canvasOrigin, XY point, int scale) {
		SDL_Rect r = { canvasOrigin.x + point.x * scale, canvasOrigin.y + point.y * scale, scale, scale };
		SDL_RenderFillRect(g_rd, &r);
	}	
	
	void drawPointStrikethrough(XY canvasOrigin, XY point, int scale) {
		SDL_Rect r = { canvasOrigin.x + point.x * scale, canvasOrigin.y + point.y * scale, scale, scale };
		if (r.x < g_windowW && r.x + r.w >= 0) {
			SDL_RenderDrawLine(g_rd, r.x, r.y, r.x + scale - 1, r.y + scale - 1);
		}
	}
	void drawPointOutline(XY canvasOrigin, XY point, int scale) {
		SDL_Rect r = { canvasOrigin.x + point.x * scale, canvasOrigin.y + point.y * scale, scale, scale };
		
		SDL_RenderDrawRect(g_rd, &r);
		drawPointStrikethrough(canvasOrigin, point, scale);
		//SDL_RenderDrawLine(g_rd, r.x, r.y+scale/2, r.x + scale/2-1, r.y + scale-1);
		//SDL_RenderDrawLine(g_rd, r.x+scale/2, r.y, r.x + scale-1, r.y + scale/2-1);
	}

	void drawWholeSelectedPoint(XY canvasDrawPoint, XY onCanvasPoint, int scale);
	void drawActiveColorPoint(MainEditor* e, XY onCanvasPoint);
	void drawSelectedPoint(MainEditor* e, XY onCanvasPoint);

	void drawPixelRect(XY from, XY to, XY canvasDrawPoint, int scale);
};

void g_loadBrushes();