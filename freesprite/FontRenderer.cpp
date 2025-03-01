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

TextRenderer::TextRenderer() {
    font = TTF_OpenFont(pathInProgramDirectory(FONT_PATH).c_str(), 18);
    font = font == NULL ? TTF_OpenFont(FONT_PATH, 18) : font;
    fontJP = TTF_OpenFont(pathInProgramDirectory(FONT_PATH_JP).c_str(), 18);
    fontJP = fontJP == NULL ? TTF_OpenFont(FONT_PATH_JP, 18) : fontJP;
}

TextRenderer::~TextRenderer() {
    if (font != NULL) {
        TTF_CloseFont(font);
    }
    if (fontJP != NULL) {
        TTF_CloseFont(fontJP);
    }
}

XY TextRenderer::RenderString(std::string text, int x, int y, SDL_Color col) {
    int drawX = x;
    int drawY = y;
    uint32_t currentUTF8Sym = 0;
    int nextUTFBytes = 0;
    for (int chx = 0; chx != text.size(); chx++) {
        char target = text.at(chx);
        if (target == '\n') {
            drawX = x;
            drawY += 18;
        }
        else {
            bool shouldDraw = ParseUTF8(target, &nextUTFBytes, currentUTF8Sym);
            if (shouldDraw) {
                /*if (!renderedGlyphs.contains(target)) {
                    RenderGlyph(target);
                }*/
                if (!renderedGlyphs.contains(currentUTF8Sym)) {
                    RenderGlyph(currentUTF8Sym);
                }
                GlyphData glyphData = renderedGlyphs[currentUTF8Sym];
                SDL_Rect drawRect = { drawX, drawY, glyphData.w, glyphData.h };
                SDL_SetTextureColorMod(glyphData.texture, col.r, col.g, col.b);
                SDL_SetTextureAlphaMod(glyphData.texture, col.a);
                SDL_RenderCopy(g_rd, glyphData.texture, NULL, &drawRect);
                drawX += glyphData.advance;
            }
        }
    }
    return { drawX, drawY };
}

XY TextRenderer::StatStringDimensions(std::string text)
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
            drawY += 18;
        }
        else {
            bool shouldDraw = ParseUTF8(target, &nextUTFBytes, currentUTF8Sym);
            if (shouldDraw) {
                if (!renderedGlyphs.contains(currentUTF8Sym)) {
                    RenderGlyph(currentUTF8Sym);
                }
                GlyphData glyphData = renderedGlyphs[currentUTF8Sym];
                drawX += glyphData.advance;
                if (drawX > maxDraw.x) {
					maxDraw.x = drawX;
				}
            }
        }
    }
    drawY += 18;
    maxDraw.y = drawY;
    return maxDraw;
}

void TextRenderer::RenderGlyph(uint32_t a) {
    TTF_Font* usedFont =
        (a >= 0x3000 && a <= 0x30ff) || (a >= 0xff00 && a <= 0xffef) || (a >= 0x4e00 && a <= 0x9faf) ? fontJP
        : font;
    SDL_Surface* gl = TTF_RenderGlyph32_Blended(usedFont, (Uint32)a, SDL_Color{ 255,255,255,255 });
    if (gl != NULL) {
        SDL_Texture* newTexture = tracked_createTextureFromSurface(g_rd, gl);

        GlyphData newGlyphData;
        TTF_GlyphMetrics32(usedFont, a, &newGlyphData.minx, &newGlyphData.maxx, &newGlyphData.miny, &newGlyphData.maxy, &newGlyphData.advance);
        newGlyphData.texture = newTexture;
        newGlyphData.w = gl->w;
        newGlyphData.h = gl->h;
        SDL_FreeSurface(gl);
        renderedGlyphs[a] = newGlyphData;

    }
    else {
        renderedGlyphs[a] = GlyphData{ 0,0,0,0,0,0,0,NULL };
    }
}