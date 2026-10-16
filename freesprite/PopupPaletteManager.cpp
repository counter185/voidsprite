#include "PopupPaletteManager.h"
#include "UIButton.h"
#include "ScrollingPanel.h"
#include "UITextField.h"
#include "UIStackPanel.h"
#include "UILabel.h"
#include "PopupTextBox.h"
#include "PopupYesNo.h"
#include "Notification.h"
#include "PopupPickColor.h"
#include "PopupContextMenu.h"
#include "UIColorPicker.h"

UIButton* PopupPaletteManager::genColorButton(IPalette* p, NamedColor& c, int rIndex, int cIndex, bool editable, bool canRename)
{
    UIButton* ret = new UIButton();
    ret->wxWidth = 20;
    ret->wxHeight = 20;
    ret->fill = Fill::Solid(c.value);
    ret->tooltip = c.name;
    if (editable) {
        ret->onRightClickCallback = [this, rIndex, cIndex, p, c, canRename](UIButton*) {
            std::vector<NamedOperation> colorOperations = {
                { "Remove color",
                    [this, rIndex, cIndex, c, p]() {
                        g_addPopup(PopupYesNo::ConfirmPrompt(
                            "Remove color",
                            frmt("Remove the color #{} : {:06X} from palette {}?", cIndex, c.value, p->getName()),
                            [p, cIndex, rIndex]() {
                                p->removeColor(cIndex, rIndex);
                                g_reloadColorMap();
                            }));
                    }
                },
                { "Move color left",
                    [this, cIndex, p, rIndex]() {
                        p->moveColor(cIndex, false, rIndex);
                        g_reloadColorMap();
                    }
                },
                { "Move color right",
                    [this, cIndex, p, rIndex]() {
                        p->moveColor(cIndex, true, rIndex);
                        g_reloadColorMap();
                    }
                },
            };

            if (canRename) {
                colorOperations.push_back({
                    "Rename color", [p, c, cIndex, rIndex]() {
                        PopupTextBox* tx = new PopupTextBox("Rename color", frmt("Rename color #{} : {:06X}", cIndex, c.value), c.name);
                        tx->onTextInputConfirmedCallback = [p, rIndex, cIndex](PopupTextBox*, std::string t) {
                            p->getColorList()[rIndex].colors[cIndex].name = t;
                            p->save();
                            g_reloadColorMap();
                        };
                        g_addPopup(tx);
                    }
                });
            }

            PopupContextMenu* menu = new PopupContextMenu(colorOperations);
            g_addPopup(menu);
        };
    }
    return ret;
}

