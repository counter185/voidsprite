#pragma once
#include "BaseScreen.h"
#include "Canvas.h"
#include "Layer.h"
#include "PanelUserInteractable.h"

#define pushPixelFunction std::function<void(u32)>

inline std::unordered_map<std::string, std::function<void(FILE*, pushPixelFunction)>> pixelReaders = {
    {"I8",		[](FILE* f, pushPixelFunction ppx) {u8 i; fread(&i, 1, 1, f);       ppx(PackRGBAtoARGB(i,i,i,255)); }},
    {"A8",		[](FILE* f, pushPixelFunction ppx) {u8 i; fread(&i, 1, 1, f);       ppx(PackRGBAtoARGB(255,255,255,i)); }},
    {"IA88",	[](FILE* f, pushPixelFunction ppx) {u8 ia[2]; fread(ia, 1, 2, f);   ppx(PackRGBAtoARGB(ia[0], ia[0], ia[0], ia[1])); }},
    {"RGB24",	[](FILE* f, pushPixelFunction ppx) { u8 c[3]; fread(c, 1, 3, f);    ppx(PackRGBAtoARGB(c[0], c[1], c[2], 0xff)); }},
    {"BGR24",	[](FILE* f, pushPixelFunction ppx) { u8 c[3]; fread(c, 1, 3, f);    ppx(PackRGBAtoARGB(c[2], c[1], c[0], 0xff)); }},
    {"RGBA32",	[](FILE* f, pushPixelFunction ppx) { u8 c[4]; fread(c, 1, 4, f);    ppx(PackRGBAtoARGB(c[0], c[1], c[2], c[3])); }},
    {"RGBX32",	[](FILE* f, pushPixelFunction ppx) { u8 c[4]; fread(c, 1, 4, f);    ppx(PackRGBAtoARGB(c[0], c[1], c[2], 0xff)); }},
    {"ARGB32",	[](FILE* f, pushPixelFunction ppx) { u8 c[4]; fread(c, 1, 4, f);    ppx(PackRGBAtoARGB(c[1], c[2], c[3], c[0])); }},
    {"XRGB32",	[](FILE* f, pushPixelFunction ppx) { u8 c[4]; fread(c, 1, 4, f);    ppx(PackRGBAtoARGB(c[1], c[2], c[3], 0xff)); }},
    {"ABGR32",	[](FILE* f, pushPixelFunction ppx) { u8 c[4]; fread(c, 1, 4, f);    ppx(PackRGBAtoARGB(c[3], c[2], c[1], c[0])); }},
    {"RGB565",	[](FILE* f, pushPixelFunction ppx) { u16 c; fread(&c, 2, 1, f);     ppx(RGB565toARGB8888(c)); }},
};

enum PixelOrder : int {
    PO_XthenY = 0,
    PO_YthenX = 1,
};

class ExtractDataParametersPanel : public PanelUserInteractable {
private:
    ExtractDataScreen* caller;
public:
    UIButton *hPlusButton = NULL,
             *hMinusButton = NULL,
             *wPlusButton = NULL,
             *wMinusButton = NULL,
             *offsetPlusButton = NULL,
             *offsetMinusButton = NULL;

    ExtractDataParametersPanel(ExtractDataScreen* parent);

};

class ExtractDataScreen : public BaseScreen
{
private:
    PlatformNativePathString filePath;
    FILE* fileHandle = NULL;
    Canvas c;
    bool pan = false;
    bool zMod = false;
    bool qMod = false;

    Layer* dataLayer = NULL;
    std::string currentPixelFormat = "RGB24";
    PixelOrder currentPixelOrder = PO_XthenY;
    XY onCanvasEndPosition = { 0,0 };
    u64 fileOffset = 0;
    u32 layerWidth = 256;
    u32 layerHeight = 256;
    bool pixelDataChanged = true;

    ExtractDataParametersPanel* parametersPanel = NULL;

public:
    ExtractDataScreen(PlatformNativePathString file) : filePath(file) {
        c = Canvas(XY{ 1,1 });
        parametersPanel = new ExtractDataParametersPanel(this);
        parametersPanel->position = { 20, 50 };
        wxsManager.addDrawable(parametersPanel);

        fileHandle = platformOpenFile(filePath, PlatformFileModeRB);
        dimensionsUpdated();
        c.recenter();
    }

    void render() override;
    void takeInput(SDL_Event evt) override;
    std::string getName() override { return TL("vsp.extractdata") + ": " + fileNameFromPath(convertStringToUTF8OnWin32(filePath)); }

    void processLayer();

    std::string getCurrentPixelFormat() { return currentPixelFormat; }
    void setCurrentPixelFormat(std::string format) {
        currentPixelFormat = format;
        pixelDataChanged = true;
    }
    u64 getCurrentFileOffset() { return fileOffset; }
    void setCurrentFileOffset(u64 offset) {
        fileOffset = offset;
        pixelDataChanged = true;
    }
    int getLayerWidth() { return layerWidth; }
    int getLayerHeight() { return layerHeight; }
    void setLayerWidth(int w) {
        if (w < 1) w = 1;
        layerWidth = w;
        dimensionsUpdated();
    }
    void setLayerHeight(int h) {
        if (h < 1) h = 1;
        layerHeight = h;
        dimensionsUpdated();
    }

    PixelOrder getCurrentPixelOrder() { return currentPixelOrder; }
    void setCurrentPixelOrder(PixelOrder order) {
        currentPixelOrder = order;
        pixelDataChanged = true;
    }

    void renderBackground();
    void dimensionsUpdated() {
        if (dataLayer != NULL) {
            delete dataLayer;
        }
        dataLayer = Layer::tryAllocLayer(layerWidth, layerHeight);
        c.dimensions = XY{ (int)layerWidth, (int)layerHeight };
        pixelDataChanged = true;
    }
};

