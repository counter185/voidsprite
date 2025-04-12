#include "Notification.h"
#include "FontRenderer.h"

std::vector<Notification> g_notifications;

void g_renderNotifications()
{
    int notifY = 30;
    int notifOriginX = g_windowW - 450;
    for (Notification& notif : g_notifications) {
        //background
        int notifX = notifOriginX + 30 * (1.0 - XM1PW3P1(notif.timer.percentElapsedTime(300)));
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0xf0 * XM1PW3P1(notif.timer.percentElapsedTime(200) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)))));
        SDL_Rect r = { notifX, notifY, 400, 60 };
        SDL_RenderFillRect(g_rd, &r);

        //animated border lines
        //gradient
        uint32_t color = sdlcolorToUint32(notif.color);
        XY leftEP = statLineEndpoint(XY{ r.x, r.y }, XY{ r.x, r.y + r.h }, XM1PW3P1(notif.timer.percentElapsedTime(300)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)));
        renderGradient({ r.x, r.y, r.w / 4, leftEP.y - r.y }, modAlpha(color, 0x40), modAlpha(color, 0), modAlpha(color, 0x40), modAlpha(color, 0));

        SDL_SetRenderDrawColor(g_rd, notif.color.r, notif.color.g, notif.color.b, 0x80 + (uint8_t)(0x60 * (1.0 - XM1PW3P1(notif.timer.percentElapsedTime(500)))));
        //left line
        drawLine(XY{ r.x, r.y }, leftEP);
        //right line
        drawLine(XY{ r.x + r.w, r.y + r.h }, XY{ r.x + r.w, r.y }, XM1PW3P1(notif.timer.percentElapsedTime(300)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500)));

        //icon
        int textX = notifX + 10;
        if (notif.icon != NULL) {
            SDL_Rect iconRect = { notifX + 5, notifY + 5, 50, 50 };
            SDL_SetTextureAlphaMod(notif.icon, (uint8_t)(0xff * XM1PW3P1(notif.timer.percentElapsedTime(200, 200)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500))));
            SDL_RenderCopy(g_rd, notif.icon, NULL, &iconRect);
            SDL_SetTextureAlphaMod(notif.icon, 0xff);
            textX += 50;
        }

        //text
        g_fnt->RenderString(notif.title, textX, notif.message != "" ? notifY + 5 : notifY + 15, SDL_Color{ 255,255,255,(uint8_t)(0xff * XM1PW3P1(notif.timer.percentElapsedTime(200, 100)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500))) });
        g_fnt->RenderString(notif.message, textX, notif.title != "" ? notifY + 30 : notifY + 15, SDL_Color{ 255,255,255,(uint8_t)(0xd0 * XM1PW3P1(notif.timer.percentElapsedTime(200, 150)) * (1.0 - notif.timer.percentElapsedTime(500, notif.duration - 500))) }, 16);
        notifY += 65;

        //pulse outline
        if (g_config.vfxEnabled) {
			double timer800ms = notif.timer.percentElapsedTime(800);
            SDL_SetRenderDrawColor(g_rd, notif.color.r, notif.color.g, notif.color.b, (u8)(0x80 * (1.0 - XM1PW3P1(timer800ms))));
            SDL_Rect rg = offsetRect(r, 1 + 20 * XM1PW3P1(timer800ms));
            SDL_RenderDrawRect(g_rd, &rg);

            for (int x = 1; x < 6; x++) {
                XY vline1Origin = { r.x - (int)(40 * x * XM1PW3P1(timer800ms)), r.y };
                drawLine(vline1Origin, xyAdd(vline1Origin, { 0,r.h }), 1.0);
            }
        }
    }
}

void g_addNotification(Notification a) {
    g_notifications.push_back(a);
}
void g_tickNotifications() {
    for (int x = 0; x < g_notifications.size(); x++) {
        if (g_notifications[x].timer.elapsedTime() > g_notifications[x].duration) {
            g_notifications.erase(g_notifications.begin() + x);
            x--;
        }
    }
}