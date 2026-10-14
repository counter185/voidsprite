#pragma once
#include "BasePopup.h"
class PopupPaletteManager :
    public BasePopup
{
protected:
    ScrollingPanel* mainPanel;

    void reloadPaletteList(std::string filterQuery = "");
public:
    PopupPaletteManager();
};

