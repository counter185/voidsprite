#pragma once
#include "globals.h"

struct GlyphData {
    int minx = 0, miny = 0, maxx = 0, maxy = 0, advance = 0;
    int w = 0, h = 0;
    SDL_Texture* texture = NULL;
};

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    XY RenderString(std::string text, int x, int y, SDL_Color col = { 255,255,255,255 }, int size = 18);
    XY StatStringDimensions(std::string text, int size = 18);
private:
    TTF_Font* font = NULL;
    TTF_Font* fontJP = NULL;
    std::map<int, std::map<uint32_t, GlyphData>> renderedGlyphs;

    void RenderGlyph(uint32_t a, int size);
};

bool ParseUTF8(unsigned char ch, int* nextUTFBytes, uint32_t& out);