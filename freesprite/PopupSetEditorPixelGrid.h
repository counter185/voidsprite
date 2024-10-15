#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UITextField.h"
#include "UIButton.h"
#include "maineditor.h"
#include "Notification.h"

class PopupSetEditorPixelGrid :
    public BasePopup, public EventCallbackListener
{
public:
    std::string title = "";
    std::string text = "";
    std::vector<XY> predefinedTileSizes = {
        {0,0},
        {8,8},
        {16,16},
        {24,32},
        {32,32},
        {48,48},
        {64,64}
    };

    UITextField* tboxX;
    UITextField* tboxY;
    UITextField* tboxPadRX;
    UITextField* tboxPadBY;
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
            if (!tboxX->text.empty() && !tboxY->text.empty() && !tboxPadRX->text.empty() && !tboxPadBY->text.empty()) {
                caller->tileDimensions = XY{ std::stoi(tboxX->text), std::stoi(tboxY->text) };
                XY newTileGridPaddingBottomRight = XY{ std::stoi(tboxPadRX->text), std::stoi(tboxPadBY->text) };
                if (newTileGridPaddingBottomRight.x >= caller->tileDimensions.x && newTileGridPaddingBottomRight.x != 0) {
                    newTileGridPaddingBottomRight.x = 0;
                    g_addNotification(ErrorNotification("Invalid padding size", "Padding overflows tile size"));
                }
                if (newTileGridPaddingBottomRight.y >= caller->tileDimensions.y && newTileGridPaddingBottomRight.y != 0) {
                    newTileGridPaddingBottomRight.y = 0;
                    g_addNotification(ErrorNotification("Invalid padding size", "Padding overflows tile size"));
                }
                caller->tileGridPaddingBottomRight = newTileGridPaddingBottomRight;
                caller->tileGridAlpha = (uint8_t)(opacitySlider->sliderPos * 255);
                closePopup();
            }
        }
        else {
            closePopup();
        }
        
    }
    void eventTextInputConfirm(int evt_id, std::string data) override {
        if (evt_id == 1) {
            eventButtonPressed(0);
        }
    }
};

