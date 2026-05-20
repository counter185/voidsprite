#include <algorithm>
#include "TabbedView.h"
#include "FontRenderer.h"
#include "UIStackPanel.h"
#include "UIButton.h"

TabbedView::TabbedView(std::vector<Tab> tabN, int buttonWidth) {

    sizeToContent = true;
    passThroughMouse = true;
    //establish the right button width
    for (auto& tab : tabN) {
        int textWidth = g_fnt->StatStringDimensions(tab.name).x + 10 + (tab.icon != NULL ? buttonsHeight : 0);
		buttonWidth = ixmax(buttonWidth, textWidth);
    }

    for (int x = 0; x < tabN.size(); x++) {
        UIButton* nbutton = new UIButton();
        nbutton->wxWidth = buttonWidth;
        nbutton->wxHeight = buttonsHeight;
        nbutton->text = tabN[x].name;
        nbutton->icon = tabN[x].icon;
        nbutton->onClickCallback = [this, x](...) { switchTab(x); };
        tabButtons.push_back(nbutton);

        Panel* subPanel = new Panel();
        subPanel->passThroughMouse = true;
        subPanel->sizeToContent = true;
        subPanel->position = { 0, buttonsHeight };
        subPanel->enabled = false;
        tabN[x].tabPanel = subPanel;
        subWidgets.addDrawable(subPanel);
        tabs.push_back(tabN[x]);
    }

    std::vector<Drawable*> downcast;
    std::transform(tabButtons.begin(), tabButtons.end(), std::back_inserter(downcast), [](auto* a) { return a; });

    subWidgets.addDrawable(UIStackPanel::Horizontal(0, downcast));

    updateTabButtonsAndState();
    tabSwitchTimer.start();
}
