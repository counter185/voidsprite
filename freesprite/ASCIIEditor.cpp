#include <fstream>
#include <SDL3_image/SDL_image.h>

#include "ASCIIEditor.h"
#include "FontRenderer.h"
#include "Notification.h"
#include "FileIO.h"

ASCIIEditor::ASCIIEditor(XY dimensions)
{
    lastRenderer = g_rd;
    session = new ASCIISession(dimensions);
    reloadFont();
    updateCanvasSize();
    c.recenter();
}

ASCIIEditor::ASCIIEditor(ASCIISession* ssn)
{
    lastRenderer = g_rd;
    session = ssn;
    reloadFont();
    updateCanvasSize();
    c.recenter();
}

ASCIIEditor::~ASCIIEditor()
{
    delete session;
    if (font != NULL) {
        delete font;
    }
}

void ASCIIEditor::render()
{
    renderGradient({ 0,0,g_windowW,g_windowH }, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF202020);

    if (lastRenderer != g_rd) {
        reloadFont();
        lastRenderer = g_rd;
    }

    c.lockToScreenBounds();

    SDL_Rect renderArea = c.getCanvasOnScreenRect();
    XY glyphSize = font->getGlyphSize();
    glyphSize = { glyphSize.x * fontScale.x, glyphSize.y * fontScale.y };

    int drawcalls = 0;

    for (int y = 0; y < session->getSize().y; y++) {
        SDL_Rect yCheck = c.getTileScreenRectAt({ 0,y }, glyphSize);
        if (yCheck.y >= g_windowH) {
            break;
        }
        if (yCheck.y + yCheck.h >= 0) {
            for (int x = 0; x < session->getSize().x; x++) {
                auto ch = session->get({ x,y });
                if (!(ch.ch == 0 && !font->skip00) && !(ch.ch == ' ' && !font->skipSpace)) {
                    SDL_Rect onScreenRect = c.getTileScreenRectAt({ x,y }, glyphSize);
                    if (onScreenRect.x >= g_windowW) {
                        break;
                    }
                    renderCharOnScreen(ch, onScreenRect);
                    drawcalls++;
                }
            }
        }
    }

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x30);
    c.drawTileGrid(glyphSize);

    renderCurrentTool();

#if _DEBUG
    g_fnt->RenderString(frmt("dc: {}", drawcalls), 10, 10);
#endif

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
    SDL_Rect offs = offsetRect(renderArea, 1);
    SDL_RenderDrawRect(g_rd, &offs);

    BaseScreen::render();
}

void ASCIIEditor::takeInput(SDL_Event evt)
{
    if (evt.type == SDL_QUIT) {
        closeThisScreen();
    }

    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);
    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {
        if (evt.type == SDL_EVENT_DROP_FILE) {
            std::string path = evt.drop.data;
            PlatformNativePathString npath = convertStringOnWin32(path);
            if (tryLoadCustomFontBitmap(npath)) {
                fontBitmapPath = npath;
                updateCanvasSize();
            }
        }
        else {
            if (!inputCurrentTool(evt)) {
                c.takeInput(evt);
            }
        }
    }
}

void ASCIIEditor::renderCurrentTool()
{
    XY glyphSize = font->getGlyphSize();
    switch (inputMode) {
        case INPUTMODE_TEXTINPUT:
            {
                SDL_Rect cursorRect = c.getTileScreenRectAt(cursorPos, { glyphSize.x * fontScale.x, glyphSize.y * fontScale.y });
                u64 timer = SDL_GetTicks64() % 2000;
                double alpha = 1.0 - (timer / 2000.0);
                SDL_SetRenderDrawColor(g_rd, 255, 255, 255, (u8)(0xa0 * alpha));
                SDL_RenderFillRect(g_rd, &cursorRect);
            }
            break;
    }
}

bool ASCIIEditor::inputCurrentTool(SDL_Event evt)
{
    XY glyphSize = font->getGlyphSize();
    switch (inputMode) {
        case INPUTMODE_TEXTINPUT:
            if (evt.type == SDL_EVENT_KEY_DOWN) {
                if (evt.key.scancode == SDL_SCANCODE_BACKSPACE) {
                    if (cursorPos.x > 0) {
                        cursorPos.x--;
                        session->set(cursorPos, { 0, currentForeground, currentBackground });
                    }
                    return true;
                }
                else if (evt.key.scancode == SDL_SCANCODE_LEFT) {
                    if (cursorPos.x > 0) {
                        cursorPos.x--;
                    }
                    return true;
                }
                else if (evt.key.scancode == SDL_SCANCODE_RIGHT) {
                    cursorPos.x++;
                    return true;
                }
                else if (evt.key.scancode == SDL_SCANCODE_UP) {
                    if (cursorPos.y > 0) {
                        cursorPos.y--;
                    }
                    return true;
                }
                else if (evt.key.scancode == SDL_SCANCODE_DOWN) {
                    cursorPos.y++;
                    return true;
                }
            }
            else if (evt.type == SDL_EVENT_TEXT_INPUT) {
                std::string text = evt.text.text;
                std::wstring wtext = utf8StringToWstring(text);
                for (wchar_t wc : wtext) {
                    if (codepageToUnicodeToBmpIndexMap[437].containsA((u32)wc)) {
                        u8 ch = (u8)codepageToUnicodeToBmpIndexMap[437][(u32)wc];
                        session->setWithResize(cursorPos, { ch, currentForeground, currentBackground });
                        updateCanvasSize();
                        cursorPos.x++;
                    }
                }
                return true;
            }
            else if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (evt.button.button == SDL_BUTTON_LEFT) {
                    XY mousePos = { evt.button.x, evt.button.y };
                    XY tilePos = c.getTilePosAt(mousePos, { glyphSize.x * fontScale.x, glyphSize.y * fontScale.y });
                    if (tilePos.x > 0 && tilePos.y > 0) {
                        cursorPos = tilePos;
                        SDL_StartTextInput(g_wd);
                    }
                    return true;
                }
            }
            break;
    }
    return false;
}

