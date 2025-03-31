#include "PopupQuickConvert.h"
#include "UIButton.h"
#include "FontRenderer.h"
#include "FileIO.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"
#include "Notification.h"
#include "UICheckbox.h"

PopupQuickConvert::PopupQuickConvert(std::string tt, std::string tx) {
    wxHeight = 200;
    UIButton* nbutton = new UIButton();
    nbutton->text = "Back";
    nbutton->position = XY{ wxWidth - 130, wxHeight - 40 };
    nbutton->wxHeight = 35;
    nbutton->wxWidth = 120;
    nbutton->setCallbackListener(0, this);
    wxsManager.addDrawable(nbutton);

    checkForceRGB = new UICheckbox("Always convert palettized formats to RGB", false);
	checkForceRGB->position = XY{ 20, 140 };
	checkForceRGB->setCallbackListener(EVENT_QUICKCONVERT_FORCE_RGB, this);
	wxsManager.addDrawable(checkForceRGB);

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

	makeTitleAndDesc(tt, tx);
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

void PopupQuickConvert::onDropFileEvent(SDL_Event evt)
{
    if (evt.type == SDL_DROPFILE) {

		std::string path = std::string(evt.drop.data);
		PlatformNativePathString outPath = convertStringOnWin32(path);

		MainEditor* session = loadAnyIntoSession(path);
		FileExporter* exporter = g_fileExporters[exporterIndex];
		doQuickConvert(session, outPath, exporter, checkForceRGB->isChecked());
    }
}

void PopupQuickConvert::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
	exporterIndex = index;
	pickExportFormat->text = name;
}

void PopupQuickConvert::doQuickConvert(MainEditor* session, PlatformNativePathString outPath, FileExporter* exporter, bool forceConvertRGB)
{
	if (exporter == NULL) {
		for (FileExporter*& e : g_fileExporters) {
			if (stringEndsWithIgnoreCase(outPath, convertStringOnWin32(e->extension()))) {
				exporter = e;
				break;
			}
		}
	}
	if (exporter == NULL) {
		g_addNotification(ErrorNotification("Error", "No exporter found for this file"));
		printf("No exporter found for file %s\n", convertStringToUTF8OnWin32(outPath).c_str());
		delete session;
		return;
	}

	if (session != NULL) {

		if (exporter->exportsWholeSession()) {

			if (session->isPalettized && (forceConvertRGB || (exporter->formatFlags() & FORMAT_PALETTIZED) == 0)) {
				MainEditor* rgbConvEditor = ((MainEditorPalettized*)session)->toRGBSession();
				delete session;
				session = rgbConvEditor;
			}

			outPath += convertStringOnWin32(exporter->extension());

			if (exporter->exportData(outPath, session)) {
				g_addNotification(SuccessNotification("Success", "Exported file"));
			}
			else {
				g_addNotification(ErrorNotification("Error", "Failed to export file"));
			}
		}
		else {
			Layer* l = NULL;

			if (session->isPalettized) {
				if (!forceConvertRGB && (exporter->formatFlags() & FORMAT_PALETTIZED) != 0) {
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

			if (!stringEndsWithIgnoreCase(outPath, convertStringOnWin32(exporter->extension()))) {
				outPath += convertStringOnWin32(exporter->extension());
			}

			if (exporter->exportData(outPath, l)) {
				g_addNotification(SuccessNotification("Success", "Exported file"));
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
