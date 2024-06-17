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
	SDL_Texture* icon;
	SDL_Color color;

	Notification(std::string title, std::string message, int duration = 5000, SDL_Texture* icon = NULL, SDL_Color color = SDL_Color{255,255,255,255}) : title(title), message(message) {
		this->duration = duration;
		this->icon = icon;
		this->color = color;
		timer.start();
	}
};

