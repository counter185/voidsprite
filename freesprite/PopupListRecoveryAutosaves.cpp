#include "PopupListRecoveryAutosaves.h"
#include "ScrollingPanel.h"
#include "UIButton.h"
#include "maineditor.h"
#include "Notification.h"

PopupListRecoveryAutosaves::PopupListRecoveryAutosaves()
{
	wxWidth = 700;
	makeTitleAndDesc(TL("vsp.launchpad.nav.recoveryautosaves"), TL("vsp.launchpad.recoveryautosaves.desc"));

	ScrollingPanel* scrollPanel = new ScrollingPanel();
	scrollPanel->position = XY{ 10, 110 };
	scrollPanel->wxWidth = wxWidth - 20;
	scrollPanel->wxHeight = 240;
	scrollPanel->scrollVertically = true;
	scrollPanel->scrollHorizontally = false;
	scrollPanel->bgColor = Fill::Solid(0x70101010);
	wxsManager.addDrawable(scrollPanel);

	auto autosaveFiles = platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/autosaves"), ".voidsn");
	int x = 0;
	for (auto& f : autosaveFiles) {
		UIButton* b = new UIButton();
		b->text = fileNameFromPath(convertStringToUTF8OnWin32(f));
		b->position = XY{ 0,x };
		x += 30;
		b->wxWidth = scrollPanel->wxWidth - 40;
		b->wxHeight = 30;
		b->onClickCallback = [this, f](UIButton*) {
			MainEditor* newSsn = loadAnyIntoSession(convertStringToUTF8OnWin32(f));
			if (newSsn != NULL) {
				g_addScreen(newSsn);
				closePopup();
			}
			else {
				g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.fileloadfail")));
			}
		};
		scrollPanel->subWidgets.addDrawable(b);
	}

	UIButton* closeBtn = actionButton(TL("vsp.cmn.close"));
    closeBtn->onClickCallback = [this](UIButton*) { this->closePopup(); };
}
