#include "PopupNewImage.h"
#include "StartScreen.h"
#include "UIButton.h"

PopupNewImage::PopupNewImage()
{
	wxWidth = 500;
	wxHeight = 430;

	PanelNewImage* newImg = new PanelNewImage();
	newImg->position = XY{ 10, 4 };
	newImg->imageCreatedCallback = [this]() { closePopup(); };
	wxsManager.addDrawable(newImg);
	wxWidth = 20 + newImg->getDimensions().x;

	actionButton(TL("vsp.cmn.close"))->onClickCallback = [this](...) { closePopup(); };
}
