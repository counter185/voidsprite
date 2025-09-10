#pragma once
#include "globals.h"
#include "drawable.h"
#include "DrawableManager.h"
#include "UITextField.h"
#include "EventCallbackListener.h"

class UIColorInputField :
    public Drawable, public EventCallbackListener
{
public:
	int wxWidth = 250, wxHeight = 30;
	uint32_t pickedColor = 0xFF000000;
	DrawableManager wxsManager;
	UITextField* textField;

	UIColorInputField();
	~UIColorInputField() {
		wxsManager.freeAllDrawables();
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
		return wxsManager.mouseInAny(thisPositionOnScreen, mousePos);//pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight }) || 
	}
	void render(XY pos) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;

	void focusIn() override {
		Drawable::focusIn();
	}
	void focusOut() override {
		Drawable::focusOut();
		wxsManager.forceUnfocus();
	}

	void setPickedColor(uint32_t c) {
		textField->setText(frmt("{:06X}", c & 0xFFFFFF));
		c |= 0xFF000000;
		pickedColor = c;
	}

	void eventTextInput(int evt_id, std::string data) override
	{
		if (evt_id == EVENT_COLORPICKER_TEXTFIELD) {
			if (data.size() == 6) {
				unsigned int col;
				if (tryRgbStringToColor(data, &col)) {
					col |= 0xff000000;
					setPickedColor(col);
					//setMainEditorColorRGB(col);
				}
			}

		}
	}

	void eventTextInputConfirm(int evt_id, std::string data) override
	{
		if (evt_id == EVENT_COLORPICKER_TEXTFIELD) {
			if (data == "rand" || data == "random") {
				setPickedColor((0xFF << 24) + ((rand() % 256) << 16) + ((rand() % 256) << 8) + (rand() % 256));
				//setMainEditorColorRGB((0xFF << 24) + ((rand() % 256) << 16) + ((rand() % 256) << 8) + (rand() % 256));
			}
			else if (g_colors.contains(data)) {
				setPickedColor(g_colors[data]);
				//setMainEditorColorRGB(g_colors[data]);
			}
		}
	}
};

