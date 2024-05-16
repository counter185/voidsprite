#include "StartScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "FileIO.h"
#include "PopupMessageBox.h"

void StartScreen::tick() {

}

void StartScreen::render()
{
	SDL_Rect logoRect = SDL_Rect{ 4, g_windowH - 4 - 40 * 4, 128 * 4, 40 * 4 };
	SDL_RenderCopy(g_rd, g_mainlogo, NULL, &logoRect);
	g_fnt->RenderString("alpha16.05.2024", 2, g_windowH - 20 - 20, SDL_Color{255,255,255,0x50});

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
		g_closeScreen(this);
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
				//std::string extension = filePath.substr(filePath.find_last_of('.'));
				PlatformNativePathString fPath;
#if _WIN32
				fPath = utf8StringToWstring(filePath);
#else
				fPath = filePath;
#endif

				if (stringEndsWith(filePath, ".voidsn") || stringEndsWith(filePath, ".voidsnv1")) {
					MainEditor* session = readVOIDSN(fPath);
					if (session == NULL) {
						printf("voidsession load failed");
					}
					else {
						g_addScreen(session);
					}
				}
				else {

					Layer* l = NULL;
					for (FileImportNPath importer : g_fileImportersNPaths) {
						if (stringEndsWith(filePath, importer.extension) && importer.canImport(fPath)) {
							l = importer.importFunction(fPath, 0);
							if (l != NULL) {
								break;
							}
							else {
								printf("%s : load failed\n", importer.name.c_str());
							}
						}
					}
					if (l == NULL) {
						for (FileImportUTF8Path importer : g_fileImportersU8Paths) {
							if (stringEndsWith(filePath, importer.extension) && importer.canImport(filePath)) {
								l = importer.importFunction(filePath, 0);
								if (l != NULL) {
									break;
								}
								else {
									printf("%s : load failed\n", importer.name.c_str());
								}
							}
						}
					}

					if (l != NULL) {
						g_addScreen(new MainEditor(l));
					}
					else {
						g_addPopup(new PopupMessageBox("", "Failed to load file."));
						printf("No importer for file available\n");
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
