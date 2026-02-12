#include "PopupExportScaled.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"
#include "TabbedView.h"
#include "UIButton.h"
#include "UITextField.h"
#include "FileIO.h"
#include "Notification.h"
#include "settings.h"
#include "UILabel.h"
#include "PopupChooseFormat.h"

PopupExportScaled::PopupExportScaled(MainEditor* parent)
{
    setSize({ 600, 350 });
    this->caller = parent;

    TabbedView* tabbedView = new TabbedView({ {TL("vsp.exportscaled.integerscale")}, {TL("vsp.exportscaled.pixelsize")} });
    tabbedView->position = { 10, 50 };
    tabbedView->onTabSwitchedCallback = [this](TabbedView* v, int tab) {
        mode = (ExportMode)tab;
        recalc();
    };
    wxsManager.addDrawable(tabbedView);
    

    //integer scale tab
    UILabel* tabTitle = new UILabel(TL("vsp.exportscaled.integerscale.title"));
    tabTitle->position = { 10, 10 };
    tabTitle->fontsize = 20;
    tabbedView->tabs[0].wxs.addDrawable(tabTitle);

    tboxISX = new UITextField();
    tboxISX->position = { 10, 60 };
    tboxISX->setText("1");

    UILabel* l = new UILabel("x");
    l->position = { 125, 60 };

    tboxISY = new UITextField();
    tboxISY->position = { 140, 60 };
    tboxISY->setText("1");

    tboxISX->isNumericField = tboxISY->isNumericField = true;
    tboxISX->wxWidth = tboxISY->wxWidth = 110;

    tabbedView->tabs[0].wxs.addDrawable(tboxISX);
    tabbedView->tabs[0].wxs.addDrawable(l);
    tabbedView->tabs[0].wxs.addDrawable(tboxISY);


    //pixel size tab
    tabTitle = new UILabel(TL("vsp.exportscaled.pixelsize.title"));
    tabTitle->position = { 10, 10 };
    tabTitle->fontsize = 20;
    tabbedView->tabs[1].wxs.addDrawable(tabTitle);

    tboxPXSX = new UITextField();
    tboxPXSX->position = { 10, 60 };
    tboxPXSX->setText(std::to_string(caller->canvas.dimensions.x));

    l = new UILabel("x");
    l->position = { 125, 60 };

    tboxPXSY = new UITextField();
    tboxPXSY->position = { 140, 60 };
    tboxPXSY->setText(std::to_string(caller->canvas.dimensions.y));

    tboxPXSX->isNumericField = tboxPXSY->isNumericField = true;
    tboxPXSX->wxWidth = tboxPXSY->wxWidth = 110;

    tabbedView->tabs[1].wxs.addDrawable(tboxPXSX);
    tabbedView->tabs[1].wxs.addDrawable(l);
    tabbedView->tabs[1].wxs.addDrawable(tboxPXSY);


    labelOutputSize = new UILabel();
    labelOutputSize->position = { 15, wxHeight - labelOutputSize->fontsize - 30 };
    labelOutputSize->color = { 255,255,255,0xA0 };
    wxsManager.addDrawable(labelOutputSize);

    
    tboxISX->onTextChangedCallback = tboxISY->onTextChangedCallback = 
        tboxPXSX->onTextChangedCallback = tboxPXSY->onTextChangedCallback = 
        [this](UITextField* t, std::string text) {
            recalc();
        };

    recalc();
    genExporterList();

    makeTitleAndDesc(TL("vsp.maineditor.nav.exportscaled"), "");

    UIButton* closeButton = actionButton(TL("vsp.cmn.cancel"));
    closeButton->onClickCallback = [this](...) {
        closePopup();
    };
    UIButton* confirmButton = actionButton(TL("vsp.cmn.confirm"));
    confirmButton->onClickCallback = [this](...) {
        if (resultSize.x > 0 && resultSize.y > 0) {
            PopupChooseFormat* formatPopup = 
                caller->isPalettized ? PopupChooseFormat::withDefaultIndexedExportFormats("Choose format", "")
                : PopupChooseFormat::withDefaultRGBExportFormats("Choose format", "");
                
            formatPopup->chooseFormatAndDoFileSavePrompt(TL("vsp.popup.saveimage"), 
                [this](FormatDef* fmt, PlatformNativePathString path){
                    if (exportWithExporter((FileExporter*)fmt->udata, path)) {
                        closePopup();
                    }
                }
            );
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.exportscaled.invalidsize")));
        }
    };
}

