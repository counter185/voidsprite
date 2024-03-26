#pragma once
#include "globals.h"

#define EVENT_COLORPICKER_TEXTFIELD 1
#define EVENT_MAINEDITOR_SAVEFILE 2
#define EVENT_BRUSHPICKER_BRUSH_CHANGED 3
#define EVENT_COLORPICKER_TOGGLEERASER 4
#define EVENT_COLORPICKER_SLIDERH 5
#define EVENT_COLORPICKER_SLIDERS 6
#define EVENT_COLORPICKER_SLIDERV 7

class EventCallbackListener {
public:
	virtual void eventGeneric(int evt_id, int data1, int data2) {}
	virtual void eventGeneric2(int evt_id, void* data1) {}
	virtual void eventTextInput(int evt_id, std::string data) {}
	virtual void eventTextInputConfirm(int evt_id, std::string data) {}
	virtual void eventButtonPressed(int evt_id) {}
	virtual void eventFileSaved(int evt_id, PlatformNativePathString name) {}
	virtual void eventPopupClosed(int evt_id, BasePopup* target) {}
	virtual void eventSliderPosChanged(int evt_id, float value) {}
};