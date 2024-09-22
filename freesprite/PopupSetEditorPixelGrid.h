#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UITextField.h"
#include "UIButton.h"
#include "maineditor.h"

class PopupSetEditorPixelGrid :
    public BasePopup, public EventCallbackListener
{
public:
    std::string title = "";
    std::string text = "";
    std::vector<XY> predefinedTileSizes = {
        {8,8},
        {16,16},
        {32,32},
        {48,48},
        {64,64}
    };

    UITextField* tboxX;
    UITextField* tboxY;
    UISlider* opacitySlider;

    MainEditor* caller;

    PopupSetEditorPixelGrid(MainEditor* parent, std::string tt, std::string tx);

    void render() override;

    void eventDropdownItemSelected(int evt_id, int index, std::string name) {
    if (evt_id == 39) {   
        XY newTileSize = predefinedTileSizes[index];
       
        caller->tileDimensions = newTileSize;
        
        caller->tileGridAlpha = (uint8_t)(opacitySlider->sliderPos * 255);
       
        closePopup();
    }
}

    void eventButtonPressed(int evt_id) override {
        if (evt_id == 0) {
            if (!tboxX->text.empty() && !tboxY->text.empty()) {
                caller->tileDimensions = XY{ std::stoi(tboxX->text), std::stoi(tboxY->text) };
                caller->tileGridAlpha = (uint8_t)(opacitySlider->sliderPos * 255);
                closePopup();
            }
        }
        else {
            closePopup();
        }
        
    }
};

