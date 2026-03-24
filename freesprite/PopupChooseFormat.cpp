#include "PopupChooseFormat.h"
#include "ScrollingPanel.h"
#include "Panel.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UIStackPanel.h"
#include "FileIO.h"
#include "FontRenderer.h"
#include "UITextField.h"
#include "PopupContextMenu.h"

void PopupChooseFormat::buildFormatList() {
    formatsPanel->subWidgets.freeAllDrawables();

    std::vector<Drawable*> priority1Buttons;
    std::vector<Drawable*> priority2Buttons;

    for (auto& f : formats) {
        if (filterQuery.empty() || stringContainsIgnoreCase(f.name, filterQuery) || stringContainsIgnoreCase(f.extension, filterQuery)) {
            FormatDef* ptr = &f;
            Panel* newButton = new Panel();
            newButton->sizeToContent = true;

            bool isFav = stringStartsWithIgnoreCase(f.name, "\xE2\x98\x85");

            std::string name = f.name;
            std::string originalName = f.originalName;

            UIButton* btn = new UIButton(name);
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

            if (isFav) {
                btn->onRightClickCallback = [this, originalName](...) {
                    g_addPopup(new PopupContextMenu({
                        {"Remove from favourites", [this, originalName]() {
                            auto indexAt = std::find(g_config.favExportFormats.begin(), g_config.favExportFormats.end(), originalName);
                            if (indexAt != g_config.favExportFormats.end()) {
                                g_config.favExportFormats.erase(indexAt);
                                g_saveConfig();
                                filterList(filterQuery);
                            }
                            else {
                                logerr("failed to remove fav format");
                            }
                        }}
                    }));
                };
                btn->fill = Fill::Gradient(0xD02D2C19, 0xD0000000, 0xD02D2C19, 0xD0000000);
                btn->colorTextFocused = btn->colorTextUnfocused = SDL_Color{ 0xFF, 0xF3, 0x8A, 0xff };
                priority1Buttons.push_back(newButton);
            }
            else {
                btn->onRightClickCallback = [this, originalName](...) {
                    g_addPopup(new PopupContextMenu({
                        {"Add to favourites", [this, originalName]() {
                            g_config.favExportFormats.push_back(originalName);
                            g_saveConfig();
                            filterList(filterQuery);
                        }}
                    }));
                };
                priority2Buttons.push_back(newButton);
            }
        }
    }

    if (!priority1Buttons.empty()) {
        UILabel* favsLabel = new UILabel("Favourites", { 0,0 }, 23);
        favsLabel->color = SDL_Color{ 0xFF, 0xF3, 0x8A, 0xff };
        priority1Buttons.insert(priority1Buttons.begin(), UIStackPanel::Horizontal(0, {
            Panel::Space(5, 0),
            favsLabel
        }));
        priority1Buttons.insert(priority1Buttons.begin()+1, Panel::Space(0, 5));
    }

    UIStackPanel* finalStack = UIStackPanel::Vertical(30, { 
        UIStackPanel::Vertical(0, priority1Buttons), 
        UIStackPanel::Vertical(0, priority2Buttons) 
    });
    finalStack->takeMouseWheelEvents = false;

    formatsPanel->subWidgets.addDrawable(finalStack);
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

PopupChooseFormat* PopupChooseFormat::withDefaultExportFormats(std::string tt, std::string tx, u32 formatFlags) {
    std::vector<FormatDef> ff;
    for (FileExporter* f : g_fileExporters) {
        if (formatFlags == 0 || (f->formatFlags() & formatFlags)) {
            ff.push_back({
                .name = f->name(),
                .extension = f->extension(),
                .description = f->description,
                .udata = (void*)f
            });
        }
    }
    std::vector<FormatDef> ff2 = processFavouriteFormats(ff);
    PopupChooseFormat* ret = new PopupChooseFormat(tt, tx, ff2);
    ret->hasDefaultFormats = true;
    ret->defaultFormats = ff;
    return ret;
}
PopupChooseFormat* PopupChooseFormat::withDefaultRGBExportFormats(std::string tt, std::string tx) {
    return withDefaultExportFormats(tt, tx, FORMAT_RGB);
}
PopupChooseFormat* PopupChooseFormat::withDefaultIndexedExportFormats(std::string tt, std::string tx) {
    return withDefaultExportFormats(tt, tx, FORMAT_PALETTIZED);
}

PopupChooseFormat* PopupChooseFormat::withDefaultPaletteExportFormats(std::string tt, std::string tx)
{
    std::vector<FormatDef> ff;
    for (PaletteExporter* f : g_paletteExporters) {
        ff.push_back({
            .name = f->name(),
            .extension = f->extension(),
            .description = f->description,
            .udata = (void*)f
        });
    }
    std::vector<FormatDef> ff2 = processFavouriteFormats(ff);
    PopupChooseFormat* ret = new PopupChooseFormat(tt, tx, ff2);
    ret->hasDefaultFormats = true;
    ret->defaultFormats = ff;
    return ret;
}

std::vector<FormatDef> PopupChooseFormat::processFavouriteFormats(std::vector<FormatDef> srcList) {
    std::vector<FormatDef> ret;
    for (auto& f : srcList) {
        f.originalName = f.name;
        if (std::find(g_config.favExportFormats.begin(), g_config.favExportFormats.end(), f.name) != g_config.favExportFormats.end()) {
            f.name = "\xE2\x98\x85 " + f.name;
            ret.insert(ret.begin(), f);
        } else {
            ret.push_back(f);
        }
    }
    return ret;
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
    if (hasDefaultFormats) {
        formats = processFavouriteFormats(defaultFormats);
    }
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
