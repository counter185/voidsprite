#include <SDL3_ttf/SDL_ttf.h>

#include "FontRenderer.h"
#include "mathops.h"

bool ParseUTF8(unsigned char ch, int* nextUTFBytes, uint32_t& out) {
    if ((ch >> 5) == 0b110) {
        *nextUTFBytes = 1;
        out = ((uint32_t)ch & 0b11111) << 6;
        return false;
    }
    else if ((ch >> 4) == 0b1110) {
        *nextUTFBytes = 2;
        out = ((uint32_t)ch & 0b1111) << 12;
        return false;
    }
    else if ((ch >> 3) == 0b11110) {
        *nextUTFBytes = 3;
        out = ((uint32_t)ch & 0b111) << 18;
        return false;
    }
    else if ((ch >> 2) == 0b111110) {
        *nextUTFBytes = 4;
        out = ((uint32_t)ch & 0b11) << 24;
        return false;
    }
    else if ((ch >> 1) == 0b1111110) {
        *nextUTFBytes = 5;
        out = ((uint32_t)ch & 0b1) << 30;
        return false;
    }
    else if ((ch >> 6) == 0b10) {
        if ((*nextUTFBytes)-- > 0) {
            out |= ((uint32_t)ch & 0b111111) << (*nextUTFBytes * 6);
            return *nextUTFBytes == 0;
        }
        else {
            //invalid utf8;
            out = 0;
            return true;
        }
    }
    else {
        *nextUTFBytes = 0;
        out = (uint32_t)ch;
        return true;
    }

}

TTFFontObject::TTFFontObject(TTF_Font* f) : font(f) {
    isBitmapFont = !TTF_FontIsScalable(f);
    loginfo(std::format("[TTFFontObject] font: {}, engine: {}", TTF_GetFontFamilyName(f), isBitmapFont ? "bitmap" : "ttf"));
}

TTFFontObject::~TTFFontObject() {
        if (font != NULL) {
            TTF_CloseFont(font);
        }
    }

TextRenderer::TextRenderer() {
    /*font = TTF_OpenFont(pathInProgramDirectory(FONT_PATH).c_str(), 18);
    font = font == NULL ? TTF_OpenFont(FONT_PATH, 18) : font;
    fontJP = TTF_OpenFont(pathInProgramDirectory(FONT_PATH_JP).c_str(), 18);
    fontJP = fontJP == NULL ? TTF_OpenFont(FONT_PATH_JP, 18) : fontJP;*/
}

TextRenderer::~TextRenderer() {

    for (auto& s : fontStack) {
        delete s;
    }
}

XY TextRenderer::RenderString(std::string text, int x, int y, SDL_Color col, int size) {
    SDL_Rect clipNow = g_currentClip();
    int drawX = x;
    int drawY = y;
    uint32_t currentUTF8Sym = 0;
    int nextUTFBytes = 0;
    bool lastCharWasTooHigh = false;
    for (int chx = 0; chx < text.size(); chx++) {
        char target = text.at(chx);

        if (lastCharWasTooHigh) {
            if (target != '\n') {
                continue;
            }
            else {
                lastCharWasTooHigh = false;
            }
        }
        
        if (target == '\n') {
            drawX = x;
            drawY += size;
            if (drawY > clipNow.y + clipNow.h) {
                break;
            }
        }
        else {
            bool shouldDraw = ParseUTF8(target, &nextUTFBytes, currentUTF8Sym);
            if (shouldDraw) {
                /*if (!renderedGlyphs.contains(target)) {
                    RenderGlyph(target);
                }*/
                GlyphData glyphData = getGlyphForChar(currentUTF8Sym, size);// renderedGlyphs[size][currentUTF8Sym];
                SDL_Rect drawRect = { drawX, drawY, glyphData.w, glyphData.h };
                if (drawRect.y + drawRect.h >= clipNow.y) {
                    SDL_SetTextureColorMod(glyphData.texture, col.r, col.g, col.b);
                    SDL_SetTextureAlphaMod(glyphData.texture, col.a);
                    SDL_RenderCopy(g_rd, glyphData.texture, NULL, &drawRect);
                }
                else {
                    lastCharWasTooHigh = true;
                }
                drawX += glyphData.advance;
            }
        }
    }
    return { drawX, drawY };
}

