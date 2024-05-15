#include "PopupSetEditorPixelGrid.h"
#include "FontRenderer.h"

PopupSetEditorPixelGrid::PopupSetEditorPixelGrid(MainEditor* parent, std::string tt, std::string tx) 
{

    wxHeight = 240;

    this->caller = parent;
    this->title = tt;
    this->text = tx;
    UIButton* nbutton = new UIButton();
    nbutton->text = "Set";
    nbutton->position = XY{ wxWidth - 260, wxHeight - 40 };
    nbutton->wxWidth = 120;
    nbutton->setCallbackListener(0, this);
    wxsManager.addDrawable(nbutton);

    UIButton* nbutton2 = new UIButton();
    nbutton2->text = "Cancel";
    nbutton2->position = XY{ wxWidth - 130, wxHeight - 40 };
    nbutton2->wxWidth = 120;
    nbutton2->setCallbackListener(1, this);
    wxsManager.addDrawable(nbutton2);

    tboxX = new UITextField();
    tboxX->position = XY{ 20, 80 };
    tboxX->text = std::to_string(caller->tileDimensions.x);
    tboxX->wxWidth = 120;
    wxsManager.addDrawable(tboxX);

    tboxY = new UITextField();
    tboxY->position = XY{ 160, 80 };
    tboxY->text = std::to_string(caller->tileDimensions.y);
    tboxY->wxWidth = 120;
    wxsManager.addDrawable(tboxY);

    opacitySlider = new UISlider();
    opacitySlider->position = XY{100, 140};
    opacitySlider->wxHeight = 40;
    opacitySlider->sliderPos = caller->tileGridAlpha / 255.0f;
    wxsManager.addDrawable(opacitySlider);
}

void PopupSetEditorPixelGrid::render()
{
	renderDefaultBackground();

	XY titlePos = getDefaultTitlePosition();
	XY contentPos = getDefaultContentPosition();

	g_fnt->RenderString(title, titlePos.x, titlePos.y);
	g_fnt->RenderString(text, contentPos.x, contentPos.y);
    XY opacityTextPos = xyAdd(getPopupOrigin(), XY{ 20, 140 });
    g_fnt->RenderString("Opacity", opacityTextPos.x, opacityTextPos.y);
	renderDrawables();
}
