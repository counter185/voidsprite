#pragma once

SDL_Texture* getVisualConfigTexture(std::string key, SDL_Renderer* rd = NULL);

enum FillType {
    FILL_INVALID = 0,
    FILL_SOLID = 1,
    FILL_GRADIENT = 2,
    FILL_TEXTURE = 3,
    FILL_3POINTVGRADIENT = 4,
};

class Fill {
protected:
    Fill(FillType t) : type(t) {}
public:
    FillType type;
    u32 fillSolidColor = 0xFFFFFFFF;
    u32 fillGradientUL = 0xFFFFFFFF, fillGradientUR = 0xFFFFFFFF, fillGradientDL = 0xFFFFFFFF, fillGradientDR = 0xFFFFFFFF;
    u32 fillGradientML = 0xFFFFFFFF, fillGradientMR = 0xFFFFFFFF;
    std::string fillTextureVCKey = "";
    HotReloadableTexture* fillTexture = NULL;

    static Fill None()
    {
        Fill r(FILL_INVALID);
        return r;
    }

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
    static Fill ThreePointVerticalGradient(u32 ul, u32 ur, u32 ml, u32 mr, u32 dl, u32 dr)
    {
        Fill r(FILL_3POINTVGRADIENT);
        r.fillGradientUL = ul;
        r.fillGradientUR = ur;
        r.fillGradientML = ml;
        r.fillGradientMR = mr;
        r.fillGradientDL = dl;
        r.fillGradientDR = dr;
        return r;
    }
    static Fill Texture(HotReloadableTexture* tex)
    { 
        Fill r(FILL_TEXTURE);
        r.fillTexture = tex;
        return r;
    }
    static Fill Texture(std::string vcAssetKey)
    { 
        Fill r(FILL_TEXTURE);
        r.fillTextureVCKey = vcAssetKey;
        return r;
    }

    Fill(SDL_Color c);
    ~Fill() {
        if (fillTexture != NULL) {
            delete fillTexture;
        }
    }

    void fill(SDL_Rect r);

    std::string serialize();
    static Fill deserialize(std::string str);
};