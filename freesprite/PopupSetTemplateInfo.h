#pragma once
#include "BasePopup.h"
class PopupSetTemplateInfo :
    public BasePopup
{
protected:
    MainEditor* caller = NULL;
public:
    PopupSetTemplateInfo(MainEditor* caller);

};

