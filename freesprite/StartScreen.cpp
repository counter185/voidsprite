#include "StartScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "FileIO.h"
#include "PopupMessageBox.h"
#include "Notification.h"

void StartScreen::tick() {
	if (closeNextTick) {
		g_closeScreen(this);
	}
}

void StartScreen::render()
{
	SDL_Rect logoRect = SDL_Rect{ 4, g_windowH - 4 - 40 * 4, 128 * 4, 40 * 4 };
	SDL_RenderCopy(g_rd, g_mainlogo, NULL, &logoRect);
	g_fnt->RenderString(std::format("alpha@{}", __DATE__), 2, g_windowH - 20 - 20, SDL_Color{255,255,255,0x50});

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

	if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_LALT) {
		wxsManager.forceFocusOn(navbar);
		return;
	}

	if (!wxsManager.anyFocused() || evt.type == SDL_DROPFILE) {
		switch (evt.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if (evt.button.button == SDL_BUTTON_LEFT) {
					if (evt.button.state) {
						SDL_Rect logoRect = SDL_Rect{ 4, g_windowH - 4 - 40 * 4, 128 * 4, 40 * 4 };
						if (pointInBox({ evt.button.x, evt.button.y }, logoRect)) {
							g_addNotification(Notification("voidsprite alpha", "by counter185 & contributors", 6000, g_iconNotifTheCreature, COLOR_INFO));
						}
					}
				}
				break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEWHEEL:
				break;
			case SDL_KEYDOWN:
				if (evt.key.keysym.sym == SDLK_v && g_ctrlModifier) {
					tryOpenImageFromClipboard();
				}
				break;
			case SDL_DROPFILE:
			{
				std::string filePath = evt.drop.file;
				//std::string extension = filePath.substr(filePath.find_last_of('.'));
				tryLoadFile(filePath);
				SDL_free(evt.drop.file);
			}
				break;
		}
	}
	else {
		wxsManager.passInputToFocused(evt);
	}
}

void StartScreen::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex) {
	wprintf(L"path: %s, index: %i\n", name.c_str(), importerIndex);
	importerIndex--;
	if (importerIndex >= g_fileSessionImportersNPaths.size()) {
		importerIndex -= g_fileSessionImportersNPaths.size();
		Layer* nlayer = g_fileImportersNPaths[importerIndex].importFunction(name, 0);
		if (nlayer != NULL) {
			g_addScreen(new MainEditor(nlayer));
		}
		else {
			g_addNotification(Notification("Error", "Failed to load file", 5000, NULL, COLOR_ERROR));
		}
	}
	else {
		MainEditor* session = g_fileSessionImportersNPaths[importerIndex].importFunction(name);
		if (session != NULL) {
			g_addScreen(session);
		}
		else {
			g_addNotification(Notification("Error", "Failed to load file", 5000, NULL, COLOR_ERROR));
		}
	}
}

void StartScreen::tryLoadFile(std::string path)
{
	PlatformNativePathString fPath;
#if _WIDEPATHS
	fPath = utf8StringToWstring(path);
#else
	fPath = path;
#endif

	for (FileSessionImportNPath importer : g_fileSessionImportersNPaths) {
		if (stringEndsWithIgnoreCase(path, importer.extension) && importer.canImport(fPath)) {
			MainEditor* session = importer.importFunction(fPath);
			if (session != NULL) {
				g_addScreen(session);
				return;
			}
			else {
				printf("%s: load failed\n", importer.name.c_str());
			}
		}
	}
	/*if (stringEndsWith(path, ".voidsn") || stringEndsWith(path, ".voidsnv1")) {
		MainEditor* session = readVOIDSN(fPath);
		if (session == NULL) {
			printf("voidsession load failed");
		}
		else {
			g_addScreen(session);
		}
	}*/
	{

		Layer* l = NULL;
		for (FileImportNPath importer : g_fileImportersNPaths) {
			if (stringEndsWithIgnoreCase(path, importer.extension) && importer.canImport(fPath)) {
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
				if (stringEndsWithIgnoreCase(path, importer.extension) && importer.canImport(path)) {
					l = importer.importFunction(path, 0);
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
			g_addScreen(l->isPalettized ? new MainEditorPalettized((LayerPalettized*)l) : new MainEditor(l));
		}
		else {
			//g_addPopup(new PopupMessageBox("", "Failed to load file."));
			g_addNotification(Notification("Error", "Failed to load file", 6000, NULL, COLOR_ERROR));
			printf("No importer for file available\n");
		}
	}
}

void StartScreen::tryOpenImageFromClipboard()
{
	Layer* l = platformGetImageFromClipboard();
	if (l != NULL) {
		g_addScreen(new MainEditor(l));
	}
	else {
		g_addNotification(ErrorNotification("Error", "No image in clipboard"));
	}
}
