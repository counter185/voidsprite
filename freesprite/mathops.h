#pragma once

#include <math.h>

struct rgb {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
};

struct hsv {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
};

struct hsl {
    double h;
    double s;
    double l;
};


void renderGradient(SDL_Rect bounds, uint32_t colorUL, uint32_t colorUR, uint32_t colorDL, uint32_t colorDR);
void renderGradient(XY ul, XY ur, XY dl, XY dr, uint32_t colorUL, uint32_t colorUR, uint32_t colorDL, uint32_t colorDR);

std::string pathInProgramDirectory(std::string path);

//stolen code yay
//https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);

//https://gist.github.com/ciembor/1494530
hsl rgb2hsl(rgb c);
rgb hsl2rgb(hsl c);

rgb sdlColorToRGB(SDL_Color c);
rgb u32ToRGB(u32 color);

SDL_FColor toFColor(SDL_Color c);
SDL_Color rgb2sdlcolor(rgb a);
bool tryRgbStringToColor(std::string str, unsigned int* ret);
unsigned int alphaBlend(unsigned int colora, unsigned int colorb);
uint32_t sdlcolorToUint32(SDL_Color c);
SDL_Color uint32ToSDLColor(u32 c);
uint32_t modAlpha(uint32_t color, uint8_t alpha);

u32 hsvShift(u32 color, hsv shift);
u32 hslShift(u32 color, hsl shift);
u32 hslShiftPixelStudioCompat(u32 color, hsl shift);
u32 invertColor(u32 color);

bool pointInBox(XY point, SDL_Rect rect);

double xyDistance(XY p1, XY p2);
bool xyEqual(XY p1, XY p2);
XY xyAdd(XY p1, XY p2);
XY xySubtract(XY p1, XY p2);
u64 encodeXY(XY a);
XY decodeXY(u64 enc);
SDL_FPoint xytofp(XY p);
double angleBetweenTwoPoints(XY a, XY b);
XY getSnappedPoint(XY from, XY to);

std::string stringToLower(std::string a);
std::string shiftJIStoUTF8(std::string a);
std::wstring utf8StringToWstring(std::string a);
std::string wstringToUTF8String(std::wstring a);
std::string convertStringToUTF8OnWin32(PlatformNativePathString a);
PlatformNativePathString convertStringOnWin32(std::string a);

bool stringStartsWithIgnoreCase(std::string c, std::string startsWith);
bool stringEndsWithIgnoreCase(std::wstring c, std::wstring endsWith);
bool stringEndsWithIgnoreCase(std::string c, std::string endsWith);

std::string evalRelativePath(std::string directory, std::string file);
std::string fileNameFromPath(std::string fullPath);

void rasterizeLine(XY from, XY to, std::function<void(XY)> forEachPixel, int arc = 0, bool ceilLine = false);
void rasterizeDiamond(XY from, XY to, std::function<void(XY)> forEachPixel);
void rasterizeEllipse(XY posMin, XY posMax, std::function<void(XY)> forEachPixel);
void rasterizeSplitEllipse(XY posMin, XY posMax, std::function<void(XY)> forEachPixel);
void rasterizeSplitEllipseByY(XY posMin, XY posMax, std::function<void(XY)> forEachPixel);
void rasterizeBezierCurve(std::vector<XY> points, std::function<void(XY)> forEachPixel);

XY statLineEndpoint(XY p1, XY p2, double percent);
void drawLine(XY p1, XY p2, double percent = 1.0);
XY evalBezierPoint(std::vector<XY> vtx, double percent);
double XM1PW3P1(double x);

int ixmin(int a, int b);
int ixmax(int a, int b);
int ixpow(int a, int b);
int iclamp(int vmin, int b, int vmax);
float fxmin(float a, float b);
float fxmax(float a, float b);
float fclamp(float vmin, float b, float vmax);
double dxmin(double a, double b);
double dxmax(double a, double b);

uint32_t BEtoLE32(uint32_t a);
uint16_t BEtoLE16(uint16_t a);

