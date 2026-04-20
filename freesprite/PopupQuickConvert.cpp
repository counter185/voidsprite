#include "PopupQuickConvert.h"
#include "UIButton.h"
#include "FontRenderer.h"
#include "FileIO.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"
#include "Notification.h"
#include "UICheckbox.h"
#include "PopupChooseFormat.h"

PopupQuickConvert::PopupQuickConvert(std::string tt, std::string tx) {
    currentExporter = g_fileExporters.front();
    wxHeight = 200;
    UIButton* nbutton = actionButton(TL("vsp.cmn.close"));
    nbutton->onClickCallback = [this](UIButton*) {
        closePopup();
    };

    UICheckbox* checkForceRGB = new UICheckbox(TL("vsp.quickconvert.forceindexedtorgb"), &forceRGB);
    checkForceRGB->position = XY{ 20, 140 };
    wxsManager.addDrawable(checkForceRGB);

    std::vector<std::string> formats;
    for (auto& fmt : g_fileExporters) {
        formats.push_back(fmt->name());
    }

    UIButton* pickExporterButton = new UIButton();
    pickExporterButton->position = XY{ 20, 100 };
    pickExporterButton->wxWidth = 400;
    pickExporterButton->wxHeight = 30;
    pickExporterButton->onClickCallback = [this](UIButton* b) {
        PopupChooseFormat* formatPicker = PopupChooseFormat::withDefaultExportFormats("Choose format", "");
        formatPicker->onFormatChosenCallback = [this, b](FormatDef* f) {
            b->text = f->name;
            currentExporter = (FileExporter*)f->udata;
        };
        g_addPopup(formatPicker);
    };
    pickExporterButton->text = currentExporter->name();
    wxsManager.addDrawable(pickExporterButton);

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

        g_startNewOperation([this, path, outPath](OperationProgressReport* progress) {
            MainEditor* session = loadAnyIntoSession(path, NULL, progress);
            progress->resetProgress();
            doQuickConvert(session, outPath, currentExporter, forceRGB, progress);
        });
    }
}

void PopupQuickConvert::doQuickConvert(
    MainEditor* session, 
    PlatformNativePathString outPath, 
    FileExporter* exporter, 
    bool forceConvertRGB,
    OperationProgressReport* progress)
{
    ENSURE_REPORT_VALID(progress);
    if (exporter == NULL) {
        for (FileExporter*& e : g_fileExporters) {
            if (stringEndsWithIgnoreCase(outPath, convertStringOnWin32(e->extension()))) {
                exporter = e;
                break;
            }
        }
    }
    if (exporter == NULL) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "No exporter found for this file"));
        logwarn(frmt("No exporter found for file {}", convertStringToUTF8OnWin32(outPath)));
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

            if (exporter->exportData(outPath, session, progress, NULL)) {
                g_addNotification(SuccessNotification("Success", "Exported file"));
            }
            else {
                g_addNotification(ErrorNotification("Error", "Failed to export file"));
            }
        }
        else {
            Layer* l = NULL;

            if (session->isPalettized) {
                MainEditorPalettized* upcastSession = (MainEditorPalettized*)session;
                if (!forceConvertRGB && (exporter->formatFlags() & FORMAT_PALETTIZED) != 0) {
                    l = upcastSession->flattenImageWithoutConvertingToRGB();
                }
                else {
                    l = upcastSession->flattenImageAndConvertToRGB(upcastSession->getCurrentFrame());
                }
            }
            else {
                l = session->flattenImage();
            }

            if (!stringEndsWithIgnoreCase(outPath, convertStringOnWin32(exporter->extension()))) {
                outPath += convertStringOnWin32(exporter->extension());
            }

            if (exporter->exportData(outPath, l, progress, NULL)) {
#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
                emDownloadFile(outPath);
#endif
                g_addNotification(SuccessNotification("Success", "Exported file"));
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to export file"));
            }
            delete l;
        }
        delete session;
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.fileloadfail")));
    }
}