bool PopupExportScaled::exportWithExporter(FileExporter* exporter, PlatformNativePathString name) {
    bool result = false;
    if (resultSize.x > 0 && resultSize.y > 0) {

        if (exporter->exportsWholeSession()) {
            MainEditor* newSession = NULL;

            std::vector<Frame*> nFrames;
            for (auto& f : caller->frames) {
                Frame* scaledFrame = new Frame();
                scaledFrame->activeLayer = f->activeLayer;
                for (Layer* l : f->layers) {
                    Layer* scaled = l->copyCurrentVariantScaled(resultSize);
                    scaledFrame->layers.push_back(scaled);
                }
                nFrames.push_back(scaledFrame);
            }

            newSession = caller->isPalettized ? new MainEditorPalettized(nFrames) : new MainEditor(nFrames);

            if (newSession != NULL) {
                newSession->activeFrame = caller->activeFrame;
                newSession->tileDimensions = caller->tileDimensions;
                newSession->selLayer = caller->selLayer;
                result = exporter->exportData(name, newSession);
                delete newSession;
            }

        }
        else {
            Layer* flat = NULL;
            if (caller->isPalettized) {
                MainEditorPalettized* pssn = (MainEditorPalettized*)caller;
                flat = pssn->flattenImageWithoutConvertingToRGB();
            }
            else {
                flat = caller->flattenImage();
            }

            if (flat != NULL) {
                Layer* scaled = flat->copyCurrentVariantScaled(resultSize);
                delete flat;
                result = exporter->exportData(name, scaled);
                delete scaled;
            }
        }

        g_addNotification(result ? SuccessNotification("Success", "File exported successfully.") :
            ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.exportfail")));
        if (result) {
#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
            emDownloadFile(lastConfirmedSavePath);
#endif
            if (g_config.openSavedPath) {
                platformOpenFileLocation(name);
            }
        }
    } else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.exportscaled.invalidsize")));
    }
    return result;
}

void PopupExportScaled::genExporterList() {
    exporterList = {};
    if (caller->isPalettized) {
        for (auto& e : g_palettizedFileExporters) {
            if ((e->formatFlags() & FORMAT_PALETTIZED) != 0) {
                exporterList.push_back(e);
            }
        }
    }
    else {
        for (auto f : g_fileExporters) {
            exporterList.push_back(f);
        }
    }
}

void PopupExportScaled::recalc()
{
    switch (mode) {
        case EXPORTMODE_INTEGERSCALE:
            try {
                XY newSize = { std::stoi(tboxISX->getText()), std::stoi(tboxISY->getText()) };
                XY canvasSize = caller->canvas.dimensions;
                resultSize = { canvasSize.x * newSize.x, canvasSize.y * newSize.y };
            }
            catch (std::exception&) {
                resultSize = { -1,-1 };
            }
            break;
        case EXPORTMODE_PIXELSCALE:
            try {
                XY newSize = { std::stoi(tboxPXSX->getText()), std::stoi(tboxPXSY->getText()) };
                resultSize = newSize;
            }
            catch (std::exception&) {
                resultSize = { -1,-1 };
            }
            break;
    }

    labelOutputSize->setText(
        resultSize.x >= 0 ? frmt("{}: {}x{}", TL("vsp.exportscaled.resultsize"), resultSize.x, resultSize.y)
        : TL("vsp.exportscaled.invalidsize")
    );
}
