#pragma once
#include "BasePopup.h"
class PopupChooseExtsToAssoc :
    public BasePopup
{
private:
    std::map<std::string, bool> filetypes;

public:
    PopupChooseExtsToAssoc();
};

