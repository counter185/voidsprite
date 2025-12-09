#include <regex>
#include "globals.h"
#include "mathops.h"
#include "FontRenderer.h"
#include "shiftjis.h"

#define _USE_MATH_DEFINES
#include <math.h>

bool pointInBox(XY point, SDL_Rect rect) {
    XY normalP = { point.x - rect.x, point.y - rect.y };
    return 
        (rect.w >= 0 ? (normalP.x >= 0) : (normalP.x <= 0)) && normalP.x < rect.w
        && (rect.h >= 0 ? (normalP.y >= 0) : (normalP.y <= 0)) && normalP.y < rect.h;
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
u64 encodeXY(XY a) {
    u64 ret = 0;
    ret |= a.x;
    ret <<= 32;
    ret |= a.y;
    return ret;
}
XY decodeXY(u64 enc) {
    XY ret;
    ret.y = enc & 0xFFFFFFFF;
    ret.x = enc >> 32;
    return ret;
}

SDL_FPoint xytofp(XY p)
{
    return {(float)p.x, (float)p.y};
}

SDL_Rect offsetRect(SDL_Rect r, int offset)
{
    return {
        r.x - offset,
        r.y - offset,
        r.w + offset * 2,
        r.h + offset * 2
    };
}
SDL_Rect offsetRect(SDL_Rect r, int offsetX, int offsetY)
{
    return {
        r.x - offsetX,
        r.y - offsetY,
        r.w + offsetX * 2,
        r.h + offsetY * 2
    };
}

double angleBetweenTwoPoints(XY a, XY b)
{
    return atan2(b.y - a.y, b.x - a.x) / M_PI * 180 + 180;
}

std::vector<std::string> splitString(std::string a, char b)
{
    std::vector<std::string> ret;
    while (a.find(b) != std::string::npos) {
        std::string part = a.substr(0, a.find(b));
        ret.push_back(part);
        a = a.substr(a.find(b) + 1);
    }
    ret.push_back(a);
    return ret;
}

std::string stringToLower(std::string a)
{
    std::string ret;
    for (int i = 0; i < a.size(); i++) {
        ret += tolower(a.at(i));
    }
    return ret;
}

std::string shiftJIStoUTF8(std::string a)
{
    std::string ret;

    int stringPtr = 0;
    while (stringPtr < a.size()) {
        uint16_t sjisChar = a[stringPtr++];
        if (shiftjisToUTF8Map.contains(sjisChar)) {
            ret += shiftjisToUTF8Map[sjisChar];
        }
        else {
            sjisChar = (sjisChar << 8) + (uint8_t)a[stringPtr++];
            if (shiftjisToUTF8Map.contains(sjisChar)) {
                ret += shiftjisToUTF8Map[sjisChar];
            }
            else {
                ret += '?';
            }
        }
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
std::string wstringToUTF8String(std::wstring a)
{
    std::string ret = "";

    for (wchar_t& c : a) {
        if (c <= 0x7f) {
            ret += (char)c;
        }
        else if (c <= 0x7FF) {
            char c1 = 0b11000000 + (c >> 6);
            char c2 = 0b10000000 + (c & 0b111111);
            ret += c1;
            ret += c2;
        }
        else if (c <= 0xFFFF) {
            char c1 = 0b11100000 + (c >> 12);
            char c2 = 0b10000000 + ((c >> 6) & 0b111111);
            char c3 = 0b10000000 + (c & 0b111111);
            ret += c1;
            ret += c2;
            ret += c3;
        }
        else if (c <= 0x10FFFF) {
            char c1 = 0b11110000 + (c >> 18);
            char c2 = 0b10000000 + ((c >> 12) & 0b111111);
            char c3 = 0b10000000 + ((c >> 6) & 0b111111);
            char c4 = 0b10000000 + (c & 0b111111);
            ret += c1;
            ret += c2;
            ret += c3;
            ret += c4;
        }
        else {
            ret += '?';
        }
    }

    return ret;
}

std::string convertStringToUTF8OnWin32(PlatformNativePathString a)
{
#if _WIDEPATHS
    return wstringToUTF8String(a);
#else
    return a;
#endif
}

PlatformNativePathString convertStringOnWin32(std::string a) {
#if _WIDEPATHS
    return utf8StringToWstring(a);
#else
    return a;
#endif
}

bool stringStartsWith(std::string c, std::string startsWith) {
    if (c.size() < startsWith.size()) {
        return false;
    }
    return c.substr(0, startsWith.size()) == startsWith;
}

bool stringStartsWithIgnoreCase(std::string c, std::string startsWith)
{
    if (c.size() < startsWith.size()) {
        return false;
    }
    auto otherString = c.substr(0, startsWith.size());
    std::transform(otherString.begin(), otherString.end(), otherString.begin(), ::tolower);
    std::transform(startsWith.begin(), startsWith.end(), startsWith.begin(), ::tolower);
    return otherString == startsWith;
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

bool stringContainsIgnoreCase(std::string c, std::string contains)
{
    std::transform(c.begin(), c.end(), c.begin(), ::tolower);
    std::transform(contains.begin(), contains.end(), contains.begin(), ::tolower);
    return c.find(contains) != std::string::npos;
}

std::string evalRelativePath(std::string directory, std::string file) {
    std::string basePath = directory.substr(0, directory.find_last_of("/\\") + 1);
    std::string output = "";
    std::string commonPath = basePath.substr(0, basePath.size() - 1);
    while (commonPath.find_last_of("/\\") != std::string::npos && !stringStartsWith(file, commonPath)) {
        commonPath = commonPath.substr(0, commonPath.find_last_of("/\\"));
        output += "../";
    }
    return output + file.substr(1 + commonPath.size());
}

std::string fileNameFromPath(std::string fullPath)
{
    if (fullPath.find_last_of("/\\") == std::string::npos) {
        return fullPath;
    }
    else {
        return fullPath.substr(fullPath.find_last_of("/\\") + 1);
    }
}

PlatformNativePathString appendPath(PlatformNativePathString parent, PlatformNativePathString subdir)
{
    PlatformNativePathString separator =
#if _WIN32
        L"\\"
#else
        "/"
#endif
        ;
    PlatformNativePathString ret = parent;
    if (parent.size() == 0) {
        return subdir;
    }
    if (parent.size() > 0 && parent[parent.size() - 1] != '/' && parent[parent.size() - 1] != '\\') {
        ret += separator;
    }
    ret += subdir;
    return ret;
}

XY getSnappedPoint(XY from, XY to) {
    double ang = angleBetweenTwoPoints(from, to);
    if ((ang > 70 && ang < 110) || (ang > 250 && ang < 290)) {
        return { from.x, to.y };
    }
    else if ((ang < 20 || ang > 340) || (ang > 160 && ang < 200)) {
        return { to.x, from.y };
    }
    else {
        if ((ang > 90 && ang < 180) || (ang < 360 && ang > 270)) {
            //x+b
            XY pdiff = { to.x - from.x, from.y - to.y };
            int diffs[2] = { pdiff.x, pdiff.y };
            int diff = abs(diffs[0]) > abs(diffs[1]) ? diffs[0] : diffs[1];
            return { from.x + diff, from.y - diff };
        }
        else {
            //-x+b
            XY pdiff = { to.x - from.x, to.y - from.y };
            int diffs[2] = { pdiff.x, pdiff.y };
            int diff = abs(diffs[0]) > abs(diffs[1]) ? diffs[0] : diffs[1];
            return { from.x + diff, from.y + diff };
        }
    }
}

XYZd xyzdAdd(XYZd p1, XYZd p2)
{
    return { p1.x + p2.x, p1.y + p2.y, p1.z + p2.z };
}

double xyzdDistance(XYZd p1, XYZd p2)
{
    return sqrt(
        pow(p1.x - p2.x, 2)
        + pow(p1.y - p2.y, 2)
        + pow(p1.z - p2.z, 2)
    );
}

void rasterizeLine(XY from, XY to, std::function<void(XY)> forEachPixel, int arc, bool ceilLine)
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
                int yPos = (int)round(a * x + b);
                //SetPixel(XY{ x, yPos }, color);
                forEachPixel(XY{ x, yPos });
            }
        }
        else {
            float a = (float)(from.x - to.x) / (from.y - to.y);
            float b = from.x - a * from.y;

            for (int x = yMin; x <= yMax; x++) {
                int xPos = (int)round(a * x + b);
                //SetPixel(XY{ xPos, x }, color);
                forEachPixel(XY{ xPos, x });
            }
        }
    }
}
void rasterizeDiamond(XY from, XY to, std::function<void(XY)> forEachPixel)
{
    XY pointTop = XY{(from.x + to.x) / 2, from.y};
    XY pointRight = XY{to.x, (from.y + to.y) / 2};
    XY pointBottom = XY{(from.x + to.x) / 2, to.y};
    XY pointLeft = XY{from.x, (from.y + to.y) / 2};

    rasterizeLine(pointTop, pointRight, forEachPixel);
    rasterizeLine(pointRight, pointBottom, forEachPixel);
    rasterizeLine(pointBottom, pointLeft, forEachPixel);
    rasterizeLine(pointLeft, pointTop, forEachPixel);
}
void rasterizeEllipse(XY posMin, XY posMax, std::function<void(XY)> forEachPixel) {
    XY calcPosMax = xyAdd(posMax, { 1,1 });
    XY dimensions = xySubtract(calcPosMax, posMin);
    int b = dimensions.y / 2;
    int a = dimensions.x / 2;
    bool xEven = dimensions.x % 2 == 0;
    bool yEven = dimensions.y % 2 == 0;
    //g_fnt->RenderString(frmt("min={},{}  max={},{}   a={}  b={},  even={} {}", posMin.x, posMin.y, posMax.x, posMax.y, a, b, xEven, yEven), 50, 50);

    XY lastPTL, lastPTR, lastPBL, lastPBR;
    bool lastPointsSet = false;
    if (a == 0) {
        return;
    }

    std::map<u64, bool> pixelsToPlace;

    
    for (int ap = 0; xEven ? (ap < a) : (ap <= a); ap++) {
        double ySq = (b * b) * ((double)(ap * ap) / pow(a + (xEven ? -1 : 0), 2) - 1) * -1;
        double y = sqrt(ySq);
        //std::cout << ap << ":  " << ySq << "  " << y << std::endl;
        XY finalPxPositionTopRight = {posMin.x + a + ap, posMin.y + b - (int)y};

        int rdy = (int)round(y);

        XY pTL = { posMin.x + a - (xEven ? 1 : 0) - ap, posMin.y + b - rdy };
        XY pTR = { posMin.x + a + ap, posMin.y + b - rdy };
        XY pBL = { posMin.x + a - (xEven ? 1 : 0) - ap, posMin.y + b - (yEven ? 1 : 0) + rdy };
        XY pBR = { posMin.x + a + ap, posMin.y + b - (yEven ? 1 : 0) + rdy };

        if (ap > 0) {
            rasterizeLine(lastPTL, pTL, [&](XY px) { 
                pixelsToPlace[encodeXY(px)] = true;
            });
            rasterizeLine(lastPTR, pTR, [&](XY px) { 
                pixelsToPlace[encodeXY(px)] = true;
            });
            rasterizeLine(lastPBL, pBL, [&](XY px) { 
                pixelsToPlace[encodeXY(px)] = true;
            });
            rasterizeLine(lastPBR, pBR, [&](XY px) { 
                pixelsToPlace[encodeXY(px)] = true;
            });
        }
        lastPTL = pTL;
        lastPTR = pTR;
        lastPBL = pBL;
        lastPBR = pBR;
        lastPointsSet = true;
    }

    if (lastPointsSet) {
        rasterizeLine(lastPTL, lastPBL, [&](XY px) {
            pixelsToPlace[encodeXY(px)] = true;
        });
        rasterizeLine(lastPTR, lastPBR, [&](XY px) {
            pixelsToPlace[encodeXY(px)] = true;
        });
    }
    /*std::map<int, bool> xPositions;
    rasterizeSplitEllipseByY(posMin, posMax,
        [&](XY px) { 
            pixelsToPlace[encodeXY(px)] = true; 
            //forEachPixel(px);
            xPositions[px.x] = true;
        });
    rasterizeSplitEllipse(posMin, posMax,
        [&](XY px) { 
            if (!xPositions.contains(px.x)) {
                pixelsToPlace[encodeXY(px)] = true;
                //forEachPixel(px);
            }
        });*/

    for (auto& px : pixelsToPlace) {
        forEachPixel(decodeXY(px.first));
    }
    
}
void rasterizeSplitEllipse(XY posMin, XY posMax, std::function<void(XY)> forEachPixel) {
    XY calcPosMax = xyAdd(posMax, { 1,1 });
    XY dimensions = xySubtract(calcPosMax, posMin);
    int b = dimensions.y / 2;
    int a = dimensions.x / 2;
    bool xEven = dimensions.x % 2 == 0;
    bool yEven = dimensions.y % 2 == 0;
    //g_fnt->RenderString(frmt("min={},{}  max={},{}   a={}  b={},  even={} {}",posMin.x, posMin.y, posMax.x, posMax.y,  a,b, xEven, yEven), 50, 50);

    if (a == 0) {
        return;
    }

    for (int ap = 0; xEven ? (ap < a) : (ap <= a); ap++) {
        double ySq = (b * b) * ((double)(ap * ap) / (a * a) - 1) * -1;
        double y = sqrt(ySq);
        //std::cout << ap << ":  " << ySq << "  " << y << std::endl;
        int yy = (int)round(y);

        forEachPixel({ posMin.x + a + ap, posMin.y + b - (yEven ? 1 : 0) + yy });
        forEachPixel({ posMin.x + a - (xEven ? 1 : 0) - ap, posMin.y + b - (yEven ? 1 : 0) + yy});

        forEachPixel({ posMin.x + a + ap, posMin.y + b - yy});
        forEachPixel({ posMin.x + a - (xEven ? 1 : 0) - ap, posMin.y + b - yy });
    }
}

