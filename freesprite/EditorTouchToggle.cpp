#include "EditorTouchToggle.h"
#include "UILabel.h"
#include "UIButton.h"
#include "maineditor.h"
#include "Notification.h"

EditorTouchToggle::EditorTouchToggle(MainEditor* caller) : parent(caller)
{
    wxWidth = 180;
    wxHeight = 150;

	UILabel* lbl = new UILabel(TL("vsp.maineditor.panel.touchmode.title"));
	lbl->position = { 5, 2 };
	subWidgets.addDrawable(lbl);

	UIButton* closeButton = new UIButton("X");
	closeButton->position = { wxWidth - 25, 2 };
	closeButton->wxWidth = 20;
	closeButton->wxHeight = 20;
	closeButton->onClickCallback = [this](UIButton* btn) {
		parent->removeWidget(this);
	};
	subWidgets.addDrawable(closeButton);

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
	subWidgets.addDrawable(btn);
}

void EditorTouchToggle::render(XY position)
{
    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };

    u32 colorBG1 = PackRGBAtoARGB(0x30, 0x30, 0x30, focused ? 0xa0 : 0x90);
    u32 colorBG2 = PackRGBAtoARGB(0x10, 0x10, 0x10, focused ? 0xa0 : 0x90);
    renderGradient(r, colorBG2, colorBG1, colorBG1, colorBG1);
    if (thisOrParentFocused()) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
    }

	DraggablePanel::render(position);
}
