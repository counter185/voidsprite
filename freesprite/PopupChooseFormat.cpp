#include "PopupChooseFormat.h"
#include "ScrollingPanel.h"
#include "Panel.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UIStackPanel.h"
#include "FileIO.h"
#include "FontRenderer.h"
#include "UITextField.h"

void PopupChooseFormat::buildFormatList() {
    formatsPanel->subWidgets.freeAllDrawables();

    std::vector<Drawable*> buttons;

    for (auto& f : formats) {
        if (filterQuery.empty() || stringContainsIgnoreCase(f.name, filterQuery) || stringContainsIgnoreCase(f.extension, filterQuery)) {
            FormatDef* ptr = &f;
            Panel* newButton = new Panel();
            newButton->sizeToContent = true;

            UIButton* btn = new UIButton(f.name);
            btn->position = { 0,0 };
            btn->wxWidth = formatsPanel->wxWidth - 40;
            btn->fontSize = 22;
            btn->wxHeight = f.description.empty() ? 35 : (40 + g_fnt->StatStringDimensions(f.description, 15).y);
            btn->onClickCallback = [this, ptr](UIButton* b) { this->finish(ptr); };
            newButton->subWidgets.addDrawable(btn);

            UILabel* extLabel = new UILabel(f.extension, { g_fnt->StatStringDimensions(f.name, 22).x + 20, 5 }, 16);
            extLabel->color = SDL_Color{ 255,255,255, 100 };
            newButton->subWidgets.addDrawable(extLabel);

            if (!f.description.empty()) {
                UILabel* descLabel = new UILabel(f.description, { 8, 28 }, 15);
                descLabel->color = SDL_Color{ 255,255,255, 130 };
                newButton->subWidgets.addDrawable(descLabel);
            }

            buttons.push_back(newButton);
        }
    }

    UIStackPanel* buttonStack = UIStackPanel::Vertical(0, buttons);
    buttonStack->takeMouseWheelEvents = false;
    formatsPanel->subWidgets.addDrawable(buttonStack);
}

PopupChooseFormat::PopupChooseFormat(std::string tt, std::string tx, std::vector<FormatDef> f) : formats(f) {

    wxWidth = 750;
    wxHeight = 500;
    makeTitleAndDesc(tt, tx);

    formatsPanel = new ScrollingPanel();
    formatsPanel->wxWidth = wxWidth - 10;
    formatsPanel->wxHeight = wxHeight - 120;
    buildFormatList();

    UITextField* searchField = new UITextField();
    searchField->wxWidth = 200;
    searchField->onTextChangedCallback = [this](UITextField*, std::string s) {
        filterList(s);
    };

    UIStackPanel* searchAndListStack = 
        UIStackPanel::Vertical(5, {
            UIStackPanel::Horizontal(20, {
                new UILabel(TL("vsp.cmn.search")),
                searchField
            }),
            formatsPanel
        });
    searchAndListStack->position = { 5,40 };
    wxsManager.addDrawable(searchAndListStack);

    actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](UIButton* b){ this->closePopup(); };
}

PopupChooseFormat* PopupChooseFormat::withDefaultRGBExportFormats(std::string tt, std::string tx) {
    std::vector<FormatDef> ff;
    for (FileExporter* f : g_fileExporters) {
        if (f->formatFlags() & FORMAT_RGB) {
            ff.push_back({
                .name = f->name(),
                .extension = f->extension(),
                .description = f->description,
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
                .description = f->description,
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

void PopupChooseFormat::filterList(std::string search)
{
    filterQuery = search;
    buildFormatList();
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