void rasterizeSplitEllipseByY(XY posMin, XY posMax, std::function<void(XY)> forEachPixel) {
    XY calcPosMax = xyAdd(posMax, { 1,1 });
    XY dimensions = xySubtract(calcPosMax, posMin);
    dimensions = { dimensions.y, dimensions.x };
    int b = dimensions.y / 2;
    int a = dimensions.x / 2;
    bool xEven = dimensions.x % 2 == 0;
    bool yEven = dimensions.y % 2 == 0;
    //g_fnt->RenderString(frmt("min={},{}  max={},{}   a={}  b={},  even={} {}",posMin.x, posMin.y, posMax.x, posMax.y,  a,b, xEven, yEven), 50, 50);

    if (a == 0) {
        return;
    }

    for (int ap = 0; xEven ? (ap < a) : (ap <= a); ap++) {
        double ySq = (b * b) * ((double)(ap * ap) / (a * a) - 1) * -1;
        double y = sqrt(ySq);
        int yy = (int)round(y);

        forEachPixel({ posMin.x + b - (yEven ? 1 : 0) + yy,  posMin.y + a + ap });
        forEachPixel({ posMin.x + b - (yEven ? 1 : 0) + yy,  posMin.y + a - (xEven ? 1 : 0) - ap});

        forEachPixel({ posMin.x + b - yy,                    posMin.y + a + ap, });
        forEachPixel({ posMin.x + b - yy,                    posMin.y + a - (xEven ? 1 : 0) - ap });
    }
    
}

