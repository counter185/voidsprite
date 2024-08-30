#pragma once
#include "BasePopup.h"
#include "globals.h"
#include "EventCallbackListener.h"

class PopupTextTool :
    public BasePopup, EventCallbackListener
{
public:
	std::string title = "";
	std::string text = "";

	UITextField* textbox;
	UITextField* textboxSize;
	int textSize;
	ToolText* caller;

    PopupTextTool(ToolText* parent, std::string tt, std::string tx);

    void render() override;

    void eventButtonPressed(int evt_id) override;
};

