#include "UICheckbox.h"
#include "UILabel.h"

UICheckbox::UICheckbox(std::string text)
{
    passThroughMouse = true;
    label = new UILabel(text);
    label->position = { 35,0 };
    subWidgets.addDrawable(label);
}

UICheckbox::UICheckbox(std::string text, bool* linkTo) : UICheckbox(text)
{
    checkbox = new UICheckboxButton(linkTo);
    checkbox->position = { 0,0 };
    checkbox->setCallbackListener(0, this);
    subWidgets.addDrawable(checkbox);
}

UICheckbox::UICheckbox(std::string text, bool defaultState) : UICheckbox(text)
{
    checkbox = new UICheckboxButton(defaultState);
    checkbox->position = { 0,0 };
    checkbox->setCallbackListener(0, this);
    subWidgets.addDrawable(checkbox);
}

XY UICheckbox::getDimensions() {
    XY checkboxDim = checkbox->getDimensions();
    XY labelDim = label->getDimensions();
    return {
        ixmax(checkbox->position.x + checkboxDim.x, label->position.x + labelDim.x),
        ixmax(checkbox->position.y + checkboxDim.y, label->position.y + labelDim.y)
    };
}

void UICheckboxButton::render(XY pos) {
    UIButton::render(pos);

    SDL_Rect r = { pos.x + position.x + 5, pos.y + position.y + 5, wxWidth - 10, wxHeight - 10 };
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);

    XY lineP1 = { r.x, r.y + r.h / 2 };
    XY lineP2 = { r.x + r.w / 3, r.y + r.h / 5 * 4 };
    XY lineP3 = { r.x + r.w, lineP2.y - ((r.x+r.w) - lineP2.x)};

    double timer = clickedFlag ? XM1PW3P1(stateChangeTimer.percentElapsedTime(400)) : 1.0;
    if (isChecked()) {
        drawLine(lineP2, lineP1, timer);
        drawLine(lineP2, lineP3, timer);
    }
    else {
        drawLine(lineP1, lineP2, 1.0-timer);
        drawLine(lineP3, lineP2, 1.0-timer);
    }
}

void UICheckboxButton::click()
{
    clickedFlag = true;
    setChecked(!isChecked());
    stateChangeTimer.start();
    UIButton::click();
}
