#pragma once
#include "BasePopup.h"
#include "ParameterStore.h"
class PopupSetParameters :
    public BasePopup
{
public:
    PopupSetParameters(std::string title, ParameterStore*, ParamList* params = NULL, std::string locKeyPrefix = "");

    void finish(bool confirmed);

    std::function<void()> onFinishCallback = NULL;
};

