#include "PopupSetEditorPixelGrid.h"
#include "FontRenderer.h"
#include "UIDropdown.h"
#include "UISlider.h"

PopupSetEditorPixelGrid::PopupSetEditorPixelGrid(MainEditor* parent, std::string tt, std::string tx) 
{

    wxHeight = 280;
    std::vector<std::string> names;
    for (XY& tileSize : predefinedTileSizes) {  
        names.push_back(xyEqual(tileSize, { 0,0 }) ? std::string("No grid") : frmt("{}x{}", tileSize.x, tileSize.y));
    }

    this->caller = parent;
    UIButton* nbutton = actionButton(TL("vsp.cmn.apply"));
    nbutton->setCallbackListener(0, this);

    UIButton* nbutton2 = actionButton(TL("vsp.cmn.cancel"));
    nbutton2->setCallbackListener(1, this);

    tboxX = new UITextField();
    tboxX->position = XY{ 20, 80 };
    tboxX->setText(std::to_string(caller->tileDimensions.x));
    tboxX->wxWidth = 120;
    wxsManager.addDrawable(tboxX);

    tboxY = new UITextField();
    tboxY->position = XY{ 160, 80 };
    tboxY->setText(std::to_string(caller->tileDimensions.y));
    tboxY->wxWidth = 120;
    wxsManager.addDrawable(tboxY);

    UILabel* paddingLabel = new UILabel("Padding");
    paddingLabel->position = XY{ 20, 125 };
    wxsManager.addDrawable(paddingLabel);


    UILabel* ll = new UILabel("Right");
    ll->position = XY{ 120, 125 };
    wxsManager.addDrawable(ll);

    tboxPadRX = new UITextField();
    tboxPadRX->position = XY{ 175, 125 };
    tboxPadRX->setText(std::to_string(caller->tileGridPaddingBottomRight.x));
    tboxPadRX->wxWidth = 60;
    wxsManager.addDrawable(tboxPadRX);

    ll = new UILabel("Bottom");
    ll->position = XY{ 250, 125 };
    wxsManager.addDrawable(ll);

    tboxPadBY = new UITextField();
    tboxPadBY->position = XY{ 320, 125 };
    tboxPadBY->setText(std::to_string(caller->tileGridPaddingBottomRight.y));
    tboxPadBY->wxWidth = 60;
    wxsManager.addDrawable(tboxPadBY);

    UILabel* opacityLabel = new UILabel("Opacity");
    opacityLabel->position = XY{ 20, 185 };
    wxsManager.addDrawable(opacityLabel);

    opacitySlider = new UISlider();
    opacitySlider->position = XY{100, 180};
    opacitySlider->wxHeight = 40;
    opacitySlider->sliderPos = caller->tileGridAlpha / 255.0f;
    wxsManager.addDrawable(opacitySlider);

    UIDropdown* dropdown = new UIDropdown(names);
    dropdown->position = XY{ 300, 80 }; 
    dropdown->text = "Presets";
    dropdown->setCallbackListener(39, this);
    wxsManager.addDrawable(dropdown);

    makeTitleAndDesc(tt, tx);
}
