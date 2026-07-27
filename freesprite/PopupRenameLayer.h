#pragma once
#include "PopupTextBox.h"
class PopupRenameLayer :
    public PopupTextBox
{
private:
    std::string originalName = "";
    UIDropdown* favsDropdown = NULL;

    void updateFavsList();
public:
    PopupRenameLayer(std::string defaultValue = "");
};