float halfToFloat(uint16_t half);

uint32_t RGBA4444toARGB8888(uint16_t rgba4444);
uint32_t RGB5A3toARGB8888(uint16_t rgb5a3Byte);
uint32_t BGR565toARGB8888(uint16_t rgb5a3Byte);
uint32_t RGB565toARGB8888(uint16_t rgb5a3Byte);
uint32_t RGB555toARGB8888(uint16_t rgb555);
uint32_t BGR555toARGB8888(uint16_t bgr555);
uint32_t PackRGBAtoARGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

std::vector<std::string> split(std::string a, char b);

int randomInt(int minIncl, int maxExcl);
std::string randomUUID();
std::string secondsTimeToHumanReadable(u64 seconds);

SDL_Event convertTouchToMouseEvent(SDL_Event src);

template<typename T>
inline std::vector<T> joinVectors(std::initializer_list<std::vector<T>> vecs)
{
    std::vector<T> ret;
    for (const auto& vec : vecs) {
        ret.insert(ret.end(), vec.begin(), vec.end());
    }
    return ret;
}

class Bitblock {
private:
    u8* data;
    u64 size;
    u64 sizeBytes;
public:
    Bitblock(u64 size) {
        this->size = size;
        this->sizeBytes = (u64)ceill(size / 8.0);
        data = (u8*)tracked_malloc(sizeBytes);
        if (data == NULL) {
            throw std::bad_alloc();
        }
    }
    ~Bitblock() { tracked_free(data); }
    void set(u64 index, bool value) {
        u64 byteIndex = index / 8;
        u32 bitIndex = index % 8;
        u8 mask = 1 << bitIndex;
        if (value) {
            data[byteIndex] |= mask;
        } else {
            data[byteIndex] &= ~mask;
        }
    }
    bool get(u64 index) {
        if (size <= index) {
            throw std::out_of_range("index out of range");
        }
        u64 byteIndex = index / 8;
        u32 bitIndex = index % 8;
        u8 mask = 1 << bitIndex;
        return (data[byteIndex] & mask) != 0;
    }
    void setAll(bool value) { memset(data, value ? 0xFF : 0x00, sizeBytes); }
};

struct ScanlineMapElement {
    XY origin;
    XY size;    //y must be 1, x is the width
};
class ScanlineMap {
public:
    bool iPromiseNotToPutDuplicatePoints = false;   //skips checking if a point already exists in the map when adding
    std::map<int, std::vector<ScanlineMapElement>> scanlineMap;

    void mergeScanline(int p) {
        std::vector<ScanlineMapElement>& smm = scanlineMap[p];
        if (smm.size() <= 1) {
            return;
        }
        std::sort(smm.begin(), smm.end(), [](ScanlineMapElement a, ScanlineMapElement b) { return a.origin.x < b.origin.x; });
        for (int x = 0; x < smm.size() - 1; x++) {
            if ((smm[x].origin.x + smm[x].size.x) == (smm[x + 1].origin.x)) {
                smm[x].size.x += smm[x + 1].size.x;
                smm.erase(smm.begin() + x + 1);
                x--;
            }
        }
    }
    void addPoint(XY point) {
        if (iPromiseNotToPutDuplicatePoints || !pointExists(point)) {
            int p = point.y;
            scanlineMap[p].push_back({ point, {1,1} });
            mergeScanline(p);
        }
    }
    void addScanline(ScanlineMapElement e) {
        for (int x = 0; x < e.size.x; x++) {
            addPoint({ e.origin.x + x, e.origin.y });
        }
    }
    void addRect(SDL_Rect r) {
        for (int y = 0; y < r.h; y++) {
            addScanline({ {r.x,r.y + y}, {r.w,1} });
        }
    }
    void addOtherMap(ScanlineMap other) {
        other.forEachScanline([this](ScanlineMapElement e) { addScanline(e); });
    }
    bool pointExists(XY point) {
        std::vector<ScanlineMapElement>& r = scanlineMap[point.y];
        for (ScanlineMapElement& sme : r) {
            if (sme.origin.x <= point.x && (sme.origin.x + sme.size.x) > point.x) {
                return true;
            }
        }
        return false;
    }
    void forEachScanline(std::function<void(ScanlineMapElement)> f) {
        for (auto& me : scanlineMap) {
            for (auto& mm : me.second) {
                f(mm);
            }
        }
    }
    void forEachPoint(std::function<void(XY)> f) {
        for (auto& me : scanlineMap) {
            for (auto& mm : me.second) {
                for (int x = 0; x < mm.size.x; x++) {
                    f({ x + mm.origin.x, me.first });
                }
            }
        }
    }
    void clear() {
        scanlineMap.clear();
    }
    SDL_Rect getMinMaxRect() {
        XY min = {INT_MAX, INT_MAX};
        XY max = {INT_MIN, INT_MIN};
        for (auto& me : scanlineMap) {
            if (me.first < min.y) {
                min.y = me.first;
            }
            if (me.first > max.y) {
                max.y = me.first;
            }
            for (auto& mm : me.second) {
                if (mm.origin.x < min.x) {
                    min.x = mm.origin.x;
                }
                if ((mm.origin.x + mm.size.x) > max.x) {
                    max.x = mm.origin.x + mm.size.x;
                }
            }
        }
        return {min.x, min.y, max.x - min.x, max.y - min.y};
    }
};
//todo ^ make the above into an intlinemap too

