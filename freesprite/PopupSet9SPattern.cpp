#include "PopupSet9SPattern.h"
#include "Brush9SegmentRect.h"
#include "FontRenderer.h"

#include "ScrollingPanel.h"
#include "UIButton.h"

PopupSet9SPattern::PopupSet9SPattern()
{
    wxWidth = 600;
    wxHeight = 350;

    ScrollingPanel* buttonsPanel = new ScrollingPanel();
    buttonsPanel->position = XY{ 20, 60 };
    buttonsPanel->wxWidth = wxWidth - 40;
    buttonsPanel->wxHeight = wxHeight - 120;
    buttonsPanel->scrollHorizontally = false;
    buttonsPanel->scrollVertically = true;
    wxsManager.addDrawable(buttonsPanel);

    int i = 0;
    int yNow = 0;
    for (NineSegmentPattern*& p : g_9spatterns) {
        UIButton* nbutton = new UIButton();
        nbutton->wxHeight = ixmax(30, p->dimensions.y + 2);
        if (nbutton->wxHeight < 40) {
            nbutton->wxHeight *= 2;
        }
        nbutton->wxWidth = 360;
        nbutton->text = "";
        nbutton->icon = p->cachedTexture;
        nbutton->position = XY{ 0, yNow };
        yNow += nbutton->wxHeight;
        nbutton->setCallbackListener(i++, this);
        buttonsPanel->subWidgets.addDrawable(nbutton);
    }

    UIButton* nbutton = new UIButton();
    nbutton->text = "Back";
    nbutton->position = XY{ wxWidth - 130, wxHeight - 40 };
    nbutton->wxHeight = 35;
    nbutton->wxWidth = 120;
    nbutton->setCallbackListener(-1, this);
    wxsManager.addDrawable(nbutton);

    makeTitleAndDesc("Set 9-segment pattern");
}

void PopupSet9SPattern::eventButtonPressed(int evt_id)
{
    if (evt_id == -1) {
        closePopup();
    }
    else {
        if (callback != NULL) {
            callback->eventGeneric(callback_id, evt_id, 0);
        }
        closePopup();
    }
}
