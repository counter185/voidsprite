#pragma once
#include "globals.h"

#define EVENT_COLORPICKER_TEXTFIELD 1

class EventCallbackListener {
public:
	virtual void eventGeneric(int evt_id, int data1, int data2) {}
	virtual void eventGeneric2(int evt_id, void* data1) {}
	virtual void eventTextInput(int evt_id, std::string data) {}
};