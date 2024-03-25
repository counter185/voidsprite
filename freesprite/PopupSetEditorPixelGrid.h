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

    UITextField* tboxX;
    UITextField* tboxY;
    UISlider* opacitySlider;

    MainEditor* caller;

    PopupSetEditorPixelGrid(MainEditor* parent, std::string tt, std::string tx);

    void render() override;

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

