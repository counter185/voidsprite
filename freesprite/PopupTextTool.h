#pragma once
#include "BasePopup.h"
#include "globals.h"

class PopupTextTool :
    public BasePopup
{
protected:
    std::vector<std::string> foundFonts;
public:
    
    std::string fontPath = "";
    UITextField* textbox;
    UITextField* textboxSize;
    int textSize;
    ToolText* caller;
    UIDropdown* fontsDropdown;

    PopupTextTool(ToolText* parent, std::string tt, std::string tx);
    void defaultInputAction(SDL_Event evt) override;

    std::vector<std::string> listAllSystemFonts();
};

