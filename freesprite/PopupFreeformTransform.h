#pragma once
#include "BasePopup.h"
#include "Canvas.h"

class PopupFreeformTransform : public BasePopup {
protected:
    MainEditor* caller;
    Layer* target;
    SDL_Rect targetPasteRect;

    int draggingCorner = -1;
    XY dragStart = { 0,0 };
public:
    PopupFreeformTransform(MainEditor* caller, Layer* target);
    ~PopupFreeformTransform();

    void render() override;
    void defaultInputAction(SDL_Event evt) override;
    SDL_Rect evalPasteRectChange();
    void renderDefaultBackground() override;;
    XY getPopupOrigin() override { return { 10,10 }; };

    std::pair<int, XY> getMouseOverDraggablePoint();
    SDL_Rect getRenderTargetRect();
    Canvas& getCallerCanvas();
    void paste();
};