#include <fstream>

#include "ASCIIEditor.h"
#include "FontRenderer.h"

ASCIIEditor::ASCIIEditor(XY dimensions)
{
    lastRenderer = g_rd;
    session = new ASCIISession(dimensions);
    loadDefaultFont();
    resize(session->getSize());
    c.recenter();
}

ASCIIEditor::ASCIIEditor(ASCIISession* ssn)
{
    lastRenderer = g_rd;
    session = ssn;
    loadDefaultFont();
    resize(session->getSize());
	c.recenter();
}

ASCIIEditor::~ASCIIEditor()
{
    delete session;
}

void ASCIIEditor::render()
{
    if (lastRenderer != g_rd) {
        loadDefaultFont();
		lastRenderer = g_rd;
    }

    c.lockToScreenBounds();

	SDL_Rect renderArea = c.getCanvasOnScreenRect();
	XY glyphSize = font->getGlyphSize();

    for (int y = 0; y < session->getSize().y; y++) {
        SDL_Rect yCheck = c.getTileScreenRectAt({ 0,y }, glyphSize);
        if (yCheck.y >= g_windowH) {
            break;
        }
        if (yCheck.y + yCheck.h >= 0) {
            for (int x = 0; x < session->getSize().x; x++) {
                SDL_Rect onScreenRect = c.getTileScreenRectAt({ x,y }, glyphSize);
                renderCharOnScreen(session->get({ x,y }), onScreenRect);
            }
        }
    }

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x30);
    c.drawTileGrid(glyphSize);

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
        c.takeInput(evt);
    }
}

void ASCIIEditor::loadDefaultFont()
{
    if (font != NULL) {
        delete font;
    }
    BitmapFontObject* newFont = new BitmapFontObject(IMGLoadAssetToSurface("codepage437-8x8-voidfont.png"), 437);
    font = newFont;
}

void ASCIIEditor::renderCharOnScreen(ASCIIChar ch, SDL_Rect onScreenRect)
{
	XY glyphSize = font->getGlyphSize();
    auto glyphChar = font->getDirectCodepageGlyph(ch.ch);
	SDL_Color bgColor = uint32ToSDLColor(ch.background);
	SDL_Color fgColor = uint32ToSDLColor(ch.foreground);

	SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	SDL_RenderFillRect(g_rd, &onScreenRect);

    SDL_SetTextureColorMod(glyphChar.texture, fgColor.r, fgColor.g, fgColor.b);
    SDL_RenderCopy(g_rd, glyphChar.texture, NULL, &onScreenRect);
}

void ASCIIEditor::resize(XY newSize)
{
    session->resize(newSize);
    XY glyphSize = font->getGlyphSize();
    c.dimensions = { newSize.x * glyphSize.x, newSize.y * glyphSize.y };
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