struct IntLineMapElement {
    int x;
    int w;
};
class IntLineMap {
public:
    int pointCount = 0;
    bool iPromiseNotToPutDuplicatePoints = false;
    std::map<int, std::vector<IntLineMapElement>> intlineMap;

    void mergeIntLine(int y) {
        std::vector<IntLineMapElement>& smm = intlineMap[y];
        if (smm.size() <= 1) {
            return;
        }
        std::sort(smm.begin(), smm.end(), [](IntLineMapElement a, IntLineMapElement b) { return a.x < b.x; });
        for (int x = 0; x < smm.size() - 1; x++) {
            if ((smm[x].x + smm[x].w) == (smm[x + 1].x)) {
                smm[x].w += smm[x + 1].w;
                smm.erase(smm.begin() + x + 1);
                x--;
            }
        }
    }
    void addPoint(int x, int y) {
        if (iPromiseNotToPutDuplicatePoints || !pointExists(x,y)) {
            intlineMap[y].push_back({ x, 1 });
            pointCount++;
            mergeIntLine(y);
        }
    }
    bool pointExists(int x, int y) {
        std::vector<IntLineMapElement>& r = intlineMap[y];
        for (IntLineMapElement& sme : r) {
            if (sme.x <= x && (sme.x + sme.w) > x) {
                return true;
            }
        }
        return false;
    }
    
    void forEachScanline(std::function<void(IntLineMapElement, int)> f) {
        for (auto& me : intlineMap) {
            for (auto& mm : me.second) {
                f(mm, me.first);
            }
        }
    }

    void clear() {
        intlineMap.clear();
        pointCount = 0;
    }
};

class Detiler {
private:
    int indexNow = -1;
    int thisPower = 0;
    int squareDim = 2;
    Detiler* sub = NULL;
    XY subLast = { 0,0 };
public:
    Detiler(int width, int pow = 0) {
		squareDim = width;
        thisPower = pow;
	}
    ~Detiler() {
        if (sub != NULL) {
            delete sub;
        }
    }
    XY next() {
        indexNow++;
        if (indexNow == squareDim * squareDim) {
            if (sub == NULL) {
                sub = new Detiler(squareDim, thisPower + 1);
                sub->next();
            }
            subLast = sub->next();
        }
        indexNow %= squareDim * squareDim;
        int x = indexNow % squareDim;
        int y = indexNow / squareDim;
        return xyAdd(subLast, {x * ixpow(squareDim, thisPower), y * ixpow(squareDim, thisPower)});
    }
};
