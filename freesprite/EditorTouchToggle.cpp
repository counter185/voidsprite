#include "EditorTouchToggle.h"
#include "UILabel.h"
#include "UIButton.h"
#include "maineditor.h"
#include "Notification.h"

EditorTouchToggle::EditorTouchToggle(MainEditor* caller) : parent(caller)
{
    wxWidth = 180;
    wxHeight = 150;

	setupDraggable();
	setupCollapsible();
	addTitleText(TL("vsp.maineditor.panel.touchmode.title"));
	setupCloseButton([this]() { parent->removeWidget(this);});

	static std::map<int, std::string> modeNameKeys = {
			{0, "vsp.maineditor.panel.touchmode.pan"},
			{1, "vsp.maineditor.panel.touchmode.leftclick"},
			{2, "vsp.maineditor.panel.touchmode.rightclick"}
	};
    UIButton* btn = new UIButton("Toggle");
	btn->position = { 5, 30 };
    btn->wxWidth = wxWidth - 10;
    btn->wxHeight = wxHeight - 5 - 35;
	btn->onClickCallback = [this](UIButton* btn) {
		(*((int*)&(parent->touchMode)))++;
		(*((int*)&(parent->touchMode))) %= (int)TOUCHMODE_MAX;
		btn->text = TL(modeNameKeys[(int)parent->touchMode]);
		//g_addNotification(Notification("Touch mode changed", frmt("New touch mode: {}", (int)parent->touchMode), 1000));
	};
	
	btn->text = TL(modeNameKeys[(int)parent->touchMode]);
	wxsTarget().addDrawable(btn);
}
