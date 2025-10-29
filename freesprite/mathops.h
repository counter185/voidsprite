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

SDL_Surface* trimSurface(SDL_Surface* target, SDL_Rect dims);

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
SDL_Rect offsetRect(SDL_Rect r, int offset);
SDL_Rect offsetRect(SDL_Rect r, int offsetX, int offsetY);
double angleBetweenTwoPoints(XY a, XY b);
XY getSnappedPoint(XY from, XY to);

XYZd xyzdAdd(XYZd p1, XYZd p2);

std::vector<std::string> splitString(std::string a, char b);
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
PlatformNativePathString appendPath(PlatformNativePathString parent, PlatformNativePathString subdir);

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
SDL_Event scaleScreenPositionsInEvent(SDL_Event src);

SDL_Rect fitInside(SDL_Rect outer, SDL_Rect inner);

u32 parseIpAddress(std::string ipv4);
std::string ipToString(u32 ipv4);

#define matrix std::vector<std::vector<double>>
matrix matrixMultiply(matrix a, matrix b);

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
    // Skips checking if a point already exists in the map when adding
    // Improves speed but you must ensure you don't add duplicates
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
    int height = -1;

    Detiler(int width, int pow = 0) {
        squareDim = width;
        thisPower = pow;
    }
    ~Detiler() {
        if (sub != NULL) {
            delete sub;
        }
    }

    static Detiler* detilerStack(std::vector<int> tileSizes) {
        Detiler* ret = NULL;
        Detiler** pptr = &ret;
        for (int& v : tileSizes) {
            if (v > 1) {
                *pptr = new Detiler(v);
                pptr = &((*pptr)->sub);
            }
        }
        return ret;
    }

    XY next() {
        indexNow++;
        int indexAreaSize = (height <= 0 ? squareDim : height) * squareDim;
        if (indexNow == indexAreaSize) {
            if (sub == NULL) {
                sub = new Detiler(squareDim, thisPower + 1);
                sub->next();
            }
            subLast = sub->next();
        }
        indexNow %= indexAreaSize;
        int x = indexNow % squareDim;
        int y = indexNow / squareDim;
        return xyAdd(subLast, {x * ixpow(squareDim, thisPower), y * ixpow(squareDim, thisPower)});
    }
};

//double ended map
//assumes we're nice enough to not put duplicate values
template <typename T, typename S>
class demapAB {
private:
    std::map<T, S> ABmap;
    std::map<S, T> BAmap;
public:
    demapAB() {}
    demapAB(std::vector<std::pair<T, S>> contents) {
        for (auto& kv : contents) {
            pushSideA(kv.first, kv.second);
        }
    }

    S operator[](T v){
        return getFromA(v);
    }

    void pushSideA(T key, S value) {
        ABmap[key] = value;
        BAmap[value] = key;
    }
    void pushSideB(S key, S value) {
        ABmap[value] = key;
        BAmap[key] = value;
    }

    S getFromA(T key) {
        return ABmap[key];
    }
    T getFromB(S key) {
        return BAmap[key];
    }

    bool containsA(T v) {
        return ABmap.contains(v);
    }
    bool containsB(S v) {
        return BAmap.contains(v);
    }
};

class SDLVectorU8IOStream {
public:
    std::vector<u8> data;
    u64 pos = 0;

    struct StreamObjs {
        SDL_IOStreamInterface iface;
        SDLVectorU8IOStream* stream;
    };

    static Sint64 size(void* userdata) {
        return ((StreamObjs*)userdata)->stream->data.size();
    }
    static Sint64 seek(void* userdata, Sint64 offset, SDL_IOWhence whence) {
        StreamObjs* streamObjs = (StreamObjs*)userdata;
        SDLVectorU8IOStream* stream = streamObjs->stream;
        Sint64 newPos = 0;
        switch (whence) {
        case SDL_IO_SEEK_SET:
            newPos = offset;
            break;
        case SDL_IO_SEEK_CUR:
            newPos = stream->pos + offset;
            break;
        case SDL_IO_SEEK_END:
            newPos = stream->data.size() + offset;
            break;
        }
        if (newPos < 0) {
            newPos = 0;
        }
        if (newPos > (Sint64)stream->data.size()) {
            newPos = stream->data.size();
        }
        stream->pos = newPos;
        return stream->pos;
    }
    static size_t read(void* userdata, void* ptr, size_t size, SDL_IOStatus* status) {
        StreamObjs* streamObjs = (StreamObjs*)userdata;
        SDLVectorU8IOStream* stream = streamObjs->stream;
        if (stream->pos + size > stream->data.size()) {
            size = stream->data.size() - stream->pos;
        }
        if (size == 0) {
            *status = SDL_IO_STATUS_EOF;
            return 0;
        }
        memcpy(ptr, stream->data.data() + stream->pos, size);
        stream->pos += size;
        *status = SDL_IO_STATUS_READY;
        return size;
    }
    static size_t write(void* userdata, const void* ptr, size_t size, SDL_IOStatus* status) {
        StreamObjs* streamObjs = (StreamObjs*)userdata;
        SDLVectorU8IOStream* stream = streamObjs->stream;
        if (stream->pos + size > stream->data.size()) {
            stream->data.resize(stream->pos + size);
        }
        memcpy(stream->data.data() + stream->pos, ptr, size);
        *status = SDL_IO_STATUS_READY;
        stream->pos += size;
        return size;
    }
    static bool flush(void* userdata, SDL_IOStatus* status) {
        *status = SDL_IO_STATUS_READY;
        return true;
    }
    static bool close(void* userdata) {
        delete ((StreamObjs*)userdata)->stream;
        delete ((StreamObjs*)userdata);
        return true;
    }

    static SDL_IOStream* OpenNew() {
        StreamObjs* streamObjs = new StreamObjs();
        SDL_INIT_INTERFACE(&streamObjs->iface);
        streamObjs->stream = new SDLVectorU8IOStream();
        streamObjs->iface.size = SDLVectorU8IOStream::size;
        streamObjs->iface.seek = SDLVectorU8IOStream::seek;
        streamObjs->iface.read = SDLVectorU8IOStream::read;
        streamObjs->iface.write = SDLVectorU8IOStream::write;
        streamObjs->iface.flush = SDLVectorU8IOStream::flush;
        streamObjs->iface.close = SDLVectorU8IOStream::close;

        return SDL_OpenIO(&streamObjs->iface, streamObjs);
    }
};

class DoOnReturn {
public:
    std::function<void()> func;
    DoOnReturn(std::function<void()> f) : func(f) {}
    ~DoOnReturn() {
        if (func != NULL) {
            try {
                func();
            }
            catch (std::exception& e) {
                logerr(frmt("[DoOnReturn] failed:\n {}", e.what()));
            }
        }
    }
};