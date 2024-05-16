#include "globals.h"
#include "mathops.h"
#include "FontRenderer.h"

bool pointInBox(XY point, SDL_Rect rect) {
    XY normalP = { point.x - rect.x, point.y - rect.y };
    return normalP.x >= 0 && normalP.x < rect.w
        && normalP.y >= 0 && normalP.y < rect.h;
}

XY xyAdd(XY p1, XY p2)
{
    return XY{p1.x+p2.x, p1.y+p2.y};
}
XY xySubtract(XY p1, XY p2)
{
    return XY{p1.x-p2.x, p1.y-p2.y};
}

std::wstring utf8StringToWstring(std::string a)
{
    std::wstring ret;
    uint32_t currentUTF8Sym = 0;
    int nextUTFBytes = 0;
    for (int chx = 0; chx != a.size(); chx++) {
        char target = a.at(chx);
        bool done = ParseUTF8(target, &nextUTFBytes, currentUTF8Sym);
        if (done) {
            ret += (wchar_t)currentUTF8Sym;
        }
    }
    return ret;
}

bool stringEndsWith(std::string c, std::string endsWith)
{
    if (c.size() < endsWith.size()) {
        return false;
    }
    return c.substr(c.size() - endsWith.size()) == endsWith;
}

void rasterizeLine(XY from, XY to, std::function<void(XY)> forEachPixel)
{
    if (from.x == to.x) {
        int yMin = from.y > to.y ? to.y : from.y;
        int yMax = from.y > to.y ? from.y : to.y;
        for (int y = yMin; y <= yMax; y++) {
            //SetPixel(XY{ from.x, y }, color);
            forEachPixel(XY{ from.x, y });
        }
    }
    else {
        int xMin = ixmin(from.x, to.x);
        int xMax = ixmax(from.x, to.x);
        int yMin = ixmin(from.y, to.y);
        int yMax = ixmax(from.y, to.y);

        if (xMax - xMin > yMax - yMin) {
            float a = (float)(from.y - to.y) / (from.x - to.x);
            float b = from.y - a * from.x;

            for (int x = xMin; x <= xMax; x++) {
                int yPos = (int)(a * x + b);
                //SetPixel(XY{ x, yPos }, color);
                forEachPixel(XY{ x, yPos });
            }
        }
        else {
            float a = (float)(from.x - to.x) / (from.y - to.y);
            float b = from.x - a * from.y;

            for (int x = yMin; x <= yMax; x++) {
                int xPos = (int)(a * x + b);
                //SetPixel(XY{ xPos, x }, color);
                forEachPixel(XY{ xPos, x });
            }
        }
    }
    
}

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    }
    else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = nan("");                            // its now undefined
        return out;
    }
    if (in.r >= max)                           // > is bogus, just keeps compilor happy
        out.h = (in.g - in.b) / delta;        // between yellow & magenta
    else
        if (in.g >= max)
            out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
        else
            out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if (out.h < 0.0)
        out.h += 360.0;

    return out;
}

bool tryRgbStringToColor(std::string str, unsigned int* ret)
{
    //https://github.com/counter185/coreplus-game/blob/8fcd218730a7927c109645b949cefbd85798c651/CORERG_VC/corerg_overlay_editor_addevent.cpp#L98C17-L111C32
    unsigned int color = 0;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            color += (str[i] - '0') * (1 << (4 * (5 - i)));
        }
        else if (str[i] >= 'a' && str[i] <= 'f') {
            color += (str[i] - 'a' + 10) * (1 << (4 * (5 - i)));
        }
        else if (str[i] >= 'A' && str[i] <= 'F') {
            color += (str[i] - 'A' + 10) * (1 << (4 * (5 - i)));
        }
        else {
            return false;
        }
    }
    *ret = color;
    return true;
}

rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch (i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

SDL_Color rgb2sdlcolor(rgb a) {
    return SDL_Color{ (unsigned char)(a.r * 255), (unsigned char)(a.g * 255), (unsigned char)(a.b * 255), 255 };
}

unsigned int alphaBlend(unsigned int colora, unsigned int colorb) {
    unsigned int a2 = (colorb & 0xFF000000) >> 24;
    unsigned int alpha = a2;
    if (alpha == 0) return colora;
    if (alpha == 255) return colorb;
    unsigned int a1 = (colora & 0xFF000000) >> 24;
    unsigned int nalpha = 0x100 - alpha;
    unsigned int rb1 = (nalpha * (colora & 0xFF00FF)) >> 8;
    unsigned int rb2 = (alpha * (colorb & 0xFF00FF)) >> 8;
    unsigned int g1 = (nalpha * (colora & 0x00FF00)) >> 8;
    unsigned int g2 = (alpha * (colorb & 0x00FF00)) >> 8;
    unsigned int anew = a1 + a2;
    if (anew > 255) { anew = 255; }
    return ((rb1 + rb2) & 0xFF00FF) + ((g1 + g2) & 0x00FF00) + (anew << 24);
}

int ixmin(int a, int b) { return a > b ? b : a; }
int ixmax(int a, int b) { return a > b ? a : b; }
int iclamp(int vmin, int b, int vmax) { return ixmax(vmin, ixmin(b, vmax)); }
float fxmin(float a, float b) { return a > b ? b : a; }
float fxmax(float a, float b) { return a > b ? a : b; }
float fclamp(float vmin, float b, float vmax) { return fxmax(vmin, fxmin(b, vmax)); }
double dxmin(double a, double b) { return a > b ? a : b; }
double dxmax(double a, double b) { return a > b ? a : b; }

uint32_t BEtoLE32(uint32_t a)
{
    return (a >> 24) + ((a >> 8) & 0xff00) + ((a << 8) & 0xff0000) + (a << 24);
}

uint16_t BEtoLE16(uint16_t a)
{
    return (a >> 8) + (a << 8);
}

uint32_t RGB5A3toARGB8888(uint16_t rgb5a3Byte)
{
    uint32_t outPixel = 0;
    if (rgb5a3Byte >> 15 == 0) {
        uint8_t a = 0x20 * (rgb5a3Byte >> 12);
        uint8_t r = 0x11 * ((rgb5a3Byte >> 8) & 0b1111);
        uint8_t g = 0x11 * ((rgb5a3Byte >> 4) & 0b1111);
        uint8_t b = 0x11 * ((rgb5a3Byte) & 0b1111);
        outPixel = (a << 24) + (r << 16) + (g << 8) + b;
    }
    else {
        uint8_t a = 0xff;
        uint8_t r = 0x8 * ((rgb5a3Byte >> 10) & 0b11111);
        uint8_t g = 0x8 * ((rgb5a3Byte >> 5) & 0b11111);
        uint8_t b = 0x8 * ((rgb5a3Byte) & 0b11111);
        outPixel = (a << 24) + (r << 16) + (g << 8) + b;
    }
    return outPixel;
}

uint32_t RGB565toARGB8888(uint16_t rgb565)
{
    uint8_t r = ((rgb565 >> 11) & 0b11111) * 0x8;
    uint8_t g = ((rgb565 >> 5) & 0b111111) * 0x4;
    uint8_t b = (rgb565 & 0b11111) * 0x8;
    return 0xFF000000 + (r << 16) + (g << 8) + b;

    return 0;
}

uint32_t PackRGBAtoARGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (a << 24) + (r << 16) + (g << 8) + b;
}