void PopupPaletteManager::reloadPaletteList()
{
    std::string filterQuery = lastFilterQuery;
    mainPanel->subWidgets.freeAllDrawables();
    std::vector<Drawable*> stack;
    for (IPalette*& palette : g_loadedPalettes) {

        std::string name = palette->getName();
        if (filterQuery == "" || stringContainsIgnoreCase(name, filterQuery)) {

            bool formatSupportsSections = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_EXT);
            bool formatSupportsLocking = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_EXT);
            bool formatSupportsName = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_NAME);
            bool formatSupportsDescription = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_DESCRIPTION);
            bool formatSupportsColorRenames = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_COLOR_NAMES);
            bool formatIsImported = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_IMPORTED);
            bool editable = palette->canSave();
            bool hidden = paletteIsHidden(palette);

            UILabel* descLabel = NULL;
            if (!palette->getDescription().empty()) {
                descLabel = new UILabel(palette->getDescription(), {}, 12);
                descLabel->color = uint32ToSDLColor(0xA0FFFFFF);
            }

            std::vector<Drawable*> paletteActionsRow = {};

            UIButton* showHideButton = new UIButton(hidden ? "Unhide" : "Hide");
            showHideButton->fontSize = 14;
            showHideButton->wxWidth = 80;
            showHideButton->wxHeight = 22;
            showHideButton->onClickCallback = [this, palette](...) {togglePaletteHidden(palette); g_reloadColorMap(); };
            paletteActionsRow.push_back(showHideButton);

            if (formatIsImported) {
                UIButton* deleteButton = new UIButton("Delete...");
                deleteButton->fontSize = 14;
                deleteButton->wxWidth = 80;
                deleteButton->wxHeight = 22;
                deleteButton->onClickCallback = [this, palette](...) {
                    g_addPopup(PopupYesNo::ConfirmPrompt(frmt("Delete {}?", palette->getName()), "The palette will be deleted.",
                        [palette]() {
                            try {
                                std::filesystem::remove(std::filesystem::path(((ImportedPalette*)palette)->path));
                                g_reloadColorMap();
                            }
                            catch (std::exception& e) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), e.what()));
                            }
                        }));
                    };
                paletteActionsRow.push_back(deleteButton);
            }

            if (editable && formatSupportsName) {
                UIButton* renameButton = new UIButton("Rename...");
                renameButton->fontSize = 14;
                renameButton->wxWidth = 100;
                renameButton->wxHeight = 22;
                renameButton->onClickCallback = [this, palette](...) { 
                    PopupTextBox* rename = new PopupTextBox("Rename palette", "Choose the new name for this palette:", palette->getName());
                    rename->onTextInputConfirmedCallback = [this, palette](PopupTextBox*, std::string s) {
                        ((ColorPalette*)palette)->name = s;
                        palette->save();
                        g_reloadColorMap();
                    };
                    g_addPopup(rename);
                };
                paletteActionsRow.push_back(renameButton);
            }

            if (formatIsImported && formatSupportsLocking) {
                UIButton* lockButton = new UIButton(palette->getLocked() ? "Unlock" : "Lock");
                lockButton->tooltip = "Lock to prevent accidental edits.\n"
                                      "The palette will not be editable until it is unlocked.";
                lockButton->fontSize = 14;
                lockButton->wxWidth = 100;
                lockButton->wxHeight = 22;
                lockButton->onClickCallback = [this, palette](...) {
                    ((ImportedPalette*)palette)->locked = !palette->getLocked();
                    palette->save();
                    g_reloadColorMap();
                };
                paletteActionsRow.push_back(lockButton);
            }

            if (editable && formatSupportsDescription) {
                UIButton* descButton = new UIButton("Edit description...");
                descButton->fontSize = 14;
                descButton->wxWidth = 130;
                descButton->wxHeight = 22;
                descButton->onClickCallback = [this, palette](...) { 
                    PopupTextBox* rename = new PopupTextBox("Edit palette description", "Enter the new description for this palette:", palette->getDescription());
                    rename->onTextInputConfirmedCallback = [this, palette](PopupTextBox*, std::string s) {
                        ((ColorPalette*)palette)->description = s;
                        palette->save();
                        g_reloadColorMap();
                    };
                    g_addPopup(rename);
                };
                paletteActionsRow.push_back(descButton);
            }

            std::string url = palette->getExternalUrl();
            if (!url.empty()) {
                UIButton* openExternal = new UIButton("View online...");
                openExternal->fontSize = 14;
                openExternal->wxWidth = 100;
                openExternal->wxHeight = 22;
                openExternal->tooltip = url;
                openExternal->onClickCallback = [url](...) { platformOpenWebpageURL(url); };
                paletteActionsRow.push_back(openExternal);
            }

            //make palette rows
            std::vector<Drawable*> paletteRows;
            if (!hidden) {
                auto& rows = palette->getColorList();
                int rowIndex = 0;
                for (auto& [rowName, rowColors] : rows) {
                    //don't show <no name> if format doesn't support this
                    if (!rowName.empty() || (!palette->getLocked() && formatSupportsSections)) {
                        UILabel* rowNameLabel = new UILabel(rowName.empty() ? "<no name>" : rowName);
                        rowNameLabel->fontsize = 14;
                        rowNameLabel->color = uint32ToSDLColor(0xB0FFFFFF);
                        if (editable && formatSupportsSections) {
                            UIButton* deleteRow = new UIButton("Delete");
                            deleteRow->fontSize = 12;
                            deleteRow->wxWidth = 80;
                            deleteRow->wxHeight = 20;
                            deleteRow->onClickCallback = [this, palette, rowIndex, filterQuery](...) {
                                if (palette->getColorList().size() == 1) {
                                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Can't delete last section."));
                                }
                                else {
                                    g_addPopup(PopupYesNo::ConfirmPrompt("Delete section?", "This section will be deleted.",
                                        [this, palette, rowIndex, filterQuery]() {
                                            auto& list = palette->getColorList();
                                            list.erase(list.begin() + rowIndex);
                                            palette->save();
                                            g_reloadColorMap();
                                        }));
                                }
                                };

                            UIButton* renameRow = new UIButton("Rename");
                            renameRow->fontSize = 12;
                            renameRow->wxWidth = 80;
                            renameRow->wxHeight = 20;
                            renameRow->onClickCallback = [this, palette, rowIndex, rowName, filterQuery](...) {
                                PopupTextBox* textInput = new PopupTextBox("Rename palette section", "Choose the new name for this section:", rowName);
                                textInput->allowEmptyText = true;
                                textInput->onTextInputConfirmedCallback = [this, palette, rowIndex, rowName, filterQuery](PopupTextBox*, std::string s) {
                                    palette->getColorList()[rowIndex].name = s;
                                    palette->save();
                                    reloadPaletteList();
                                };
                                g_addPopup(textInput);
                            };

                            paletteRows.push_back(UIStackPanel::Horizontal(5, { rowNameLabel, Panel::Space(20,1), deleteRow, renameRow }));
                        }
                        else {
                            paletteRows.push_back(rowNameLabel);
                        }
                    }


                    const int maxInLine = 34;

                    //generate color buttons
                    std::vector<Drawable*> colorButtons;
                    int colorIndex = 0;
                    for (NamedColor& nc : rowColors) {
                        colorButtons.push_back(genColorButton(palette, nc, rowIndex, colorIndex++, editable, formatSupportsColorRenames));
                        if (colorButtons.size() >= maxInLine) {
                            paletteRows.push_back(UIStackPanel::Horizontal(0, colorButtons));
                            colorButtons.clear();
                        }
                    }
                    if (editable) {
                        UIButton* addButton = new UIButton();
                        addButton->wxWidth = 20;
                        addButton->wxHeight = 20;
                        addButton->text = "+";
                        addButton->onClickCallback = [this, palette, rowIndex](...) {
                            promptAddColorToPalette(palette, rowIndex);
                        };
                        colorButtons.push_back(addButton);
                    }
                    paletteRows.push_back(UIStackPanel::Horizontal(0, colorButtons));
                    rowIndex++;
                }
                if (editable && formatSupportsSections) {
                    UIButton* addSection = new UIButton("New section...");
                    addSection->fontSize = 14;
                    addSection->wxWidth = 120;
                    addSection->wxHeight = 22;
                    addSection->onClickCallback = [palette, this, filterQuery](...) {
                        palette->getColorList().push_back(PaletteRow{ "",{} });
                        palette->save();
                        reloadPaletteList();
                        };
                    paletteRows.push_back(addSection);
                }
            }

            UILabel* idLabel = NULL;
            if (formatIsImported) {
                idLabel = new UILabel(palette->getID(), {}, 13);
                idLabel->color = uint32ToSDLColor(0x60FFFFFF);
            }

            stack.push_back(UIStackPanel::Vertical(0, {
                UIStackPanel::Horizontal(10, { new UILabel(frmt("{}{}", name, (hidden ? " (hidden)" : ""))), idLabel}),
                descLabel,
                paletteActionsRow.empty() ? NULL : UIStackPanel::Horizontal(10, joinVectors({{Panel::Space(5,1)}, paletteActionsRow})),
                Panel::Space(1,5),
                UIStackPanel::Horizontal(0, {
                    Panel::Space(30, 0), UIStackPanel::Vertical(0, paletteRows)
                })
            }));
        }
    }

    UIButton* newPaletteButton = new UIButton(TL("vsp.maineditor.panel.colorpicker.tab.palettes.newpalette"));
    newPaletteButton->position = XY{ 5, 60 };
    newPaletteButton->wxWidth = 140;
    newPaletteButton->onClickCallback = [this, filterQuery](UIButton*) {
        g_promptCreateNewPalette({}, []() { });
    };
    stack.push_back(newPaletteButton);

    mainPanel->subWidgets.addDrawable(UIStackPanel::Vertical(30, stack));
}

