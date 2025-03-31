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

    void eventDropdownItemSelected(int evt_id, int index, std::string name) {
    if (evt_id == 39) {   
        XY newTileSize = predefinedTileSizes[index];
       
        caller->tileDimensions = newTileSize;
        
        caller->tileGridAlpha = (uint8_t)(opacitySlider->getValue(0, 255));
       
        closePopup();
    }
}

    void eventButtonPressed(int evt_id) override {
        if (evt_id == 0) {
            std::string tboxXtx = tboxX->getText();
            std::string tboxYtx = tboxY->getText();
            std::string tboxPadRXtx = tboxPadRX->getText();
            std::string tboxPadBYtx = tboxPadBY->getText();
            if (!tboxXtx.empty() && !tboxYtx.empty() && !tboxPadRXtx.empty() && !tboxPadBYtx.empty()) {
                caller->tileDimensions = XY{ std::stoi(tboxX->getText()), std::stoi(tboxY->getText()) };
                XY newTileGridPaddingBottomRight = XY{ std::stoi(tboxPadRXtx), std::stoi(tboxPadBYtx) };
                if (newTileGridPaddingBottomRight.x >= caller->tileDimensions.x && newTileGridPaddingBottomRight.x != 0) {
                    newTileGridPaddingBottomRight.x = 0;
                    g_addNotification(ErrorNotification("Invalid padding size", "Padding overflows tile size"));
                }
                if (newTileGridPaddingBottomRight.y >= caller->tileDimensions.y && newTileGridPaddingBottomRight.y != 0) {
                    newTileGridPaddingBottomRight.y = 0;
                    g_addNotification(ErrorNotification("Invalid padding size", "Padding overflows tile size"));
                }
                caller->tileGridPaddingBottomRight = newTileGridPaddingBottomRight;
                caller->tileGridAlpha = (uint8_t)(opacitySlider->getValue(0,255));
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

