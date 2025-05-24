#pragma once
#include "BasePopup.h"
#include "UIButton.h"

class PopupMessageBox :
    public BasePopup
{
public:
    PopupMessageBox(std::string tt, std::string tx, XY size = {600, 200}) {

        setSize(size);
		actionButton(TL("vsp.cmn.ok"))->onClickCallback = [this](UIButton* btn) {
			closePopup();
		};

        makeTitleAndDesc(tt, tx);
    }
};

