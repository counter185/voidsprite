#pragma once
#include "BasePopup.h"
class PopupPaletteManager :
    public BasePopup, public ColorListModifiedObserver
{
protected:
    ScrollingPanel* mainPanel;
    std::string lastFilterQuery = "";

    void reloadPaletteList();
public:
    PopupPaletteManager();

    void onColorListModified() override { reloadPaletteList(); }

    static void promptAddColorToPalette(IPalette* p, int rowIndex, u32 defaultColor = 0xFFFFFF);
};

