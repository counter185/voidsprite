#include "PopupChooseFormat.h"
#include "ScrollingPanel.h"
#include "Panel.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UIStackPanel.h"
#include "FileIO.h"

void PopupChooseFormat::buildFormatList() {
    formatsPanel->subWidgets.freeAllDrawables();

    std::vector<Drawable*> buttons;

    for (auto& f : formats) {
        FormatDef* ptr = &f;
        Panel* newButton = new Panel();
        newButton->sizeToContent = true;

        UIButton* btn = new UIButton(f.name);
        btn->position = {0,0};
        btn->wxWidth = formatsPanel->wxWidth - 40;
        btn->wxHeight = 70;
        btn->onClickCallback = [this, ptr](UIButton* b) { this->finish(ptr); };
        newButton->subWidgets.addDrawable(btn);

        newButton->subWidgets.addDrawable(new UILabel(f.extension, {8, 20}, 16));
        newButton->subWidgets.addDrawable(new UILabel(f.description, {8, 40}, 16));

        buttons.push_back(newButton);
    }

    UIStackPanel* buttonStack = UIStackPanel::Vertical(0, buttons);
    buttonStack->takeMouseWheelEvents = false;
    formatsPanel->subWidgets.addDrawable(buttonStack);
}

PopupChooseFormat::PopupChooseFormat(std::string tt, std::string tx, std::vector<FormatDef> f) : formats(f) {

    wxHeight = 500;
    makeTitleAndDesc(tt, tx);

    formatsPanel = new ScrollingPanel();
    formatsPanel->wxWidth = wxWidth - 10;
    formatsPanel->wxHeight = wxHeight - 100;
    formatsPanel->position = {5, 40};
    buildFormatList();
    wxsManager.addDrawable(formatsPanel);

    actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](UIButton* b){ this->closePopup(); };
}

PopupChooseFormat* PopupChooseFormat::withDefaultRGBExportFormats(std::string tt, std::string tx) {
    std::vector<FormatDef> ff;
    for (FileExporter* f : g_fileExporters) {
        if (f->formatFlags() & FORMAT_RGB) {
            ff.push_back({
                .name = f->name(),
                .extension = f->extension(),
                .description = "--test desc",
                .udata = (void*)f
            });
        }
    }

    return new PopupChooseFormat(tt, tx, ff);
}
PopupChooseFormat* PopupChooseFormat::withDefaultIndexedExportFormats(std::string tt, std::string tx) {
    std::vector<FormatDef> ff;
    for (FileExporter* f : g_fileExporters) {
        if (f->formatFlags() & FORMAT_PALETTIZED) {
            ff.push_back({
                .name = f->name(),
                .extension = f->extension(),
                .description = "--test desc",
                .udata = (void*)f
            });
        }
    }

    return new PopupChooseFormat(tt, tx, ff);
}

void PopupChooseFormat::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex) {
    if (!stringEndsWithIgnoreCase(convertStringToUTF8OnWin32(name), chosenTarget->extension)) {
        name += convertStringOnWin32(chosenTarget->extension);
    }
    if (onEventFileSavedCallback != NULL) {
        onEventFileSavedCallback(chosenTarget, name);
    }
    //g_startNewMainThreadOperation([this](){this->closePopup();});
    closePopup();
}

void PopupChooseFormat::chooseFormatAndDoFileSavePrompt(std::string promptTitle, std::function<void(FormatDef*,PlatformNativePathString)> callback) {
    onEventFileSavedCallback = callback;
    onFormatChosenCallback = [this, promptTitle](FormatDef* f) {
        platformTrySaveOtherFile(this, {{f->extension, f->name}}, promptTitle, 0);
    };
    g_addPopup(this);
}

void PopupChooseFormat::finish(FormatDef* target) {
    chosenTarget = target;
    if (chosenTarget != NULL && onFormatChosenCallback != NULL) {
        onFormatChosenCallback(chosenTarget);
    }
    if (onEventFileSavedCallback == NULL) {
        closePopup();
    }
}
