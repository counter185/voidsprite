#pragma once
#include "BasePopup.h"

class PopupSetEditorPixelGrid :
    public BasePopup
{
public:
    std::vector<XY> predefinedTileSizes = {
        {0,0},
        {8,8},
        {16,16},
        {24,32},
        {32,32},
        {48,48},
        {64,64}
    };

    UINumberInputField* tboxX;
    UINumberInputField* tboxY;
    UINumberInputField* tboxPadRX;
    UINumberInputField* tboxPadBY;
    UISlider* opacitySlider;

    MainEditor* caller;

    XY newTileDimensions{};
    XY newTilePaddingBottomRight{};
    bool newOverrideGridColor = false;
    u32 newTileGridColor = 0xFFFFFF;

    PopupSetEditorPixelGrid(MainEditor* parent, std::string tt, std::string tx);

    void finish();
};

