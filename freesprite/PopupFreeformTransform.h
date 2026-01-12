#pragma once
#include "BasePopup.h"
#include "Canvas.h"

class PopupFreeformTransform : public BasePopup {
protected:
    MainEditor* caller;
    SDL_Rect targetPasteRect;
    
    bool pasteWholeSession = false;
    Layer* target = NULL;

    int draggingCorner = -1;
    XY dragStart = { 0,0 };
public:
    std::function<void(bool)> onFinishCallback = NULL;

    PopupFreeformTransform(MainEditor* caller, Layer* target);
    ~PopupFreeformTransform();

    void render() override;
    void defaultInputAction(SDL_Event evt) override;
    SDL_Rect evalPasteRectChange();
    void renderDefaultBackground() override;
    XY getPopupOrigin() override { return { 10,10 }; };

    std::pair<int, XY> getMouseOverDraggablePoint();
    SDL_Rect getRenderTargetRect();
    Canvas& getCallerCanvas();
    void paste();
};