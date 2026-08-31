#pragma once
#include "globals.h"

void g_loadPatterns();
int g_loadCustomPatterns();
void g_loadPatternIcons();
SDL_Texture* g_generatePatternIcon(SDL_Renderer* rd, Pattern* p, int pxScale, XY patternOffset = { 0,0 }, XY texSize = { 22,22 });

class Pattern
{
protected:
    ReldTex* makeIconGenerator(XY offset = {0,0});
public:
    HotReloadableTexture* cachedIcon = NULL;

    virtual std::string getIconPath() { return ""; }
    virtual std::string getName() { return "Pattern"; }
    virtual bool canDrawAt(XY position) { return true; }
    virtual void tryLoadIcon();
};

class PatternFull : public Pattern
{
public:
    std::string getName() override { return ""; }
    bool canDrawAt(XY position) override { return true; }
};

class PatternGrid : public Pattern
{
public:
    std::string getName() override { return "1x1 grid"; }
    bool canDrawAt(XY position) override { return (position.x + position.y) % 2 == 0; }
};
class PatternGridReverse : public Pattern
{
public:
    std::string getName() override { return "1x1 grid (reversed)"; }
    bool canDrawAt(XY position) override { return (position.x + position.y) % 2 == 1; }
};
class PatternDiag2px : public Pattern
{
public:
    std::string getName() override { return "Diagonal - 2px dist."; }
    bool canDrawAt(XY position) override {
        if (position.x >= 0 && position.y >= 0) {
            return (position.x % 3) == (position.y % 3);
        }
        return false;
    }
};
class PatternDiag3px : public Pattern
{
public:
    std::string getName() override { return "Diagonal - 3px dist."; }
    bool canDrawAt(XY position) override {
        if (position.x >= 0 && position.y >= 0) {
            return (position.x % 4) == (position.y % 4);
        }
        return false;
    }
};
class PatternDiag4px : public Pattern
{
public:
    std::string getName() override { return "Diagonal - 4px dist."; }
    bool canDrawAt(XY position) override {
        if (position.x >= 0 && position.y >= 0) {
            return (position.x % 5) == (position.y % 5);
        }
        return false;
    }
};

class PatternDiag2pxReverse : public Pattern
{
public:
    std::string getName() override { return "Diagonal (reversed) - 2px dist."; }
    bool canDrawAt(XY position) override {
        if (position.x >= 0 && position.y >= 0) {
            return (2-(position.x % 3)) == (position.y % 3);
        }
        return false;
    }
};
class PatternDiag3pxReverse : public Pattern
{
public:
    std::string getName() override { return "Diagonal (reversed) - 3px dist."; }
    bool canDrawAt(XY position) override {
        if (position.x >= 0 && position.y >= 0) {
            return (3-(position.x % 4)) == (position.y % 4);
        }
        return false;
    }
};
class PatternDiag4pxReverse : public Pattern
{
public:
    std::string getName() override { return "Diagonal (reversed) - 4px dist."; }
    bool canDrawAt(XY position) override {
        if (position.x >= 0 && position.y >= 0) {
            return (4-(position.x % 5)) == (position.y % 5);
        }
        return false;
    }
};

class PatternHorizontal : public Pattern
{
    int d;
public:
    PatternHorizontal(int distance) : d(distance) {}
    std::string getName() override { return frmt("Horizontal - {}px dist.", d); }
    bool canDrawAt(XY position) override { return position.y >= 0 ? (position.y % (d+1)) == 0 : false; }
};

class PatternVertical : public Pattern 
{
    int d;
public:
    PatternVertical(int distance) : d(distance) {}
    std::string getName() override { return frmt("Vertical - {}px dist.", d); }
    bool canDrawAt(XY position) override { return position.x >= 0 ? (position.x % (d+1)) == 0 : false; }
};

class PatternSquares : public Pattern 
{
    int d;
public:
    PatternSquares(int distance) : d(distance) {}
    std::string getName() override { return frmt("Squares - {}px", d); }
    bool canDrawAt(XY position) override { return (position.x % (d+1)) != 0 && (position.y % (d+1)) != 0; }
    void tryLoadIcon() override { cachedIcon = makeIconGenerator({ 1,1 }); }
};

class PatternHT1 : public Pattern
{
    std::string getName() override { return "Diagonal half-dithering"; }
    bool canDrawAt(XY position) override { 
        int lpx = position.x % 8;
        int lpy = position.y % 8;
        return (lpx + lpy) % 2 == 0 && (lpy) < (-lpx + 7);
    }
    void tryLoadIcon() override { cachedIcon = makeIconGenerator({ -1,-1 }); }
};

class PatternHT2 : public Pattern
{
    std::string getName() override { return "Central dither"; }
    bool canDrawAt(XY position) override { 
        int lpx = position.x % 4;
        int lpy = position.y % 4;
        return lpx % 2 == 0 && lpy % 2 == 0 && (lpy == 2 || lpx == 2);
    }
};

class PatternRandom : public Pattern
{
    int d;
public:
    PatternRandom(int randomDiv) { d = randomDiv; }

    std::string getIconPath() override { return frmt("pattern_rand_{}.png", d); }
    std::string getName() override { return frmt("Random ({}%)", (int)((1.0/d) * 100)); }
    bool canDrawAt(XY position) override { return (rand() % d) == 0; }
};

class CustomPattern : public Pattern
{
public:
    static CustomPattern* load(PlatformNativePathString path);

    bool valid = false;
    uint8_t* bitmap = NULL;
    XY bitmapDimensions = { 0,0 };
    std::string name = "Custom pattern";

    CustomPattern(LayerPalettized* from);
    std::string getName() override { return name; }
    bool canDrawAt(XY position) override;

};