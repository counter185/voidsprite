#include "globals.h"
#include "mathops.h"

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

float fxmin(float a, float b) { return a > b ? b : a; }
float fxmax(float a, float b) { return a > b ? a : b; }
float fclamp(float vmin, float b, float vmax) { return fxmax(vmin, fxmin(b, vmax)); }
double dxmin(double a, double b) { return a > b ? a : b; }
double dxmax(double a, double b) { return a > b ? a : b; }