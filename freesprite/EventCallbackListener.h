#pragma once
#include "globals.h"

#define EVENT_COLORPICKER_TEXTFIELD 1
#define EVENT_MAINEDITOR_SAVEFILE 2
#define EVENT_BRUSHPICKER_BRUSH_CHANGED 3
#define EVENT_COLORPICKER_TOGGLEERASER 4
#define EVENT_COLORPICKER_SLIDERH 5
#define EVENT_COLORPICKER_SLIDERS 6
#define EVENT_COLORPICKER_SLIDERV 7
#define EVENT_MAINEDITOR_CONFIRM_CLOSE 8
#define EVENT_SPRITEPREVIEW_SET_SPRITE_TICK 9
#define EVENT_MAINEDITOR_SET_CURRENT_LAYER_NAME 10
#define EVENT_MAINEDITOR_ADD_COMMENT 11
#define EVENT_BRUSHPICKER_TOGGLE_PATTERN_MENU 12
#define EVENT_MAINEDITOR_TOGGLEREPLACE 13
#define EVENT_COLORPICKER_SLIDERR 14
#define EVENT_COLORPICKER_SLIDERG 15
#define EVENT_COLORPICKER_SLIDERB 16
#define EVENT_OTHERFILE_SAVEFILE 17
#define EVENT_OTHERFILE_OPENFILE 18
#define EVENT_LAYERPICKER_OPACITYSLIDER 19
#define EVENT_TOOLTEXT_POSTCONFIG 20

class EventCallbackListener {
public:
	virtual void eventGeneric(int evt_id, int data1, int data2) {}
	virtual void eventGeneric2(int evt_id, void* data1) {}
	virtual void eventTextInput(int evt_id, std::string data) {}
	virtual void eventTextInputConfirm(int evt_id, std::string data) {}
	virtual void eventButtonPressed(int evt_id) {}
	virtual void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex = -1) {}
	virtual void eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex = -1) {}
	virtual void eventPopupClosed(int evt_id, BasePopup* target) {}
	virtual void eventSliderPosChanged(int evt_id, float value) {}
	virtual void eventSliderPosFinishedChanging(int evt_id, float value) {}
};