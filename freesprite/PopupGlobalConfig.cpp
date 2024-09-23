#include "PopupGlobalConfig.h"
#include "UIButton.h"
#include "TabbedView.h"
#include "FontRenderer.h"
#include "UICheckbox.h"
#include "Notification.h"

enum ConfigOptions : int {
	CHECKBOX_OPEN_SAVED_PATH = 1
};

PopupGlobalConfig::PopupGlobalConfig()
{
	previousConfig = g_config;

	wxHeight = 400;
	wxWidth = 600;

	TabbedView* configTabs = new TabbedView({ {"Editor"}, {"Misc."}}, 90);
	configTabs->position = XY{ 10,50 };
	wxsManager.addDrawable(configTabs);

	XY posInTab = { 0,10 };
	UICheckbox* cb1 = new UICheckbox("Open saved file location", g_config.openSavedPath);
	cb1->position = posInTab;
	cb1->setCallbackListener(CHECKBOX_OPEN_SAVED_PATH, this);
	configTabs->tabs[0].wxs.addDrawable(cb1);
	posInTab.y += 40;

	UIButton* closeButton = new UIButton();
	closeButton->text = "Close";
	closeButton->position = XY{ wxWidth - 140, wxHeight - 40 };
	closeButton->wxHeight = 30;
	closeButton->wxWidth = 130;
	closeButton->setCallbackListener(0, this);
	wxsManager.addDrawable(closeButton);

	UIButton* saveAndCloseButton = new UIButton();
	saveAndCloseButton->text = "Save and close";
	saveAndCloseButton->position = XY{ wxWidth - 280, wxHeight - 40 };
	saveAndCloseButton->wxHeight = 30;
	saveAndCloseButton->wxWidth = 130;
	saveAndCloseButton->setCallbackListener(1, this);
	wxsManager.addDrawable(saveAndCloseButton);
}

void PopupGlobalConfig::render()
{
	renderDefaultBackground();

	XY titlePos = getDefaultTitlePosition();
	g_fnt->RenderString("Preferences", titlePos.x, titlePos.y);

	renderDrawables();
}

void PopupGlobalConfig::eventButtonPressed(int evt_id)
{
	if (evt_id == 0) {
		// close and discard
		g_config = previousConfig;
		closePopup();
	}
	else if (evt_id == 1) {
		//save and close
		if (g_saveConfig()) {
			g_addNotification(Notification("Success", "Preferences saved", 1500, NULL, COLOR_INFO));
			closePopup();
		}
		else {
			g_addNotification(ErrorNotification("Error", "Failed to save preferences"));
		}
	}
}

void PopupGlobalConfig::eventCheckboxToggled(int evt_id, bool checked)
{
	switch (evt_id) {
		case CHECKBOX_OPEN_SAVED_PATH:
			g_config.openSavedPath = checked;
			break;
	}
}