XY TextRenderer::StatStringEndpoint(std::string text, int x, int y, int size)
{
    int drawX = 0;
    int drawY = 0;

    uint32_t currentUTF8Sym = 0;
    int nextUTFBytes = 0;
    for (int chx = 0; chx != text.size(); chx++) {
        char target = text.at(chx);
        if (target == '\n') {
            drawX = 0;
            drawY += size;
        }
        else {
            bool shouldDraw = ParseUTF8(target, &nextUTFBytes, currentUTF8Sym);
            if (shouldDraw) {
                GlyphData glyphData = getGlyphForChar(currentUTF8Sym, size); //renderedGlyphs[size][currentUTF8Sym];
                drawX += glyphData.advance;
            }
        }
    }
    return xyAdd({x,y}, { drawX, drawY });
}

XY TextRenderer::StatStringDimensions(std::string text, int size)
{
    int drawX = 0;
    int drawY = 0;

    XY maxDraw = { 0,0 };

    uint32_t currentUTF8Sym = 0;
    int nextUTFBytes = 0;
    for (int chx = 0; chx != text.size(); chx++) {
        char target = text.at(chx);
        if (target == '\n') {
            drawX = 0;
            drawY += size;
        }
        else {
            bool shouldDraw = ParseUTF8(target, &nextUTFBytes, currentUTF8Sym);
            if (shouldDraw) {
                GlyphData glyphData = getGlyphForChar(currentUTF8Sym, size); //renderedGlyphs[size][currentUTF8Sym];
                drawX += glyphData.advance;
                if (drawX > maxDraw.x) {
                    maxDraw.x = drawX;
                }
            }
        }
    }
    drawY += size;
    maxDraw.y = drawY;
    return maxDraw;
}

void TextRenderer::precacheFontCommonChars(int size)
{
    //barely any effect on desktop
    //but reduces stuttering on android
    std::string str =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "1234567890"
        "!@#$%^&*()[]:\\/|+_<>?`~";
    StatStringDimensions(str, size);
}

GlyphData TextRenderer::getGlyphForChar(uint32_t ch, int size)
{
    for (auto& font : fontStack) {
        if (font->charInBounds(ch) && font->hasGlyphForChar(ch, size)) {
            return font->getGlyphForChar(ch, size);
        }
    }
    return GlyphData{ 0,0,0,0,0,0,0,NULL };
}

/*void TextRenderer::RenderGlyph(uint32_t a, int size) {
    
    TTF_Font* usedFont =
        (a >= 0x3000 && a <= 0x30ff) || (a >= 0xff00 && a <= 0xffef) || (a >= 0x4e00 && a <= 0x9faf) ? fontJP
        : font;
    TTF_SetFontSize(usedFont, size);
    SDL_Surface* gl = TTF_RenderGlyph_Blended(usedFont, (Uint32)a, SDL_Color{ 255,255,255,255 });
    if (gl != NULL) {
        SDL_Texture* newTexture = tracked_createTextureFromSurface(g_rd, gl);

        GlyphData newGlyphData;
        TTF_GetGlyphMetrics(usedFont, a, &newGlyphData.minx, &newGlyphData.maxx, &newGlyphData.miny, &newGlyphData.maxy, &newGlyphData.advance);
        newGlyphData.texture = newTexture;
        newGlyphData.w = gl->w;
        newGlyphData.h = gl->h;
        SDL_FreeSurface(gl);
        renderedGlyphs[size][a] = newGlyphData;

    }
    else {
        renderedGlyphs[size][a] = GlyphData{ 0,0,0,0,0,0,0,NULL };
    }
}*/

void TTFFontObject::RenderGlyph(uint32_t a, int size)
{
    if (!charInBounds(a)) {
        logprintf("** requested glyph outside of bounds\n");
    }
    if (TTF_FontHasGlyph(font, a)) {
        TTF_SetFontSize(font, size);
        SDL_Surface* gl = TTF_RenderGlyph_Blended(font, (Uint32)a, SDL_Color{ 255,255,255,255 });
        if (gl != NULL) {
            SDL_Texture* newTexture = tracked_createTextureFromSurface(g_rd, gl);

            GlyphData newGlyphData;
            TTF_GetGlyphMetrics(font, a, &newGlyphData.minx, &newGlyphData.maxx, &newGlyphData.miny, &newGlyphData.maxy, &newGlyphData.advance);
            newGlyphData.texture = newTexture;
            newGlyphData.w = gl->w;
            newGlyphData.h = gl->h;
            SDL_FreeSurface(gl);
            renderedGlyphs[size][a] = newGlyphData;

        }
        else {
            renderedGlyphs[size][a] = GlyphData{ 0,0,0,0,0,0,0,NULL };
        }
    }
    else {
        renderedGlyphs[size][a] = GlyphData{ 0,0,0,0,0,0,0,NULL };
    }
}
