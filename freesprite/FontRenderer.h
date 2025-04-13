#pragma once
#include "globals.h"

struct GlyphData {
    int minx = 0, miny = 0, maxx = 0, maxy = 0, advance = 0;
    int w = 0, h = 0;
    SDL_Texture* texture = NULL;
};

class FontObject {
protected:
    std::map<int, std::map<uint32_t, GlyphData>> renderedGlyphs;
public:
	std::vector<std::pair<u32, u32>> charBounds;
    
    virtual ~FontObject() {
		for (auto& glyph : renderedGlyphs) {
			for (auto& glyphData : glyph.second) {
				if (glyphData.second.texture != NULL) {
					SDL_DestroyTexture(glyphData.second.texture);
				}
			}
		}
    }

    bool charInBounds(u32 ch) {
		for (auto& bounds : charBounds) {
			if (ch >= bounds.first && ch <= bounds.second) {
				return true;
			}
		}
        return false;
    }
    virtual bool hasGlyphForChar(u32 ch, int size) = 0;
    virtual GlyphData getGlyphForChar(u32 ch, int size) = 0;
};

class TTFFontObject : public FontObject {
private:
	TTF_Font* font = NULL;
    void RenderGlyph(uint32_t a, int size);
public:
	TTFFontObject(TTF_Font* f) : font(f) {
	}
    ~TTFFontObject() override {
        if (font != NULL) {
            TTF_CloseFont(font);
        }
		FontObject::~FontObject();
    }

    bool hasGlyphForChar(u32 ch, int size) override {
        if (!renderedGlyphs[size].contains(ch)) {
			RenderGlyph(ch, size);
        }
        if (renderedGlyphs[size][ch].texture != NULL) {
            return true;
        }
    }
    GlyphData getGlyphForChar(u32 ch, int size) override {
        if (!renderedGlyphs[size].contains(ch)) {
            RenderGlyph(ch, size);
        }
        return renderedGlyphs[size][ch];
    }
};

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    void AddFont(FontObject* f) {
        if (f != NULL) {
            fontStack.insert(fontStack.begin(), f);
        }
    }
    void AddFont(TTF_Font* f, std::vector<std::pair<u32, u32>> bounds = { {0, 0xFFFFFFFF} }) {
        if (f != NULL) {
            FontObject* font = new TTFFontObject(f);
            font->charBounds = bounds;
            AddFont(font);
        }
    }
    XY RenderString(std::string text, int x, int y, SDL_Color col = { 255,255,255,255 }, int size = 18);
    XY StatStringDimensions(std::string text, int size = 18);
private:
    std::vector<FontObject*> fontStack;

	GlyphData getGlyphForChar(uint32_t ch, int size);
};

bool ParseUTF8(unsigned char ch, int* nextUTFBytes, uint32_t& out);