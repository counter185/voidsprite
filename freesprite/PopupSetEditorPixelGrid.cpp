#include "PopupSetEditorPixelGrid.h"
#include "UIDropdown.h"
#include "UISlider.h"
#include "UILabel.h"
#include "UIButton.h"
#include "UIStackPanel.h"
#include "UICheckbox.h"
#include "UIColorInputField.h"
#include "maineditor.h"

PopupSetEditorPixelGrid::PopupSetEditorPixelGrid(MainEditor* parent, std::string tt, std::string tx) 
{
    this->caller = parent;
    wxHeight = 360;

    newTileDimensions = caller->ssne.tileDimensions;
    newTilePaddingBottomRight = caller->ssne.tileGridPaddingBottomRight;
    newOverrideGridColor = caller->ssne.overrideTileGridColor;
    newTileGridColor = caller->ssne.tileGridColor;

    std::vector<std::string> names;
    for (XY& tileSize : predefinedTileSizes) {  
        names.push_back(xyEqual(tileSize, { 0,0 }) ? std::string("No grid") : frmt("{}x{}", tileSize.x, tileSize.y));
    }

    UIButton* nbutton = actionButton(TL("vsp.cmn.apply"));
    nbutton->onClickCallback = [this](UIButton* btn) { finish(); };

    UIButton* nbutton2 = actionButton(TL("vsp.cmn.cancel"));
    nbutton2->onClickCallback = [this](UIButton* btn) { g_closePopup(this); };


    tboxX = new UINumberInputField(&newTileDimensions.x);
    tboxY = new UINumberInputField(&newTileDimensions.y);

    tboxX->wxWidth = tboxY->wxWidth = 80;


    UIDropdown* dropdown = new UIDropdown(names);
    dropdown->text = "Presets";
    dropdown->wxWidth = 140;
    dropdown->onDropdownItemSelectedCallback = [this](UIDropdown* dd, int index, std::string name) {
        newTileDimensions = predefinedTileSizes[index];
        newTilePaddingBottomRight = { 0,0 };
        finish();
    };

    UIStackPanel* xyStackPanel = UIStackPanel::Horizontal(20, {
        tboxX,
        tboxY,
        dropdown
    });
    xyStackPanel->position = XY{ 20, 80 };
    wxsManager.addDrawable(xyStackPanel);


    tboxPadRX = new UINumberInputField(&newTilePaddingBottomRight.x);
    tboxPadBY = new UINumberInputField(&newTilePaddingBottomRight.y);

    tboxPadBY->validateFunction = tboxPadRX->validateFunction = [this](int val) {
        return val == 0 || val < newTileDimensions.x;
    };
    tboxPadBY->wxWidth = tboxPadRX->wxWidth = 60;

    UIStackPanel* paddingSectionStack = UIStackPanel::Horizontal(5, {
        new UILabel("Padding"),
        Panel::Space(40,1),
        new UILabel("Right"),
        tboxPadRX,
        Panel::Space(20,1),
        new UILabel("Bottom"),
        tboxPadBY
    });
    paddingSectionStack->position = XY{ 20, 125 };
    wxsManager.addDrawable(paddingSectionStack);

    tboxX->onTextChangedConfirmCallback = tboxY->onTextChangedConfirmCallback =
        tboxPadRX->onTextChangedConfirmCallback = tboxPadBY->onTextChangedConfirmCallback =
        [this](UITextField* tf, std::string n) { finish(); };


    UILabel* opacityLabel = new UILabel("Opacity");
    opacityLabel->position = XY{ 20, 180 };
    wxsManager.addDrawable(opacityLabel);

    opacitySlider = new UISlider();
    opacitySlider->position = XY{100, 180};
    opacitySlider->wxHeight = 30;
    opacitySlider->sliderPos = caller->ssne.tileGridAlpha / 255.0f;
    wxsManager.addDrawable(opacitySlider);


    UIColorInputField* gridColorInputField = new UIColorInputField();
    gridColorInputField->setColor(newTileGridColor);
    gridColorInputField->onColorChangedCallback = [this](UIColorInputField* field, u32 color) {
        newTileGridColor = color;
    };

    UIStackPanel* gridColorStack = UIStackPanel::Vertical(4, {
        new UICheckbox("Override grid color", &newOverrideGridColor),
        UIStackPanel::Horizontal(2, {
            Panel::Space(30,1),
            new UILabel("Grid color"),
            gridColorInputField
        })
    });
    gridColorStack->position = { 20, 230 };
    wxsManager.addDrawable(gridColorStack);

    makeTitleAndDesc(tt, tx);
}

void PopupSetEditorPixelGrid::finish() {
    caller->ssne.tileDimensions = newTileDimensions;
    caller->ssne.tileGridPaddingBottomRight = newTilePaddingBottomRight;
    caller->ssne.tileGridAlpha = (uint8_t)(opacitySlider->getValue(0, 255));
    caller->ssne.overrideTileGridColor = newOverrideGridColor;
    caller->ssne.tileGridColor = newTileGridColor;
    closePopup();
}
