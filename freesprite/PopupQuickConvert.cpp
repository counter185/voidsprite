#include "PopupQuickConvert.h"
#include "UIButton.h"
#include "FontRenderer.h"
#include "FileIO.h"
#include "maineditor.h"
#include "Notification.h"

PopupQuickConvert::PopupQuickConvert(std::string tt, std::string tx) {
    this->title = tt;
    this->text = tx;
    wxHeight = 200;
    UIButton* nbutton = new UIButton();
    nbutton->text = "Back";
    nbutton->position = XY{ wxWidth - 130, wxHeight - 40 };
    nbutton->wxHeight = 35;
    nbutton->wxWidth = 120;
    nbutton->setCallbackListener(0, this);
    wxsManager.addDrawable(nbutton);

    std::vector<std::string> formats;
    for (auto& fmt : g_fileExportersFlatNPaths) {
        formats.push_back(fmt.name);
    }
    for (auto& fmt : g_fileExportersMLNPaths) {
        formats.push_back(fmt.name);
    }

	exporterIndex = 0;

    pickExportFormat = new UIDropdown(formats);
    pickExportFormat->position = XY{ 20, 100 };
    pickExportFormat->wxWidth = 400;
    pickExportFormat->wxHeight = 30;
	pickExportFormat->text = formats[exporterIndex];
    pickExportFormat->setCallbackListener(EVENT_QUICKCONVERT_PICKFORMAT, this);
	pickExportFormat->genButtons();
	wxsManager.addDrawable(pickExportFormat);
}

void PopupQuickConvert::takeInput(SDL_Event evt)
{
	if (evt.type == SDL_DROPFILE) {
		onDropFileEvent(evt);
	}
	else {
		BasePopup::takeInput(evt);
	}
}

void PopupQuickConvert::render()
{
    renderDefaultBackground();

    XY titlePos = getDefaultTitlePosition();
    XY contentPos = getDefaultContentPosition();

    g_fnt->RenderString(title, titlePos.x, titlePos.y);
    g_fnt->RenderString(text, contentPos.x, contentPos.y);

    renderDrawables();
}

void PopupQuickConvert::onDropFileEvent(SDL_Event evt)
{
    if (evt.type == SDL_DROPFILE) {

		std::string path = std::string(evt.drop.file);
		PlatformNativePathString outPath = convertStringOnWin32(path);

		SDL_free(evt.drop.file);

		if (exporterIndex >= g_fileExportersFlatNPaths.size()) {
			int realExporterIndex = exporterIndex - g_fileExportersFlatNPaths.size();
			FileExportMultiLayerNPath exporter = g_fileExportersMLNPaths[realExporterIndex];

			MainEditor* session = loadSession(path);
			if (session == NULL) {
				Layer* l = loadFlat(path);
				if (l != NULL) {
					session = new MainEditor(l);
				}
				else {
					g_addNotification(ErrorNotification("Error", "Failed to load file"));
					return;
				}
			}

			outPath += convertStringOnWin32(exporter.extension);

			if (exporter.exportFunction(outPath, session)) {
				g_addNotification(Notification("Success", "Exported file", 4000, NULL, COLOR_INFO));
			}
			else {
				g_addNotification(ErrorNotification("Error", "Failed to export file"));
			}

			delete session;
		}
		else {
			FileExportFlatNPath exporter = g_fileExportersFlatNPaths[exporterIndex];

			Layer* l = loadFlat(path);
			if (l == NULL) {
				MainEditor* session = loadSession(path);
				if (session != NULL) {
					l = session->flattenImage();
					delete session;
				} else {
					g_addNotification(ErrorNotification("Error", "Failed to load file"));
					return;
				}
			}

			outPath += convertStringOnWin32(exporter.extension);

			if (exporter.exportFunction(outPath, l)) {
				g_addNotification(Notification("Success", "Exported file", 4000, NULL, COLOR_INFO));
			}
			else {
				g_addNotification(ErrorNotification("Error", "Failed to export file"));
			}
		}
    }
}

void PopupQuickConvert::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
	exporterIndex = index;
	pickExportFormat->text = name;
}

MainEditor* PopupQuickConvert::loadSession(std::string path)
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
				return session;
			}
			else {
				printf("%s: load failed\n", importer.name.c_str());
			}
		}
	}
	return NULL;
}

Layer* PopupQuickConvert::loadFlat(std::string path)
{

	PlatformNativePathString fPath;
#if _WIDEPATHS
	fPath = utf8StringToWstring(path);
#else
	fPath = path;
#endif

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
			return l;
		}
		else {
			//g_addPopup(new PopupMessageBox("", "Failed to load file."));
			//g_addNotification(Notification("Error", "Failed to load file", 6000, NULL, COLOR_ERROR));
			printf("No importer for file available\n");
		}
	}
	return NULL;
}
