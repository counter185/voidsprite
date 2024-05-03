#pragma once
#include "globals.h"

class BaseScreen
{
protected:
	int callback_id = -1;
	EventCallbackListener* callback = NULL;
public:

	virtual ~BaseScreen() {}

	virtual void render() {}
	virtual void takeInput(SDL_Event evt) {}
	virtual void tick() {}

	virtual std::string getName() { return "Base screen"; }

	void setCallbackListener(int evt_id, EventCallbackListener* callback) {
		this->callback = callback;
		this->callback_id = evt_id;
	}
};

