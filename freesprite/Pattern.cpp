#include "Pattern.h"
#include "globals.h"
#include "Notification.h"
#include "LayerPalettized.h"
#include "FileIO.h"

void g_loadPatterns()
{
    g_patterns.push_back(new PatternFull());
    g_patterns.push_back(new PatternGrid());
    g_patterns.push_back(new PatternGridReverse());
    g_patterns.push_back(new PatternDiag2px());
    g_patterns.push_back(new PatternDiag3px());
    g_patterns.push_back(new PatternDiag4px());
    g_patterns.push_back(new PatternDiag2pxReverse());
    g_patterns.push_back(new PatternDiag3pxReverse());
    g_patterns.push_back(new PatternDiag4pxReverse());
    g_patterns.push_back(new PatternHorizontal(1));
    g_patterns.push_back(new PatternHorizontal(2));
    g_patterns.push_back(new PatternHorizontal(3));
    g_patterns.push_back(new PatternHorizontal(4));
    g_patterns.push_back(new PatternVertical(1));
    g_patterns.push_back(new PatternVertical(2));
    g_patterns.push_back(new PatternVertical(3));
    g_patterns.push_back(new PatternVertical(4));
    g_patterns.push_back(new PatternSquares(1));
    g_patterns.push_back(new PatternSquares(2));
    g_patterns.push_back(new PatternSquares(3));
    g_patterns.push_back(new PatternSquares(4));
    g_patterns.push_back(new PatternHT1());
    g_patterns.push_back(new PatternHT2());
    g_patterns.push_back(new PatternRandom(2));
    g_patterns.push_back(new PatternRandom(4));
    g_patterns.push_back(new PatternRandom(8));
    g_patterns.push_back(new PatternRandom(16));
}

int g_loadCustomPatterns()
{
    int customPatterns = 0;
    auto customPatternPaths = joinVectors({
        platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("patterns/"), ".pbm"),
        platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("patterns/"), ".xbm")
        });
    for (auto& cpattern : customPatternPaths) {
        CustomPattern* p = CustomPattern::load(cpattern);
        if (p != NULL) {
            g_patterns.push_back(p);
            customPatterns++;
        }
    }
    return customPatterns;
}

void g_loadPatternIcons()
{
    for (Pattern*& pattern : g_patterns) {
        pattern->tryLoadIcon();
    }
}

SDL_Texture* g_generatePatternIcon(SDL_Renderer* rd, Pattern* p, int pxScale, XY patternOffset, XY texSize)
{
    SDL_Surface* srf = SDL_CreateSurface(texSize.x, texSize.y, SDL_PIXELFORMAT_ARGB4444);
    if (srf != NULL) {
        DoOnReturn freeSrf([srf]() {SDL_FreeSurface(srf); });

        u16* px = (u16*)srf->pixels;
        memset(px, 0, texSize.x * texSize.y * 2);
        rasterizeLine({ 1,1 }, { texSize.x - 2, 1 }, [px, srf](XY p) { ARRAY2DPOINT(px, p.x, p.y, srf->pitch / 2) = 0xFFFF; });
        rasterizeLine({ texSize.x - 2, texSize.y - 2 }, { texSize.x - 2, 2 }, [px, srf](XY p) { ARRAY2DPOINT(px, p.x, p.y, srf->pitch / 2) = 0xFFFF; });
        XY origin = { 1, 3 };
        int areaW = texSize.x - 4;
        int areaH = texSize.y - 4;
        for (int y = 0; y < areaH; y++) {
            for (int x = 0; x < areaW; x++) {
                XY point = xyAdd(origin, { x,y });
                ARRAY2DPOINT(px, point.x, point.y, srf->pitch / 2) = p->canDrawAt({ (x / pxScale) + patternOffset.x, (y / pxScale) + patternOffset.y }) ? 0xFFFF : 0;
            }
        }
        return tracked_createTextureFromSurface(rd, srf);
    }
    return NULL;
}

ReldTex* Pattern::makeIconGenerator(XY offset)
{
    return new ReldTex([this, offset](SDL_Renderer* rd) { return g_generatePatternIcon(rd, this, 2, offset); });
}

void Pattern::tryLoadIcon()
{
    cachedIcon = 
        getIconPath() == "" ? makeIconGenerator()
        : new ReldTex([this](SDL_Renderer* rd) { return IMGLoadAssetToTexture(getIconPath()); });
}

CustomPattern* CustomPattern::load(PlatformNativePathString path)
{
    LayerPalettized* loadImage = NULL;
    if (stringEndsWithIgnoreCase(path, convertStringOnWin32(".pbm"))) {
        loadImage = (LayerPalettized*)readAnymapPBM(path);
    }
    else if (stringEndsWithIgnoreCase(path, convertStringOnWin32(".xbm"))) {
        loadImage = (LayerPalettized*)readXBM(path);
    }
    
    if (loadImage != NULL) {
        if (loadImage->w != 0 && loadImage->h != 0) {
            CustomPattern* ret = new CustomPattern(loadImage);
            PlatformNativePathString fileNameWithNoPathAndExtension = path.substr(path.find_last_of(convertStringOnWin32("/\\")) + 1);
            ret->name = convertStringToUTF8OnWin32(fileNameWithNoPathAndExtension.substr(0, fileNameWithNoPathAndExtension.find_last_of(convertStringOnWin32("."))));
            delete loadImage;
            return ret;
        }
    }
    g_addNotification(ErrorNotification("Error", "Can't load: " + convertStringToUTF8OnWin32(path)));
    std::string err = "Can't load: " + convertStringToUTF8OnWin32(path);
    logprintf("%s\n", err.c_str());
    return NULL;
}

CustomPattern::CustomPattern(LayerPalettized* from)
{
    if (from != NULL) {
        bitmap = (uint8_t*)tracked_malloc(from->w * from->h, "Patterns");
        for (uint64_t p = 0; p < from->w * from->h; p++) {
            bitmap[p] = (from->pixels32())[p] == 0 ? 0 : 1;
        }
        bitmapDimensions = XY{ from->w, from->h };
    }
}

bool CustomPattern::canDrawAt(XY position)
{
    if (bitmap != NULL) {
        if (position.x >= 0 && position.y >= 0) {
            return bitmap[(position.x % bitmapDimensions.x) + (position.y % bitmapDimensions.y) * bitmapDimensions.x] == 1;
        }
    }
    return true;
}
