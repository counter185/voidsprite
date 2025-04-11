#pragma once
#include "globals.h"
#include "Timer64.h"

#define ErrorNotification(a,b) Notification(a,b,5000,g_iconNotifError,SDL_Color{0xFF,0xBA,0xBA,255})
#define SuccessNotification(a,b) Notification(a,b,4000,g_iconNotifSuccess,SDL_Color{0xD9,0xFF,0xBA,255})
#define SuccessShortNotification(a,b) Notification(a,b,1500,g_iconNotifSuccess,SDL_Color{0xD9,0xFF,0xBA,255})

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


void g_renderNotifications();
void g_tickNotifications();