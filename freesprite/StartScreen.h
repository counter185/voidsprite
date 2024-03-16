#pragma once
#include "globals.h"
#include "EventCallbackListener.h"
#include "drawable.h"
#include "DrawableManager.h"
#include "BaseScreen.h"
#include "UITextField.h"
#include "UIButton.h"
#include "maineditor.h"

class StartScreen : public BaseScreen, public EventCallbackListener
{
public:
	DrawableManager wxsManager;

	int newImgW = 0, newImgH = 0;

	StartScreen() {
		UITextField* textFieldW = new UITextField();
		textFieldW->setCallbackListener(2, this);
		textFieldW->position = XY{80,120};
		textFieldW->wxWidth = 200;
		textFieldW->numeric = true;
		wxsManager.addDrawable(textFieldW);

		UITextField* textFieldH = new UITextField();
		textFieldH->setCallbackListener(3, this);
		textFieldH->position = XY{80,155};
		textFieldH->wxWidth = 200;
		textFieldH->numeric = true;
		wxsManager.addDrawable(textFieldH);

		UIButton* buttonNewImage = new UIButton();
		buttonNewImage->setCallbackListener(4, this);
		buttonNewImage->position = XY{ 40,200 };
		buttonNewImage->wxWidth = 100;
		buttonNewImage->text = "Create...";
		wxsManager.addDrawable(buttonNewImage);
	}

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;

	void eventTextInput(int evt_id, std::string data) override {
		switch (evt_id) {
			case 2:
				newImgW = std::stoi(data);
				break;
			case 3:
				newImgH = std::stoi(data);
				break;
		}
	}
	void eventButtonPressed(int evt_id) override {
		if (evt_id == 4) {
			g_addScreen(new MainEditor(XY{ newImgW, newImgH }));
		}
	}
};