PopupPaletteManager::PopupPaletteManager()
{
    wxWidth = 750;
    wxHeight = 500;
    makeTitleAndDesc("Manage palettes", "");

    mainPanel = new ScrollingPanel();
    mainPanel->wxWidth = wxWidth - 10;
    mainPanel->wxHeight = wxHeight - 120;
    reloadPaletteList();

    UITextField* searchField = new UITextField();
    searchField->wxWidth = 200;
    searchField->onTextChangedCallback = [this](UITextField*, std::string s) {
        lastFilterQuery = s;
        reloadPaletteList();
    };

    UIStackPanel* searchAndListStack =
        UIStackPanel::Vertical(5, {
            UIStackPanel::Horizontal(20, {
                new UILabel(TL("vsp.cmn.search")),
                searchField
            }),
            mainPanel
        });
    searchAndListStack->position = { 5,40 };
    wxsManager.addDrawable(searchAndListStack);

    actionButton(TL("vsp.cmn.close"))->onClickCallback = [this](...) { closePopup(); };
}

void PopupPaletteManager::promptAddColorToPalette(IPalette* p, int rowIndex, u32 defaultColor)
{
    PopupPickColor* popup = new PopupPickColor("Add color", "");
    popup->colorPicker->canEditPalettes = false;
    popup->colorPicker->reloadColorLists();
    popup->setRGB(defaultColor);
    popup->onColorConfirmedCallback = [rowIndex, p](PopupPickColor*, u32 color) {
        p->addColor("", color, rowIndex);
        if (p->save()) {
            g_reloadColorMap();
        }
    };
    g_addPopup(popup);
}

bool PopupPaletteManager::paletteIsHidden(IPalette* p)
{
    std::string id = p->getID();
    for (auto& p : g_config.hiddenPalettes) {
        if (p == id) { return true; }
    }
    return false;
}

void PopupPaletteManager::togglePaletteHidden(IPalette* p)
{
    std::string id = p->getID();
    if (paletteIsHidden(p)) {
        g_config.hiddenPalettes.erase(std::find(g_config.hiddenPalettes.begin(), g_config.hiddenPalettes.end(), id));
    }
    else {
        g_config.hiddenPalettes.push_back(id);
    }
    g_saveConfig();
}
