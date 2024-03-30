#include "StartScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "FileIO.h"

void StartScreen::tick() {

}

void StartScreen::render()
{
	SDL_Rect logoRect = SDL_Rect{ 4, g_windowH - 4 - 40 * 4, 128 * 4, 40 * 4 };
	SDL_RenderCopy(g_rd, g_mainlogo, NULL, &logoRect);
	g_fnt->RenderString("r21.03.2024", 2, g_windowH - 20 - 20, SDL_Color{255,255,255,0x50});

	SDL_Rect bgr = SDL_Rect{ 0, 35, 560, 300 };
	SDL_SetRenderDrawColor(g_rd, 0x20, 0x20, 0x20, 0xa0);
	SDL_RenderFillRect(g_rd, &bgr);

	g_fnt->RenderString("voidsprite", 10, 40);

	g_fnt->RenderString("New image", 10, 80);

	wxsManager.renderAll();
}

void StartScreen::takeInput(SDL_Event evt)
{
	if (evt.type == SDL_QUIT) {
		g_closeLastScreen();
		return;
	}

	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
		wxsManager.tryFocusOnPoint(XY{ evt.button.x, evt.button.y });
	}

	if (!wxsManager.anyFocused() || evt.type == SDL_DROPFILE) {
		switch (evt.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEWHEEL:
				break;
			case SDL_KEYDOWN:
				break;
			case SDL_DROPFILE:
				std::string filePath = evt.drop.file;
				std::string extension = filePath.substr(filePath.find_last_of('.'));
				PlatformNativePathString fPath;
#if _WIN32
				fPath = utf8StringToWstring(filePath);
#else
				fPath = filePath;
#endif
				if (extension == ".xyz") {
					Layer* nlayer = readXYZ(fPath);
					if (nlayer == NULL) {
						printf("xyz load failed");
					}
					else {
						g_addScreen(new MainEditor(nlayer));
					}
				}
				else if (extension == ".png") {
					Layer* nlayer = readPNG(fPath);
					if (nlayer == NULL) {
						printf("png load failed");
					}
					else {
						g_addScreen(new MainEditor(nlayer));
					}
				}
				else if (extension == ".aetex") {
					Layer* nlayer = readAETEX(fPath);
					if (nlayer == NULL) {
						printf("aetex load failed");
					}
					else {
						g_addScreen(new MainEditor(nlayer));
					}
				}
				else if (extension == ".voidsn" || extension == ".voidsnv1") {
					MainEditor* session = readVOIDSN(fPath);
					if (session == NULL) {
						printf("voidsession load failed");
					}
					else {
						g_addScreen(session);
					}
				}
				else {
					SDL_Surface* srf = IMG_Load(filePath.c_str());
					if (srf == NULL) {
						printf("imageload failed: %s\n", filePath.c_str());
					}
					else {
						g_addScreen(new MainEditor(srf));
						SDL_FreeSurface(srf);
					}
				}
				SDL_free(evt.drop.file);
				break;
		}
	}
	else {
		wxsManager.passInputToFocused(evt);
	}
}
