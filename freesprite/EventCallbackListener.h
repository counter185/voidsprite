#pragma once
#include "globals.h"

#define EVENT_COLORPICKER_TEXTFIELD 1
#define EVENT_MAINEDITOR_SAVEFILE 2
#define EVENT_BRUSHPICKER_BRUSH_CHANGED 3
#define EVENT_COLORPICKER_TOGGLEERASER 4

class EventCallbackListener {
public:
	virtual void eventGeneric(int evt_id, int data1, int data2) {}
	virtual void eventGeneric2(int evt_id, void* data1) {}
	virtual void eventTextInput(int evt_id, std::string data) {}
	virtual void eventButtonPressed(int evt_id) {}
	virtual void eventFileSaved(int evt_id, std::string name) {}
};