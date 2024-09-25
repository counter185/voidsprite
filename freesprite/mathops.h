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

//stolen code yay
//https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both


void renderGradient(SDL_Rect bounds, uint32_t colorUL, uint32_t colorUR, uint32_t colorDL, uint32_t colorDR);

std::string pathInProgramDirectory(std::string path);

hsv rgb2hsv(rgb in);
SDL_Color rgb2sdlcolor(rgb a);
bool tryRgbStringToColor(std::string str, unsigned int* ret);
rgb hsv2rgb(hsv in);
unsigned int alphaBlend(unsigned int colora, unsigned int colorb);
uint32_t sdlcolorToUint32(SDL_Color c);
uint32_t modAlpha(uint32_t color, uint8_t alpha);

bool pointInBox(XY point, SDL_Rect rect);

double xyDistance(XY p1, XY p2);
bool xyEqual(XY p1, XY p2);
XY xyAdd(XY p1, XY p2);
XY xySubtract(XY p1, XY p2);
SDL_FPoint xytofp(XY p);

std::string stringToLower(std::string a);
std::string shiftJIStoUTF8(std::string a);
std::wstring utf8StringToWstring(std::string a);
std::string wstringToUTF8String(std::wstring a);
std::string convertStringToUTF8OnWin32(PlatformNativePathString a);
PlatformNativePathString convertStringOnWin32(std::string a);

bool stringEndsWithIgnoreCase(std::wstring c, std::wstring endsWith);
bool stringEndsWithIgnoreCase(std::string c, std::string endsWith);

void rasterizeLine(XY from, XY to, std::function<void(XY)> forEachPixel, int arc = 0);
void rasterizeEllipse(XY posMin, XY posMax, std::function<void(XY)> forEachPixel);

XY statLineEndpoint(XY p1, XY p2, double percent);
void drawLine(XY p1, XY p2, double percent = 1.0);
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
uint32_t PackRGBAtoARGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a);