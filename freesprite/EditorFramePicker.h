#pragma once
#include "PanelUserInteractable.h"
class EditorFramePicker :
    public PanelUserInteractable
{
protected:
    MainEditor* parent = NULL;
    ScrollingPanel* frameButtonPanel = NULL;
    UIStackPanel* frameButtonStack = NULL;
    std::vector<UIButton*> frameButtons;

    Fill fillPlayButtonPaused = visualConfigFill("maineditor/framepicker/playpause_button/paused");
    Fill fillPlayButtonPlaying = visualConfigFill("maineditor/framepicker/playpause_button/playing");

    Fill fillActiveFrame = visualConfigFill("maineditor/framepicker/active_frame_fill");
    Fill fillTracedFrame = visualConfigFill("maineditor/framepicker/traced_frame_fill");

public:
    UINumberInputField* msPerFrameInput = NULL;
    UIButton* playpauseBtn = NULL;

    EditorFramePicker(MainEditor* caller);

    void createFrameButtons();

    void flashFrame(int index);
};