void rasterizeBezierCurve(std::vector<XY> points, std::function<void(XY)> forEachPixel)
{
    const double accuracy = 0.05;
    if (points.size() >= 2) {
        XY prev;
        for (double i = 0; i < 1; i += accuracy) {
            XY p = evalBezierPoint(points, i);
            if (i != 0) {
                rasterizeLine(prev, p, [&](XY a) {
                    forEachPixel(a);
                });
                //forEachPixel(p);
            }
            prev = p;
        }
        if (points.size() > 2) {
            rasterizeLine(prev, points[points.size()-1], [&](XY a) {
                forEachPixel(a);
            });
        }
    }
}

XY statLineEndpoint(XY p1, XY p2, double percent) {
    if (percent != 1.0 && percent >= 0.0 && percent < 1.0) {
        p2.x = p1.x + round((p2.x - p1.x) * percent);
        p2.y = p1.y + round((p2.y - p1.y) * percent);
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

XY evalBezierPoint(std::vector<XY> vtx, double percent)
{
    if (vtx.size() < 2) {
        // ???
        return { 0, 0 };
    }
    if (vtx.size() == 2) {
        return statLineEndpoint(vtx[0], vtx[1], percent);
    }
    else {
        std::vector<XY> vtxm1;
        for (int x = 0; x < vtx.size() - 1; x++) {
            vtxm1.push_back(statLineEndpoint(vtx[x], vtx[x + 1], percent));
        }
        return evalBezierPoint(vtxm1, percent);
    }
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
    verts[0].color = toFColor(uint32ToSDLColor(colorUL));
    verts[1].position = xytofp({ bounds.x + bounds.w, bounds.y });
    verts[1].color = toFColor(uint32ToSDLColor(colorUR));
    verts[2].position = xytofp({ bounds.x, bounds.y + bounds.h });
    verts[2].color = toFColor(uint32ToSDLColor(colorDL));
    verts[3].position = xytofp({ bounds.x + bounds.w, bounds.y + bounds.h });
    verts[3].color = toFColor(uint32ToSDLColor(colorDR));

    int indices[6] = { 0, 1, 2, 1, 3, 2 };
    SDL_RenderGeometry(g_rd, NULL, verts, 4, indices, 6);

}

void renderGradient(XY ul, XY ur, XY dl, XY dr, uint32_t colorUL, uint32_t colorUR, uint32_t colorDL, uint32_t colorDR)
{
    SDL_Vertex verts[4];
    verts[0].position = xytofp(ul);
    verts[0].color = toFColor({(u8)((colorUL & 0xFF0000) >> 16), (u8)((colorUL & 0x00FF00) >> 8),
                              (u8)(colorUL & 0x0000FF), (u8)((colorUL & 0xFF000000) >> 24)});
    verts[1].position = xytofp(ur);
    verts[1].color = toFColor({(u8)((colorUR & 0xFF0000) >> 16), (u8)((colorUR & 0x00FF00) >> 8),
                              (u8)(colorUR & 0x0000FF), (u8)((colorUR & 0xFF000000) >> 24)});
    verts[2].position = xytofp(dl);
    verts[2].color = toFColor({(u8)((colorDL & 0xFF0000) >> 16), (u8)((colorDL & 0x00FF00) >> 8),
                              (u8)(colorDL & 0x0000FF), (u8)((colorDL & 0xFF000000) >> 24)});
    verts[3].position = xytofp(dr);
    verts[3].color = toFColor({(u8)((colorDR & 0xFF0000) >> 16), (u8)((colorDR & 0x00FF00) >> 8),
                              (u8)(colorDR & 0x0000FF), (u8)((colorDR & 0xFF000000) >> 24)});

    int indices[6] = { 0, 1, 2, 1, 3, 2 };
    SDL_RenderGeometry(g_rd, NULL, verts, 4, indices, 6);

}

std::string pathInProgramDirectory(std::string path)
{
    return (std::string(VOIDSPRITE_ASSETS_PATH).empty() ? g_programDirectory : std::string(VOIDSPRITE_ASSETS_PATH)) + path;
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

SDL_FColor toFColor(SDL_Color c) {
    return SDL_FColor{c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f};
}

SDL_Color rgb2sdlcolor(rgb a) {
    return SDL_Color{ (u8)(a.r * 255), (u8)(a.g * 255), (u8)(a.b * 255), 255 };
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
//AARRGGBB
SDL_Color uint32ToSDLColor(u32 c) {
    return SDL_Color{ (uint8_t)((c & 0xFF0000) >> 16), (uint8_t)((c & 0x00FF00) >> 8), (uint8_t)(c & 0x0000FF),
                     (uint8_t)((c & 0xFF000000) >> 24) };
}

uint32_t modAlpha(uint32_t color, uint8_t alpha)
{
    return (color & 0x00FFFFFF) + (alpha << 24);
}

SDL_Surface* trimSurface(SDL_Surface* target, SDL_Rect dims)
{
    SDL_Surface* ret = SDL_CreateSurface(dims.w, dims.h, target->format);
    int bpp = (target->pitch / target->w);
    for (int y = 0; y < dims.h; y++) {
        u8* srcScanline = &ARRAY2DPOINT(((u8*)target->pixels), dims.x*bpp, dims.y + y, target->pitch);
        u8* dstScanline = &ARRAY2DPOINT(((u8*)ret->pixels), 0, y, ret->pitch);
        memcpy(dstScanline, srcScanline, dims.w * bpp);
    }
    return ret;
}

void copyPixelsToTexture(u32* pixels, int w, int h, u8* dst, int dstPitch)
{
    if (dstPitch == w * 4) {
        memcpy(dst, pixels, w * h * 4);
    }
    else {
        for (int y = 0; y < h; y++) {
            u8* dstScanline = &ARRAY2DPOINT(dst, 0, y, dstPitch);
            u32* srcScanline = &ARRAY2DPOINT(pixels, 0, y, w);
            memcpy(dstScanline, srcScanline, w * 4);
        }
    }
}

u32 hsvShift(u32 color, hsv shift)
{
    rgb colorRGB = { (float)((color & 0xFF0000) >> 16) / 255.0f, (float)((color & 0x00FF00) >> 8) / 255.0f, (float)(color & 0x0000FF) / 255.0f };
    hsv colorHSV = rgb2hsv(colorRGB);
    colorHSV.h += shift.h;
    colorHSV.s += shift.s;
    colorHSV.v += shift.v;
    if (colorHSV.h > 360) { colorHSV.h -= 360; }
    if (colorHSV.s > 1) { colorHSV.s = 1; }
    if (colorHSV.v > 1) { colorHSV.v = 1; }
    if (colorHSV.h < 0) { colorHSV.h += 360; }
    if (colorHSV.s < 0) { colorHSV.s = 0; }
    if (colorHSV.v < 0) { colorHSV.v = 0; }
    rgb outRGB = hsv2rgb(colorHSV);
    return PackRGBAtoARGB((uint8_t)(outRGB.r * 255), (uint8_t)(outRGB.g * 255), (uint8_t)(outRGB.b * 255), (color & 0xFF000000) >> 24);
}

u32 hslShift(u32 color, hsl shift)
{
    rgb colorRGB = { (float)((color & 0xFF0000) >> 16) / 255.0f, (float)((color & 0x00FF00) >> 8) / 255.0f, (float)(color & 0x0000FF) / 255.0f };
    hsl colorHSL = rgb2hsl(colorRGB);
    colorHSL.h += shift.h;
    colorHSL.s += shift.s;
    colorHSL.l += shift.l;
    if (colorHSL.h > 360) { colorHSL.h -= 360; }
    if (colorHSL.s > 1) { colorHSL.s = 1; }
    if (colorHSL.l > 1) { colorHSL.l = 1; }
    if (colorHSL.h < 0) { colorHSL.h += 360; }
    if (colorHSL.s < 0) { colorHSL.s = 0; }
    if (colorHSL.l < 0) { colorHSL.l = 0; }
    rgb outRGB = hsl2rgb(colorHSL);
    return PackRGBAtoARGB((uint8_t)(outRGB.r * 255), (uint8_t)(outRGB.g * 255), (uint8_t)(outRGB.b * 255), (color & 0xFF000000) >> 24);
}

u32 hslShiftPixelStudioCompat(u32 ccolor, hsl shift)
{
    rgb color = { (float)((ccolor & 0xFF0000) >> 16) / 255.0f, (float)((ccolor & 0x00FF00) >> 8) / 255.0f, (float)(ccolor & 0x0000FF) / 255.0f };
    float hue = shift.h / 180.0f;
    float saturation = shift.s;
    float lightness = shift.l;
    hsv nnum = rgb2hsv(color);
    float num = nnum.h / 360;
    float num2 = nnum.s;
    float num3 = nnum.v;
    num += hue / 2;
    if (num > 1)
    {
        num -= 1;
    }
    else if (num < 0)
    {
        num += 1;
    }
    color = hsv2rgb(hsv{ num * 360, num2, num3 });
    float num4 = 0.3f * color.r + 0.59f * color.g + 0.11f * color.b;
    color.r = num4 + (color.r - num4) * (saturation + 1);
    color.g = num4 + (color.g - num4) * (saturation + 1);
    color.b = num4 + (color.b - num4) * (saturation + 1);
    if (color.r < 0)
    {
        color.r = 0;
    }
    if (color.g < 0)
    {
        color.g = 0;
    }
    if (color.b < 0)
    {
        color.b = 0;
    }
    color.r += lightness;
    color.g += lightness;
    color.b += lightness;
    color.r = dxmin(1, color.r);
    color.g = dxmin(1, color.g);
    color.b = dxmin(1, color.b);

    return PackRGBAtoARGB((uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), (ccolor & 0xFF000000) >> 24);
}

u32 invertColor(u32 color)
{
    return PackRGBAtoARGB(255 - ((color & 0xFF0000) >> 16), 255 - ((color & 0x00FF00) >> 8), 255 - (color & 0x0000FF),
                          (color & 0xFF000000) >> 24);
}

int ixmin(int a, int b) { return a > b ? b : a; }
int ixmax(int a, int b) { return a > b ? a : b; }
int ixpow(int a, int b)
{
    if (b == 0) {
        return 1;
    }
    int ret = a;
    for (int i = 1; i < b; i++) {
        ret *= a;
    }
    return ret;
}
int iclamp(int vmin, int b, int vmax) { return ixmax(vmin, ixmin(b, vmax)); }
float fxmin(float a, float b) { return a > b ? b : a; }
float fxmax(float a, float b) { return a > b ? a : b; }
float fclamp(float vmin, float b, float vmax) { return fxmax(vmin, fxmin(b, vmax)); }
double dxmin(double a, double b) { return a > b ? b : a; }
double dxmax(double a, double b) { return a > b ? a : b; }
double dclamp(double vmin, double b, double vmax) { return dxmax(vmin, dxmin(b, vmax)); }


uint32_t BEtoLE32(uint32_t a)
{
    return (a >> 24) + ((a >> 8) & 0xff00) + ((a << 8) & 0xff0000) + (a << 24);
}

uint16_t BEtoLE16(uint16_t a)
{
    return (a >> 8) + (a << 8);
}

//https://www.quora.com/Can-half-precision-floats-be-used-in-C
float halfToFloat(uint16_t half)
{
    // Convert half-precision to float 
    uint32_t sign = half & 0x8000;
    uint32_t exponent = (half >> 10) & 0x1F;
    uint32_t mantissa = half & 0x3FF;

    if (exponent == 0 && mantissa == 0) {
        return 0.0f; // zero 
    }
    else if (exponent == 31) {
        return sign ? -INFINITY : INFINITY; // infinity 
    }
    else {
        exponent += 112; // adjust bias 
        mantissa <<= 13; // adjust mantissa 
        uint32_t f_bits = (sign << 16) | (exponent << 23) | mantissa;
        return *reinterpret_cast<float*>(&f_bits);
    }
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

uint32_t RGBA4444toARGB8888(uint16_t rgba4444) {
    uint8_t r = ((rgba4444 >> 12) & 0b1111) * 0x11;
    uint8_t g = ((rgba4444 >> 8) & 0b1111) * 0x11;
    uint8_t b = ((rgba4444 >> 4) & 0b1111) * 0x11;
    uint8_t a = (rgba4444 & 0b1111) * 0x11;
    return PackRGBAtoARGB(r, g, b, a);
}
uint32_t RGB565toARGB8888(uint16_t rgb565)
{
    uint8_t r = ((rgb565 >> 11) & 0b11111) * 0x8;
    uint8_t g = ((rgb565 >> 5) & 0b111111) * 0x4;
    uint8_t b = (rgb565 & 0b11111) * 0x8;
    return PackRGBAtoARGB(r, g, b, 255);
}
uint32_t BGR565toARGB8888(uint16_t bgr565)
{
    uint8_t b = ((bgr565 >> 11) & 0b11111) * 0x8;
    uint8_t g = ((bgr565 >> 5) & 0b111111) * 0x4;
    uint8_t r = (bgr565 & 0b11111) * 0x8;
    return PackRGBAtoARGB(r,g,b,255);
}

uint32_t RGB555toARGB8888(uint16_t rgb555) 
{
    uint8_t r = ((rgb555 >> 10) & 0b11111) * 0x8;
    uint8_t g = ((rgb555 >> 5) & 0b11111) * 0x8;
    uint8_t b = (rgb555 & 0b11111) * 0x8;
    return 0xFF000000 + (r << 16) + (g << 8) + b;
}

uint32_t BGR555toARGB8888(uint16_t bgr555)
{
    uint8_t b = ((bgr555 >> 10) & 0b11111) * 0x8;
    uint8_t g = ((bgr555 >> 5) & 0b11111) * 0x8;
    uint8_t r = (bgr555 & 0b11111) * 0x8;
    return 0xFF000000 + (r << 16) + (g << 8) + b;
}

uint32_t PackRGBAtoARGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (a << 24) + (r << 16) + (g << 8) + b;
}

std::vector<std::string> split(std::string a, char b)
{
    std::vector<std::string> ret;

    std::string current = "";
    for (char& c : a) {
        if (c == b) {
            ret.push_back(current);
            current = "";
        }
        else {
            current += c;
        }
    }
    ret.push_back(current);

    return ret;
}

int randomInt(int minIncl, int maxExcl)
{
    if (minIncl > maxExcl) {
        int t = minIncl;
        minIncl = maxExcl;
        maxExcl = t;
    }
    int range = maxExcl - minIncl;
    if (range == 0) {
        return minIncl;
    }
    if (range > RAND_MAX) {
        int s = 0;
        for (int s = 0; s < 1 + (range / RAND_MAX); s++) {
            range -= RAND_MAX;
            s += rand() % ixmin(range, RAND_MAX);
        }
        return s + minIncl;
    }
    return rand() % (maxExcl - minIncl) + minIncl;
}

std::string randomUUID()
{
    std::string chars = "0123456789abcdef";
    std::string ret = "";
    for (int i = 0; i < 32; i++) {
        ret += chars[rand() % 16];
    }
    ret.insert(8, "-");
    ret.insert(13, "-");
    ret.insert(18, "-");
    ret.insert(23, "-");
    return ret;
}

std::string secondsTimeToHumanReadable(u64 seconds)
{
    u64 s = seconds % 60;
    seconds /= 60;
    u64 m = seconds % 60;
    seconds /= 60;
    u64 h = seconds % 24;
    seconds /= 24;
    std::string ret = frmt("{:02}:{:02}:{:02}", h, m, s);

    if (seconds > 0) {
        u64 d = seconds;
        ret = frmt("{}d {}", d, ret);
    }
    return ret;
}

SDL_Event handleNumLockInEvent(SDL_Event src)
{
    std::map<u32, u32> scancodeMap = {
        {SDL_SCANCODE_KP_9, SDL_SCANCODE_PAGEUP},
        {SDL_SCANCODE_KP_3, SDL_SCANCODE_PAGEDOWN},
        {SDL_SCANCODE_KP_7, SDL_SCANCODE_HOME},
        {SDL_SCANCODE_KP_1, SDL_SCANCODE_END},
        {SDL_SCANCODE_KP_0, SDL_SCANCODE_INSERT}
    };
    std::map<u32, u32> keycodeMap = {
        {SDLK_KP_9, SDLK_PAGEUP},
        {SDLK_KP_3, SDLK_PAGEDOWN},
        {SDLK_KP_7, SDLK_HOME},
        {SDLK_KP_1, SDLK_END},
        {SDLK_KP_0, SDLK_INSERT}
    };

    switch (src.type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if ((src.key.mod & SDL_KMOD_NUM) == 0) {
                if (scancodeMap.contains(src.key.scancode)) {
                    src.key.scancode = (SDL_Scancode)scancodeMap[src.key.scancode];
                }
                if (keycodeMap.contains(src.key.key)) {
                    src.key.key = keycodeMap[src.key.key];
                }
            }
            break;
    }

    return src;
}

SDL_Event convertTouchToMouseEvent(SDL_Event src)
{
    SDL_Event ret = src;
    switch (src.type) {
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
        ret.type = src.type == SDL_FINGERDOWN ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
        ret.button.button = SDL_BUTTON_LEFT;
        ret.button.clicks = 1;
        ret.button.timestamp = src.tfinger.timestamp;
        ret.button.windowID = src.tfinger.windowID;
        ret.button.which = src.tfinger.touchID;
        ret.button.down = src.type == SDL_FINGERDOWN;
        ret.button.x = src.tfinger.x * g_windowW;
        ret.button.y = src.tfinger.y * g_windowH;
        break;
    case SDL_FINGERMOTION:
        ret.type = SDL_MOUSEMOTION;
        ret.motion.timestamp = src.tfinger.timestamp;
        ret.motion.windowID = src.tfinger.windowID;
        ret.motion.x = src.tfinger.x * g_windowW;
        ret.motion.y = src.tfinger.y * g_windowH;
        ret.motion.which = src.tfinger.touchID;
        ret.motion.state = 0;
        ret.motion.xrel = src.tfinger.dx * g_windowW;
        ret.motion.yrel = src.tfinger.dy * g_windowH;
        break;
    default:
        return src;
    }
    return ret;
}
SDL_Event scaleScreenPositionsInEvent(SDL_Event src) {
    if (g_renderScale == 1) {
        return src;
    }
    double scaleFactor = g_renderScale;
    std::function<void(std::vector<float*>, double)> divMultiple = 
        [](std::vector<float*> f, double ff) { for (auto*& p : f) { *p = *p / ff; } };
    switch (src.type) {
        case SDL_EVENT_MOUSE_MOTION:
            divMultiple({&src.motion.x, &src.motion.y, &src.motion.xrel, &src.motion.yrel}, scaleFactor);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            divMultiple({ &src.button.x, &src.button.y }, scaleFactor);
            break;
        case SDL_EVENT_FINGER_CANCELED:
        case SDL_EVENT_FINGER_DOWN:
        case SDL_EVENT_FINGER_MOTION:
        case SDL_EVENT_FINGER_UP:
            //these are normalized so don't scale them
            //divMultiple({&src.tfinger.x, &src.tfinger.y, &src.tfinger.dx, &src.tfinger.dy}, scaleFactor);
            break;
        case SDL_EVENT_PEN_AXIS:
            divMultiple({ &src.paxis.x, &src.paxis.y }, scaleFactor);
            break;
        case SDL_EVENT_PEN_MOTION:
            divMultiple({ &src.pmotion.x, &src.pmotion.y }, scaleFactor);
            break;
        case SDL_EVENT_PEN_DOWN:
        case SDL_EVENT_PEN_UP:
            divMultiple({ &src.ptouch.x, &src.ptouch.y }, scaleFactor);
            break;
        case SDL_EVENT_PEN_BUTTON_DOWN:
        case SDL_EVENT_PEN_BUTTON_UP:
            divMultiple({&src.pbutton.x, &src.pbutton.y}, scaleFactor);
            break;
    }
    return src;
}

SDL_Rect fitInside(SDL_Rect outer, SDL_Rect inner)
{
    if (outer.w == 0 || outer.h == 0 || inner.w == 0 || inner.h == 0) {
        return SDL_Rect{ 0,0,0,0 };
    }
    double aspectOuter = outer.w / (double)outer.h;
    double aspectInner = inner.w / (double)inner.h;
    SDL_Rect ret = { 0,0,0,0 };
    if (aspectOuter > aspectInner) {
        //letterbox sides
        ret.h = outer.h;
        ret.w = (int)(outer.h * aspectInner);
        ret.x = outer.x + (outer.w - ret.w) / 2;
        ret.y = outer.y;
    }
    else {
        //letterbox top/bottom
        ret.w = outer.w;
        ret.h = (int)(outer.w / aspectInner);
        ret.x = outer.x;
        ret.y = outer.y + (outer.h - ret.h) / 2;
    }
    return ret;
}

u32 parseIpAddress(std::string ipv4)
{
    std::smatch m;
    std::regex_match(ipv4, m, std::regex("^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})$"));
    if (m.size() != 5) {
        return 0;
    }
    int a = std::stoi(m[1]);
    int b = std::stoi(m[2]);
    int c = std::stoi(m[3]);
    int d = std::stoi(m[4]);
    if (a > 255 || b > 255 || c > 255 || d > 255) {
        return 0;
    }
    return (a << 24) + (b << 16) + (c << 8) + d;
}

std::string ipToString(u32 ipv4)
{
    return frmt("{}.{}.{}.{}", (ipv4 >> 24) & 0xff, (ipv4 >> 16) & 0xff, (ipv4 >> 8) & 0xff, ipv4&0xff);
}

matrix matrixMultiply(matrix a, matrix b)
{
    int resultW = b[0].size();
    int resultH = a.size();

    if (a[0].size() != b.size()) {
        //invalid matrix sizes
        return matrix();
    }

    matrix result;
    result.resize(resultH);
    for (int y = 0; y < resultH; y++) {
        result[y].resize(resultW);
        for (int x = 0; x < resultW; x++) {
            double c = 0;
            for (int k = 0; k < a[0].size(); k++) {
                c += a[y][k] * b[k][x];
            }
            result[y][x] = c;
        }
    }
    return result;
}

hsl rgb2hsl(rgb c) {

    hsl result;

    double max = dxmax(dxmax(c.r, c.g), c.b);
    double min = dxmin(dxmin(c.r, c.g), c.b);

    result.h = result.s = result.l = (max + min) / 2;

    if (max == min) {
        result.h = result.s = 0; // achromatic
    }
    else {
        double d = max - min;
        result.s = (result.l > 0.5) ? d / (2 - max - min) : d / (max + min);

        if (max == c.r) {
            result.h = (c.g - c.b) / d + (c.g < c.b ? 6 : 0);
        }
        else if (max == c.g) {
            result.h = (c.b - c.r) / d + 2;
        }
        else if (max == c.b) {
            result.h = (c.r - c.g) / d + 4;
        }

        result.h /= 6;
    }

    return result;

}

//i have 0 idea what this even does
double hue2rgb(double p, double q, double t) {

    if (t < 0)
        t += 1;
    if (t > 1)
        t -= 1;
    if (t < 1. / 6)
        return p + (q - p) * 6 * t;
    if (t < 1. / 2)
        return q;
    if (t < 2. / 3)
        return p + (q - p) * (2. / 3 - t) * 6;

    return p;

}

rgb hsl2rgb(hsl c) {

    rgb result;

    if (0 == c.s) {
        result.r = result.g = result.b = c.l; // achromatic
    }
    else {
        float q = c.l < 0.5 ? c.l * (1 + c.s) : c.l + c.s - c.l * c.s;
        float p = 2 * c.l - q;
        result.r = hue2rgb(p, q, c.h + 1. / 3);
        result.g = hue2rgb(p, q, c.h);
        result.b = hue2rgb(p, q, c.h - 1. / 3);
    }

    return result;

}

rgb sdlColorToRGB(SDL_Color c)
{
    return u32ToRGB(PackRGBAtoARGB(c.r, c.g, c.b, c.a));
}

rgb u32ToRGB(u32 color)
{
    return {(float)((color & 0xFF0000) >> 16) / 255.0f, (float)((color & 0x00FF00) >> 8) / 255.0f,
            (float)(color & 0x0000FF) / 255.0f};
}
