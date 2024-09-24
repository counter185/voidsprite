#include "globals.h"
#include "mathops.h"
#include "FontRenderer.h"

bool pointInBox(XY point, SDL_Rect rect) {
    XY normalP = { point.x - rect.x, point.y - rect.y };
    return normalP.x >= 0 && normalP.x < rect.w
        && normalP.y >= 0 && normalP.y < rect.h;
}

double xyDistance(XY p1, XY p2)
{
    double ret = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
    return ret;
}

bool xyEqual(XY p1, XY p2) {
    return p1.x == p2.x && p1.y == p2.y;
}
XY xyAdd(XY p1, XY p2)
{
    return XY{p1.x+p2.x, p1.y+p2.y};
}
XY xySubtract(XY p1, XY p2)
{
    return XY{p1.x-p2.x, p1.y-p2.y};
}

SDL_FPoint xytofp(XY p)
{
    return {(float)p.x, (float)p.y};
}

std::string stringToLower(std::string a)
{
    std::string ret;
	for (int i = 0; i < a.size(); i++) {
		ret += tolower(a.at(i));
	}
	return ret;
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
PlatformNativePathString convertStringOnWin32(std::string a) {
#if _WIDEPATHS
    return utf8StringToWstring(a);
#else
    return a;
#endif
}

//can't do this with templates
bool stringEndsWithIgnoreCase(std::wstring c, std::wstring endsWith)
{
    if (c.size() < endsWith.size()) {
        return false;
    }
    auto otherString = c.substr(c.size() - endsWith.size());
    std::transform(otherString.begin(), otherString.end(), otherString.begin(), ::tolower);
    std::transform(endsWith.begin(), endsWith.end(), endsWith.begin(), ::tolower);
    return otherString == endsWith;
}
bool stringEndsWithIgnoreCase(std::string c, std::string endsWith)
{
    if (c.size() < endsWith.size()) {
        return false;
    }
    auto otherString = c.substr(c.size() - endsWith.size());
    std::transform(otherString.begin(), otherString.end(), otherString.begin(), ::tolower);
    std::transform(endsWith.begin(), endsWith.end(), endsWith.begin(), ::tolower);
    return otherString == endsWith;
}

void rasterizeLine(XY from, XY to, std::function<void(XY)> forEachPixel, int arc)
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

        if ((arc == 0 && xMax - xMin > yMax - yMin) || arc == 1) {
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
void rasterizeEllipse(XY posMin, XY posMax, std::function<void(XY)> forEachPixel) {
    XY dimensions = xySubtract(posMax, posMin);
    int b = dimensions.y / 2;
    int a = dimensions.x / 2;
    XY lastP1, lastP2;
    if (a == 0) {
        return;
    }
    //printf("%i\n", a);
    //printf("a = %i  b = %i\n", a, b);
    for (int x = 0; x <= a; x++) {
        int xx = x - a;
        int yysq = (b * b) - (b * b * xx * xx) / (a * a);
        int yy = (int)sqrt(yysq);
        int y1 = yy + b;
        int y2 = -yy + b;
        //printf("x %i => y %i (^2 = %i)\n", xx, yy, yysq); 
        XY p1 = { x, y1 };
        XY p2 = { x, y2 };
        if (x > 0) {
            //printf("Line from (%i,%i) => (%i,%i)\n", lastP1.x, lastP1.y, p1.x, p1.y);
            rasterizeLine(lastP1, p1, [&](XY a) {
                forEachPixel(xyAdd(a, posMin));
                forEachPixel(XY{ posMax.x - a.x, posMin.y + a.y });
                });
            rasterizeLine(lastP2, p2, [&](XY a) {
                forEachPixel(xyAdd(a, posMin));
                forEachPixel(XY{ posMax.x - a.x, posMin.y + a.y });
                });
        }
        lastP1 = p1;
        lastP2 = p2;
        //editor->SetPixel(xyAdd(p1, posMin), editor->pickedColor);
        //editor->SetPixel(xyAdd(p2, posMin), editor->pickedColor);
        //editor->getCurrentLayer()->setPixel(xyAdd(XY{x,y1}, posMin), editor->pickedColor);
        //editor->getCurrentLayer()->setPixel(xyAdd(XY{x,y2}, posMin), editor->pickedColor);
    }
}

XY statLineEndpoint(XY p1, XY p2, double percent) {
    if (percent != 1.0 && percent >= 0.0 && percent < 1.0) {
        p2.x = p1.x + (p2.x - p1.x) * percent;
        p2.y = p1.y + (p2.y - p1.y) * percent;
    }
    return p2;
}

void drawLine(XY p1, XY p2, double percent)
{
    if (percent == 0.0) {
        return;
    }

    p2 = statLineEndpoint(p1, p2, percent);

    SDL_RenderDrawLine(g_rd, p1.x, p1.y, p2.x, p2.y);
}

/// <summary>
/// (x-1)^3 + 1
/// </summary>
double XM1PW3P1(double x)
{
    return (x - 1) * (x - 1) * (x - 1) + 1;
}

void renderGradient(SDL_Rect bounds, uint32_t colorUL, uint32_t colorUR, uint32_t colorDL, uint32_t colorDR)
{
    SDL_Vertex verts[4];
    verts[0].position = xytofp({ bounds.x, bounds.y });
    verts[0].color = { (uint8_t)((colorUL & 0xFF0000) >> 16), (uint8_t)((colorUL & 0x00FF00) >> 8), (uint8_t)(colorUL & 0x0000FF), (uint8_t)((colorUL & 0xFF000000) >> 24)};
    verts[1].position = xytofp({ bounds.x + bounds.w, bounds.y });
    verts[1].color = { (uint8_t)((colorUR & 0xFF0000) >> 16), (uint8_t)((colorUR & 0x00FF00) >> 8), (uint8_t)(colorUR & 0x0000FF), (uint8_t)((colorUR & 0xFF000000) >> 24) };
    verts[2].position = xytofp({ bounds.x, bounds.y + bounds.h });
    verts[2].color = { (uint8_t)((colorDL & 0xFF0000) >> 16), (uint8_t)((colorDL & 0x00FF00) >> 8), (uint8_t)(colorDL & 0x0000FF), (uint8_t)((colorDL & 0xFF000000) >> 24) };
    verts[3].position = xytofp({ bounds.x + bounds.w, bounds.y + bounds.h });
    verts[3].color = { (uint8_t)((colorDR & 0xFF0000) >> 16), (uint8_t)((colorDR & 0x00FF00) >> 8), (uint8_t)(colorDR & 0x0000FF), (uint8_t)((colorDR & 0xFF000000) >> 24) };

    int indices[6] = { 0, 1, 2, 1, 3, 2 };
    SDL_RenderGeometry(g_rd, NULL, verts, 4, indices, 6);

}

std::string pathInProgramDirectory(std::string path)
{
    return g_programDirectory + "/" + path;
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

uint32_t sdlcolorToUint32(SDL_Color c)
{
    return (c.a << 24) + (c.r << 16) + (c.g << 8) + c.b;
}

uint32_t modAlpha(uint32_t color, uint8_t alpha)
{
    return (color & 0x00FFFFFF) + (alpha << 24);
}

int ixmin(int a, int b) { return a > b ? b : a; }
int ixmax(int a, int b) { return a > b ? a : b; }
int iclamp(int vmin, int b, int vmax) { return ixmax(vmin, ixmin(b, vmax)); }
float fxmin(float a, float b) { return a > b ? b : a; }
float fxmax(float a, float b) { return a > b ? a : b; }
float fclamp(float vmin, float b, float vmax) { return fxmax(vmin, fxmin(b, vmax)); }
double dxmin(double a, double b) { return a > b ? b : a; }
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
