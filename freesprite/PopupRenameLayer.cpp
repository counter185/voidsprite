#include "PopupRenameLayer.h"
#include "UIDropdown.h"
#include "UIButton.h"
#include "UIStackPanel.h"

void PopupRenameLayer::updateFavsList()
{
	std::vector<std::string> favNames = 
		joinVectors({
		g_config.favLayerNames,
		{
			"Background",
			"Foreground",
			"Character"
		}});
	favsDropdown->items = favNames;
}

PopupRenameLayer::PopupRenameLayer(std::string defaultValue)
	: PopupTextBox("Rename layer", "Rename " + defaultValue, defaultValue, 320)
{
	favsDropdown = new UIDropdown(std::vector<std::string>{});
	favsDropdown->text = "Favourites...";
	favsDropdown->position = xyAdd(tbox->position, { 20, 40 });
	favsDropdown->onDropdownItemSelectedCallback = [this](UIDropdown*, int, std::string name) {
		tbox->setText(name);
		accept();
	};
	updateFavsList();
	wxsManager.addDrawable(favsDropdown);

	UIButton* addButton = new UIButton();
	addButton->icon = g_iconLayerAdd;
	addButton->tooltip = "Add name to favourites";
	addButton->wxWidth = addButton->wxHeight = 30;
	addButton->onClickCallback = 
		[this](...) { 
			std::string textNow = tbox->getText();
			if (std::find(g_config.favLayerNames.begin(), g_config.favLayerNames.end(), textNow) == g_config.favLayerNames.end()) {
				g_config.favLayerNames.push_back(textNow);
				g_saveConfig();
				updateFavsList();	
			}
		};

	UIButton* removeBtn = new UIButton();
	removeBtn->icon = g_iconLayerDelete;
	removeBtn->tooltip = "Remove name from favourites";
	removeBtn->wxWidth = removeBtn->wxHeight = 30;
	removeBtn->onClickCallback = 
		[this](...) { 
			std::string textNow = tbox->getText();
			auto find = std::find(g_config.favLayerNames.begin(), g_config.favLayerNames.end(), textNow);
			if (find != g_config.favLayerNames.end()) {
				g_config.favLayerNames.erase(find);
				g_saveConfig();
				updateFavsList();
			}
		};
	wxsManager.addDrawable(UIStackPanel::Horizontal(10, { addButton, removeBtn }, xyAdd(tbox->position, { tbox->wxWidth + 20, 0 })));
}
