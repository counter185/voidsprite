#pragma once
#include "Panel.h"
#include "UIButton.h"

enum Anchor {
    ANCHOR_TOPLEFT = 0,
    ANCHOR_TOPCENTER = 1,
    ANCHOR_TOPRIGHT = 2,
    ANCHOR_MIDLEFT = 3,
    ANCHOR_MIDCENTER = 4,
    ANCHOR_MIDRIGHT = 5,
    ANCHOR_BOTTOMLEFT = 6,
    ANCHOR_BOTTOMCENTER = 7,
    ANCHOR_BOTTOMRIGHT = 8
};

class UIAnchorButton : public UIButton {
public:
    Anchor thisAnchor = ANCHOR_TOPLEFT;

    void render(XY at) override;
};

class UIAnchorSelect : public Panel
{
protected:
    UIAnchorButton* anchorButtons[9];

    void chooseAnchor(Anchor anch);
    void updateButtons();
public:
    Anchor selectedAnchor = ANCHOR_TOPLEFT;
    UIAnchorSelect();
};

