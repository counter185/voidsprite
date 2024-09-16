#include "PopupQuickConvert.h"
#include "UIButton.h"
#include "FontRenderer.h"
#include "FileIO.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"
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
    for (auto& fmt : g_fileExporters) {
        formats.push_back(fmt->name());
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

		MainEditor* session = loadAnyIntoSession(path);
		if (session != NULL) {

			FileExporter* exporter = g_fileExporters[exporterIndex];

			if (exporter->exportsWholeSession()) {

				if (session->isPalettized && ((exporter->formatFlags() & FORMAT_PALETTIZED) == 0)) {
					MainEditor* rgbConvEditor = ((MainEditorPalettized*)session)->toRGBSession();
					delete session;
					session = rgbConvEditor;
				}

				outPath += convertStringOnWin32(exporter->extension());

				if (exporter->exportData(outPath, session)) {
					g_addNotification(Notification("Success", "Exported file", 4000, NULL, COLOR_INFO));
				}
				else {
					g_addNotification(ErrorNotification("Error", "Failed to export file"));
				}
			}
			else {
				Layer* l = NULL;

				if (session->isPalettized) {
					if ((exporter->formatFlags() & FORMAT_PALETTIZED) != 0) {
						MainEditorPalettized* upcastSession = (MainEditorPalettized*)session;
						l = upcastSession->flattenImageWithoutConvertingToRGB();
					}
					else {
						MainEditorPalettized* upcastSession = (MainEditorPalettized*)session;
						l = upcastSession->flattenImageAndConvertToRGB();
					}
				}
				else {
					l = session->flattenImage();
				}

				outPath += convertStringOnWin32(exporter->extension());

				if (exporter->exportData(outPath, l)) {
					g_addNotification(Notification("Success", "Exported file", 4000, NULL, COLOR_INFO));
				}
				else {
					g_addNotification(ErrorNotification("Error", "Failed to export file"));
				}
				delete l;
			}
			delete session;
		}
		else {
			g_addNotification(ErrorNotification("Error", "Failed to load file"));
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
