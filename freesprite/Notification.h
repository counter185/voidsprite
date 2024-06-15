#pragma once
#include "globals.h"
#include "Timer64.h"

class Notification
{
public:
	std::string title;
	std::string message;
	Timer64 timer;
	int duration = 5000;

	Notification(std::string title, std::string message, int duration = 5000) : title(title), message(message) {
		this->duration = duration;
		timer.start();
	}
};

