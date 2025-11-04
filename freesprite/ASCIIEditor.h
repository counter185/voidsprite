#pragma once
#include "BaseScreen.h"
#include "Canvas.h"
#include "FontRenderer.h"

struct ASCIIChar {
    u8 ch = 0;
    u32 foreground = 0xFFFFFFFF;
    u32 background = 0xFF000000;
};

class ASCIISession {
private:
    XY dimensions;
    std::vector<std::vector<ASCIIChar>> data;
public:
    ASCIISession(XY dims) {
        resize(dims);
    }
    static ASCIISession* fromTXT(PlatformNativePathString path);
    static ASCIISession* fromRexpaintCompressed(PlatformNativePathString path);

    XY getSize() { return dimensions; }
    ASCIIChar get(XY at) {
        if (pointInBox(at, { 0,0,dimensions.x,dimensions.y })) {
            return data[at.y][at.x];
        }
        return { 0 };
    }
    bool set(XY at, ASCIIChar val) {
        if (pointInBox(at, { 0,0,dimensions.x,dimensions.y })) {
            data[at.y][at.x] = val;
            return true;
        }
        return false;
    }
    void setWithResize(XY at, ASCIIChar val) {
        if (!pointInBox(at, { 0,0,dimensions.x,dimensions.y })) {
            resize({ ixmax(at.x, dimensions.x), ixmax(at.y, dimensions.y) });
        }
        XY newSize = dimensions;
        if (at.x >= newSize.x) newSize.x = at.x + 1;
        if (at.y >= newSize.y) newSize.y = at.y + 1;
        resize(newSize);
        data[at.y][at.x] = val;
    }

    void resize(XY newSize) {
        dimensions = newSize;
        data.resize(newSize.y);
        for (auto& row : data) {
            row.resize(newSize.x);
        }
    }
};

class ASCIIEditor :
    public BaseScreen
{
protected:
    ASCIISession* session = NULL;
    BitmapFontObject* font = NULL;
    Canvas c;
    SDL_Renderer* lastRenderer = NULL;

    std::map<u8, GlyphData> glyphCache;

    XY fontScale = { 1,1 };

    PlatformNativePathString fontBitmapPath;

public:
    ASCIIEditor(XY dimensions);
    ASCIIEditor(ASCIISession* ssn);
    ~ASCIIEditor();

    std::string getName() override { return "ASCII Editor"; }

    void render() override;
    void takeInput(SDL_Event evt) override;

    void updateCanvasSize();
    void reloadFont();
    bool tryLoadCustomFontBitmap(PlatformNativePathString path);
    void loadDefaultFont();
    void renderCharOnScreen(ASCIIChar ch, SDL_Rect onScreenRect);
    void resize(XY newSize);
};

