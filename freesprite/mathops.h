#pragma once

//#include <math.h>

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

SDL_Color rgb2sdlcolor(rgb a);
bool tryRgbStringToColor(std::string str, unsigned int* ret);
unsigned int alphaBlend(unsigned int colora, unsigned int colorb);
uint32_t sdlcolorToUint32(SDL_Color c);
uint32_t modAlpha(uint32_t color, uint8_t alpha);

u32 hsvShift(u32 color, hsv shift);
u32 hslShift(u32 color, hsl shift);
u32 hslShiftPixelStudioCompat(u32 color, hsl shift);

bool pointInBox(XY point, SDL_Rect rect);

double xyDistance(XY p1, XY p2);
bool xyEqual(XY p1, XY p2);
XY xyAdd(XY p1, XY p2);
XY xySubtract(XY p1, XY p2);
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

void rasterizeLine(XY from, XY to, std::function<void(XY)> forEachPixel, int arc = 0);
void rasterizeEllipse(XY posMin, XY posMax, std::function<void(XY)> forEachPixel);
void rasterizeBezierCurve(std::vector<XY> points, std::function<void(XY)> forEachPixel);

XY statLineEndpoint(XY p1, XY p2, double percent);
void drawLine(XY p1, XY p2, double percent = 1.0);
XY evalBezierPoint(std::vector<XY> vtx, double percent);
double XM1PW3P1(double x);

int ixmin(int a, int b);
int ixmax(int a, int b);
int iclamp(int vmin, int b, int vmax);
float fxmin(float a, float b);
float fxmax(float a, float b);
float fclamp(float vmin, float b, float vmax);
double dxmin(double a, double b);
double dxmax(double a, double b);

uint32_t BEtoLE32(uint32_t a);
uint16_t BEtoLE16(uint16_t a);

uint32_t RGB5A3toARGB8888(uint16_t rgb5a3Byte);
uint32_t RGB565toARGB8888(uint16_t rgb5a3Byte);
uint32_t RGB555toARGB8888(uint16_t rgb555);
uint32_t BGR555toARGB8888(uint16_t bgr555);
uint32_t PackRGBAtoARGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

std::vector<std::string> split(std::string a, char b);

std::string randomUUID();

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

class Detiler {
private:
    int tileSquareDimension;
    int index = -1;
    bool vertical = true;
public:
    Detiler(int tileSquareDimension) {
        this->tileSquareDimension = tileSquareDimension;
    }
    XY next() {
        index++;
        if (vertical) {
            return XY{ index / tileSquareDimension, index % tileSquareDimension };
        } else {
            return XY{ index % tileSquareDimension, index / tileSquareDimension};
        }

        if (index == tileSquareDimension * tileSquareDimension) {
            index = -1;
        }
    }
};