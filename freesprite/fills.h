#pragma once

SDL_Texture* getVisualConfigTexture(std::string key);

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
    u32 fillGradientUL, fillGradientUR, fillGradientDL, fillGradientDR = 0xFFFFFFFF;
    u32 fillGradientML, fillGradientMR = 0xFFFFFFFF;
    std::string fillTextureVCKey = "";
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
    static Fill Texture(SDL_Texture* tex)
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

    Fill(SDL_Color c) {
        type = FILL_SOLID;
        fillSolidColor = sdlcolorToUint32(c);
    }

    void fill(SDL_Rect r) {
        switch (type) {
            case FILL_INVALID:
                break;
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
            case FILL_3POINTVGRADIENT:
                renderGradient({r.x,r.y,r.w,r.h/2}, fillGradientUL, fillGradientUR, fillGradientML, fillGradientMR);
                renderGradient({r.x,r.y+r.h/2,r.w,r.h/2}, fillGradientML, fillGradientMR, fillGradientDL, fillGradientDR);
                break;
            case FILL_TEXTURE:
                SDL_RenderCopy(g_rd, (fillTexture == NULL ? getVisualConfigTexture(fillTextureVCKey) : fillTexture), NULL, &r);
                break;
        }
    }

    std::string serialize() {
        switch (type) {
            case FILL_SOLID:
                return std::format("fill:solid:{:08X}", fillSolidColor);
            case FILL_GRADIENT:
                return std::format("fill:gradient:{:08X}/{:08X}/{:08X}/{:08X}", fillGradientUL, fillGradientUR, fillGradientDL, fillGradientDR);
            case FILL_3POINTVGRADIENT:
                return std::format("fill:3pointgradient:{:08X}/{:08X}/{:08X}/{:08X}/{:08X}/{:08X}", fillGradientUL, fillGradientUR, fillGradientML, fillGradientMR, fillGradientDL, fillGradientDR);
            case FILL_TEXTURE:
                return std::format("fill:texture:{}", fillTextureVCKey);
            default:
                return "fill:none";
        }
    }
    static Fill deserialize(std::string str) {
        try {
            std::vector<std::string> parts = splitString(str, ':');
            if (parts.size() < 2 || parts[0] != "fill") {
                return Fill(FILL_INVALID);
            }
            if (parts[1] == "solid") {
                return Fill::Solid(std::stoul(parts[2], nullptr, 16));
            }
            else if (parts[1] == "3pointgradient") {
                auto subColors = splitString(parts[2], '/');
                return Fill::ThreePointVerticalGradient(
                    std::stoul(subColors[0], nullptr, 16),
                    std::stoul(subColors[1], nullptr, 16),
                    std::stoul(subColors[2], nullptr, 16),
                    std::stoul(subColors[3], nullptr, 16),
                    std::stoul(subColors[4], nullptr, 16),
                    std::stoul(subColors[5], nullptr, 16)
                );
            }
            else if (parts[1] == "gradient") {
                auto subColors = splitString(parts[2], '/');
                return Fill::Gradient(
                    std::stoul(subColors[0], nullptr, 16),
                    std::stoul(subColors[1], nullptr, 16),
                    std::stoul(subColors[2], nullptr, 16),
                    std::stoul(subColors[3], nullptr, 16)
                );
            }
            else if (parts[1] == "texture") {
                std::string vcKey = parts[2];
                return Fill::Texture(vcKey);
            }
        }
        catch (std::exception& e) {
            logprintf("Error deserializing fill: %s\n", e.what());
        }

        return Fill(FILL_INVALID);
    }
};