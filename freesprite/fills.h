#pragma once

enum FillType {
    FILL_INVALID = 0,
    FILL_SOLID = 1,
    FILL_GRADIENT = 2,
    FILL_TEXTURE = 3
};

class Fill {
protected:
    Fill(FillType t) : type(t) {}
public:
    FillType type;
    u32 fillSolidColor = 0xFFFFFFFF;
    u32 fillGradientUL, fillGradientUR, fillGradientDL, fillGradientDR = 0xFFFFFFFF;
    SDL_Texture* fillTexture = NULL;

    static Fill Solid(u32 color)
    {
        Fill r(FILL_SOLID);
        r.fillSolidColor = color;
        return r;
    }
    static Fill Gradient(u32 ul, u32 ur, u32 dl, u32 dr)
    {
        Fill r(FILL_GRADIENT);
        r.fillGradientUL = ul;
        r.fillGradientUR = ur;
        r.fillGradientDL = dl;
        r.fillGradientDR = dr;
        return r;
    }
    static Fill Texture(SDL_Texture* tex)
    { 
        Fill r(FILL_TEXTURE);
        r.fillTexture = tex;
        return r;
    }

    Fill(SDL_Color c) {
        type = FILL_SOLID;
        fillSolidColor = sdlcolorToUint32(c);
    }

    void fill(SDL_Rect r) {
        switch (type) {
            case FILL_SOLID:
                {
                    SDL_Color c = uint32ToSDLColor(fillSolidColor);
                    SDL_SetRenderDrawColor(g_rd, c.r,c.g,c.b,c.a);
                    SDL_RenderFillRect(g_rd, &r);
                }
                break;
            case FILL_GRADIENT:
                renderGradient(r, fillGradientUL, fillGradientUR, fillGradientDL, fillGradientDR);
                break;
            case FILL_TEXTURE:
                SDL_RenderCopy(g_rd, fillTexture, NULL, &r);
                break;
        }
    }
};

#define FILL_BUTTON_CHECKED_DEFAULT Fill::Solid(0xD0FFFFFF)