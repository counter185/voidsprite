#pragma once
#include "BasePopup.h"
class PopupPaletteManager :
    public BasePopup, public ColorListModifiedObserver
{
protected:
    ScrollingPanel* mainPanel;
    std::string lastFilterQuery = "";

    UIButton* genColorButton(IPalette* p, NamedColor& c, int rIndex, int cIndex, bool editable, bool canRename);

    void reloadPaletteList();
public:
    PopupPaletteManager();

    void onColorListModified() override { reloadPaletteList(); }

    static void promptAddColorToPalette(IPalette* p, int rowIndex, u32 defaultColor = 0xFFFFFF);
    static bool paletteIsHidden(IPalette* p);
    static void togglePaletteHidden(IPalette* p);
};

