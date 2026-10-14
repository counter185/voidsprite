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
#include "UIColorPicker.h"

void PopupPaletteManager::reloadPaletteList()
{
    std::string filterQuery = lastFilterQuery;
    mainPanel->subWidgets.freeAllDrawables();
    std::vector<Drawable*> stack;
    for (IPalette*& palette : g_loadedPalettes) {

        std::string name = palette->getName();
        if (filterQuery == "" || stringContainsIgnoreCase(name, filterQuery)) {

            bool formatSupportsSections = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_EXT);
            bool formatSupportsName = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_NAME);
            bool formatSupportsDescription = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_DESCRIPTION);
            bool formatSupportsColorRenames = palette->formatFeatureSupported(PALETTE_FORMAT_FEATURE_COLOR_NAMES);
            bool editable = palette->canSave();

            UILabel* descLabel = NULL;
            if (!palette->getDescription().empty()) {
                descLabel = new UILabel(palette->getDescription(), {}, 12);
                descLabel->color = uint32ToSDLColor(0xA0FFFFFF);
            }

            std::vector<Drawable*> titleRow = {
                new UILabel(name)
            };

            std::string url = palette->getExternalUrl();
            if (!url.empty()) {
                UIButton* openExternal = new UIButton("View online...");
                openExternal->fontSize = 14;
                openExternal->wxWidth = 100;
                openExternal->wxHeight = 22;
                openExternal->tooltip = url;
                openExternal->onClickCallback = [url](...) { platformOpenWebpageURL(url); };
                titleRow.push_back(openExternal);
            }

            //make palette rows
            std::vector<Drawable*> paletteRows;
            auto& rows = palette->getColorList();
            int rowIndex = 0;
            for (auto& [rowName, rowColors] : rows) {
                //don't show <no name> if format doesn't support this
                if (!rowName.empty() || formatSupportsSections) {
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
                                PopupYesNo* confirm = new PopupYesNo("Delete section?", "This section will be deleted.");
                                confirm->onFinishCallback = [this, palette, rowIndex, filterQuery](PopupYesNo*, bool yes) {
                                    if (yes) {
                                        auto& list = palette->getColorList();
                                        list.erase(list.begin() + rowIndex);
                                        palette->save();
                                        g_reloadColorMap();
                                    }
                                };
                                g_addPopup(confirm);
                            }
                        };

                        UIButton* renameRow = new UIButton("Rename");
                        renameRow->fontSize = 12;
                        renameRow->wxWidth = 80;
                        renameRow->wxHeight = 20;
                        renameRow->onClickCallback = [this, palette, rowIndex, rowName, filterQuery](...) {
                            PopupTextBox* textInput = new PopupTextBox("Rename palette section", "Choose new name for this section:", rowName);
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
                for (auto& [colName, colValue] : rowColors) {
                    UIButton* colorButton = new UIButton();
                    colorButton->wxWidth = 20;
                    colorButton->wxHeight = 20;
                    colorButton->fill = Fill::Solid(colValue);
                    colorButton->tooltip = colName;
                    colorButtons.push_back(colorButton);
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

            stack.push_back(UIStackPanel::Vertical(0, {
                UIStackPanel::Horizontal(20, titleRow),
                descLabel,
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
