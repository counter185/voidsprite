#include "TabbedView.h"
#include "FontRenderer.h"

TabbedView::TabbedView(std::vector<Tab> tabN, int buttonWidth) {

    //establish the right button width
    for (auto& tab : tabN) {
        int textWidth = g_fnt->StatStringDimensions(tab.name).x + 10 + (tab.icon != NULL ? buttonsHeight : 0);
		buttonWidth = ixmax(buttonWidth, textWidth);
    }

    int buttonX = 0;
    for (int x = 0; x < tabN.size(); x++) {
        UIButton* nbutton = new UIButton();
        nbutton->wxWidth = buttonWidth;
        nbutton->wxHeight = buttonsHeight;
        nbutton->position = XY{ buttonX, 0 };
        nbutton->text = tabN[x].name;
        nbutton->icon = tabN[x].icon;
        buttonX += nbutton->wxWidth;
        nbutton->setCallbackListener(x, this);
        tabButtons.addDrawable(nbutton);

        tabs.push_back(tabN[x]);
    }
    updateTabButtons();
    tabSwitchTimer.start();
}