void ASCIIEditor::updateCanvasSize()
{
    XY glyphSize = font->getGlyphSize();
    XY charCounts = session->getSize();
    c.dimensions = { charCounts.x * glyphSize.x * fontScale.x, charCounts.y * glyphSize.y * fontScale.y };
}

void ASCIIEditor::reloadFont() {
    if (fontBitmapPath.empty() || !tryLoadCustomFontBitmap(fontBitmapPath)) {
        loadDefaultFont();
    }
}

bool ASCIIEditor::tryLoadCustomFontBitmap(PlatformNativePathString path)
{
    std::string u8path = convertStringToUTF8OnWin32(path);
    SDL_Surface* srf = IMG_Load(u8path.c_str());
    if (srf != NULL) {
        BitmapFontObject* newFont = new BitmapFontObject(srf, 437);
        if (font != NULL) {
            delete font;
        }
        glyphCache.clear();
        font = newFont;
        return true;
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Error loading custom font page"));
    }
    return false;
}

void ASCIIEditor::loadDefaultFont()
{
    if (font != NULL) {
        delete font;
    }
    glyphCache.clear();
    BitmapFontObject* newFont = new BitmapFontObject(IMGLoadAssetToSurface("codepage437-8x8-voidfont.png"), 437);
    font = newFont;
}

void ASCIIEditor::renderCharOnScreen(ASCIIChar ch, SDL_Rect onScreenRect)
{
    XY glyphSize = font->getGlyphSize();
    if (!glyphCache.contains(ch.ch)) {
        glyphCache[ch.ch] = font->getDirectCodepageGlyph(ch.ch);
    }
    if ((ch.background & 0xFF000000) != 0) {
        SDL_Color bgColor = uint32ToSDLColor(ch.background);
        SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        SDL_RenderFillRect(g_rd, &onScreenRect);
    }
    
    SDL_Color fgColor = uint32ToSDLColor(ch.foreground);

    GlyphData& g = glyphCache[ch.ch];

    SDL_SetTextureColorMod(g.texture, fgColor.r, fgColor.g, fgColor.b);
    SDL_RenderCopy(g_rd, g.texture, NULL, &onScreenRect);
}

void ASCIIEditor::resize(XY newSize)
{
    session->resize(newSize);
    updateCanvasSize();
}

ASCIISession* ASCIISession::fromTXT(PlatformNativePathString path)
{
    
    std::ifstream infile(path);
    if (infile.good()) {
        ASCIISession* ret = new ASCIISession({ 0,0 });
        std::string line;

        int y = 0;
        while (!infile.eof()) {
            std::string line;
            std::getline(infile, line);

            int x = 0;
            for (char c : line) {
                ret->setWithResize({ x++,y }, { (u8)c });
            }

            y++;
        }

        infile.close();
        return ret;
    }
    return NULL;
}

#pragma pack(push, 1)
struct rexpaintChar {
    u32 ch;
    u8 fgR;
    u8 fgG;
    u8 fgB;
    u8 bgR;
    u8 bgG;
    u8 bgB;
};
#pragma pack(pop)

ASCIISession* ASCIISession::fromRexpaintCompressed(PlatformNativePathString path)
{

    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        std::vector<u8> fileData;
        fseek(f, 0, SEEK_END);
        u64 size = ftell(f);
        fseek(f, 0, SEEK_SET);
        fileData.resize(size);
        fread(&fileData[0], 1, size, f);
        fclose(f);
        auto decompressed = decompressGzip(fileData.data(), size);

        if (decompressed.size() >= 16) {
            u8* next = decompressed.data();

            u32 version = *((u32*)next); next += 4;
            u32 nLayers = *((u32*)next); next += 4;
            u32 w = *((u32*)next); next += 4;
            u32 h = *((u32*)next); next += 4;

            if (decompressed.size() >= nLayers * w * h * sizeof(rexpaintChar) + 16) {

                ASCIISession* ret = new ASCIISession({ (int)w,(int)h });

                for (int l = 0; l < nLayers; l++) {
                    for (int x = 0; x < w; x++) {
                        for (int y = 0; y < h; y++) {
                            rexpaintChar xpChar = *((rexpaintChar*)next); next += sizeof(rexpaintChar);

                            u32 bgColor = PackRGBAtoARGB(xpChar.bgR, xpChar.bgG, xpChar.bgB, 255);
                            u32 fgColor = PackRGBAtoARGB(xpChar.fgR, xpChar.fgG, xpChar.fgB, 255);
                            ret->setWithResize({ x,y }, { (u8)xpChar.ch, fgColor, bgColor == 0xFFFF00F ? 0 : bgColor });
                            if (xpChar.ch > 255) {
                                logwarn("[XP] character outside codepage");
                            }
                        }
                    }

                    break;  //todo: multiple layers
                }

                return ret;
            }
        }
    }
    return NULL;
}
